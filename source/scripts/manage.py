#!/usr/bin/env python3

"""manage.py: cross-platform cmake build manager.

It only uses python stdlib modules to do the following:

- General Shell ops
- Dependency download, build, install
- Module compilation
- Wheel building
- Alternative frontend to CMake

models:
    CustomFormatter(logging.Formatter)
    MetaCommander(type)
    WheelFile(dataclass)
    ShellCmd
        Project
        AbstractBuilder
            Builder
                LlamaCppBuilder
                LlamaCppPythonBuilder
                WhisperCppBuilder
                StableDiffusionCppBuilder
        WheelBuilder
        Application(meta=MetaCommander)


It has an argparse-based cli api:

usage: manage.py [-h] [-v]  ...

cyllama build manager

    clean        clean detritus
    build        build application
    setup        setup prerequisites
    test         test modules
    wheel        build wheels
"""

import argparse
import logging
import os
import platform
import re
import shutil
import stat
import subprocess
import sys
import tarfile
import zipfile
from fnmatch import fnmatch
from dataclasses import dataclass
from pathlib import Path
from typing import List, Optional, Union, Callable
from urllib.request import urlretrieve

PYTHON = sys.executable
PLATFORM = platform.system()
ARCH = platform.machine()

__version__ = "0.0.1"

# ----------------------------------------------------------------------------
# type aliases

Pathlike = Union[str, Path]
MatchFn = Callable[[Path], bool]
ActionFn = Callable[[Path], None]

# ----------------------------------------------------------------------------
# env helpers

def getenv(key: str, default: bool = False) -> bool:
    """convert '0','1' env values to bool {True, False}"""
    return bool(int(os.getenv(key, default)))

def setenv(key: str, default: str):
    """get environ variable if it is exists else set default"""
    if key in os.environ:
        return os.getenv(key, default)
    else:
        os.environ[key] = default
        return default

# ----------------------------------------------------------------------------
# constants

PYTHON = sys.executable
PLATFORM = platform.system()
ARCH = platform.machine()
PY_VER_MINOR = sys.version_info.minor
if PLATFORM == "Darwin":
    MACOSX_DEPLOYMENT_TARGET = setenv("MACOSX_DEPLOYMENT_TARGET", "12.6")
DEFAULT_PY_VERSION = "3.13.5"
DEBUG = getenv("DEBUG", default=True)
COLOR = getenv("COLOR", default=True)

# ----------------------------------------------------------------------------
# logging config

class CustomFormatter(logging.Formatter):
    """custom logging formatting class"""

    white = "\x1b[97;20m"
    grey = "\x1b[38;20m"
    green = "\x1b[32;20m"
    cyan = "\x1b[36;20m"
    yellow = "\x1b[33;20m"
    red = "\x1b[31;20m"
    bold_red = "\x1b[31;1m"
    reset = "\x1b[0m"
    fmt = "%(asctime)s - {}%(levelname)s{} - %(name)s.%(funcName)s - %(message)s"

    FORMATS = {
        logging.DEBUG: fmt.format(grey, reset),
        logging.INFO: fmt.format(green, reset),
        logging.WARNING: fmt.format(yellow, reset),
        logging.ERROR: fmt.format(red, reset),
        logging.CRITICAL: fmt.format(bold_red, reset),
    }

    def format(self, record):
        log_fmt = self.FORMATS.get(record.levelno)
        formatter = logging.Formatter(log_fmt, datefmt="%H:%M:%S")
        return formatter.format(record)


handler = logging.StreamHandler()
handler.setFormatter(CustomFormatter())
logging.basicConfig(level=logging.DEBUG if DEBUG else logging.INFO, handlers=[handler])


# ----------------------------------------------------------------------------
# utility classes

class ShellCmd:
    """Provides platform agnostic file/folder handling."""

    log: logging.Logger

    def cmd(self, shellcmd: str, cwd: Pathlike = "."):
        """Run shell command within working directory"""
        self.log.info(shellcmd)
        try:
            subprocess.check_call(shellcmd, shell=True, cwd=str(cwd))
        except subprocess.CalledProcessError:
            self.log.critical("", exc_info=True)
            sys.exit(1)

    def download(self, url: str, tofolder: Optional[Pathlike] = None) -> Pathlike:
        """Download a file from a url to an optional folder"""
        _path = Path(os.path.basename(url))
        if tofolder:
            _path = Path(tofolder).joinpath(_path)
            if _path.exists():
                return _path
        filename, _ = urlretrieve(url, filename=_path)
        return Path(filename)

    def extract(self, archive: Pathlike, tofolder: Pathlike = "."):
        """extract a tar archive"""
        if tarfile.is_tarfile(archive):
            with tarfile.open(archive) as f:
                f.extractall(tofolder)
        elif zipfile.is_zipfile(archive):
            with zipfile.ZipFile(archive) as f:
                f.extractall(tofolder)
        else:
            raise TypeError("cannot extract from this file.")

    def fail(self, msg, *args):
        """exits the program with an error msg."""
        self.log.critical(msg, *args)
        sys.exit(1)

    def git_clone(
        self,
        url: str,
        branch: Optional[str] = None,
        directory: Optional[Pathlike] = None,
        recurse: bool = False,
        cwd: Pathlike = ".",
    ):
        """git clone a repository source tree from a url"""
        _cmds = ["git clone --depth 1"]
        if branch:
            _cmds.append(f"--branch {branch}")
        if recurse:
            _cmds.append("--recurse-submodules --shallow-submodules")
        _cmds.append(url)
        if directory:
            _cmds.append(str(directory))
        self.cmd(" ".join(_cmds), cwd=cwd)

    def getenv(self, key: str, default: bool = False) -> bool:
        """convert '0','1' env values to bool {True, False}"""
        self.log.info("checking env variable: %s", key)
        return bool(int(os.getenv(key, default)))

    def chdir(self, path: Pathlike):
        """Change current workding directory to path"""
        self.log.info("changing working dir to: %s", path)
        os.chdir(path)

    def chmod(self, path: Pathlike, perm=0o777):
        """Change permission of file"""
        self.log.info("change permission of %s to %s", path, perm)
        os.chmod(path, perm)

    def get(self, shellcmd, cwd: Pathlike = ".", shell: bool = False) -> str:
        """get output of shellcmd"""
        if not shell:
            shellcmd = shellcmd.split()
        return subprocess.check_output(
            shellcmd, encoding="utf8", shell=shell, cwd=str(cwd)
        ).strip()

    def makedirs(self, path: Pathlike, mode: int = 511, exist_ok: bool = True):
        """Recursive directory creation function"""
        self.log.info("making directory: %s", path)
        os.makedirs(path, mode, exist_ok)

    def move(self, src: Pathlike, dst: Pathlike):
        """Move from src path to dst path."""
        self.log.info("move path %s to %s", src, dst)
        shutil.move(src, dst)

    def copy(self, src: Pathlike, dst: Pathlike):
        """copy file or folders -- tries to be behave like `cp -rf`"""
        self.log.info("copy %s to %s", src, dst)
        src, dst = Path(src), Path(dst)
        if src.is_dir():
            shutil.copytree(src, dst)
        else:
            shutil.copy2(src, dst)

    def remove(self, path: Pathlike, silent: bool = False):
        """Remove file or folder."""

        # handle windows error on read-only files
        def remove_readonly(func, path, exc_info):
            "Clear the readonly bit and reattempt the removal"
            if PY_VER_MINOR < 11:
                if func not in (os.unlink, os.rmdir) or exc_info[1].winerror != 5:
                    raise exc_info[1]
            else:
                if func not in (os.unlink, os.rmdir) or exc_info.winerror != 5:
                    raise exc_info
            os.chmod(path, stat.S_IWRITE)
            func(path)

        path = Path(path)
        if path.is_dir():
            if not silent:
                self.log.info("remove folder: %s", path)
            if PY_VER_MINOR < 11:
                shutil.rmtree(path, ignore_errors=not DEBUG, onerror=remove_readonly)
            else:
                shutil.rmtree(path, ignore_errors=not DEBUG, onexc=remove_readonly)
        else:
            if not silent:
                self.log.info("remove file: %s", path)
            try:
                path.unlink()
            except FileNotFoundError:
                if not silent:
                    self.log.warning("file not found: %s", path)

    def walk(
        self,
        root: Pathlike,
        match_func: MatchFn,
        action_func: ActionFn,
        skip_patterns: list[str],
    ):
        """general recursive walk from root path with match and action functions"""
        for root_, dirs, filenames in os.walk(root):
            _root = Path(root_)
            if skip_patterns:
                for skip_pat in skip_patterns:
                    if skip_pat in dirs:
                        dirs.remove(skip_pat)
            for _dir in dirs:
                current = _root / _dir
                if match_func(current):
                    action_func(current)

            for _file in filenames:
                current = _root / _file
                if match_func(current):
                    action_func(current)

    def glob_copy(
        self,
        src: Pathlike,
        dest: Pathlike,
        patterns: list[str],
    ):
        """copy glob patterns from src dir to destination dir"""

        src = Path(src)
        dest = Path(dest)

        if not src.exists():
            raise IOError(f"src dir '{src}' not found")

        if not dest.exists():
            dest.mkdir()

        for p in patterns:
            for f in src.glob(p):
                self.copy(f, dest)

    def glob_remove(self, root: Pathlike, patterns: list[str], skip_dirs: list[str]):
        """applies recursive glob remove using a list of patterns"""

        def _match(entry: Path) -> bool:
            # return any(fnmatch(entry, p) for p in patterns)
            return any(fnmatch(entry.name, p) for p in patterns)

        def remove(entry: Path):
            self.remove(entry)

        self.walk(root, match_func=_match, action_func=remove, skip_patterns=skip_dirs)

    def pip_install(
        self,
        *pkgs,
        reqs: Optional[str] = None,
        upgrade: bool = False,
        pip: Optional[str] = None,
    ):
        """Install python packages using pip"""
        _cmds = []
        if pip:
            _cmds.append(pip)
        else:
            _cmds.append("pip3")
        _cmds.append("install")
        if reqs:
            _cmds.append(f"-r {reqs}")
        else:
            if upgrade:
                _cmds.append("--upgrade")
            _cmds.extend(pkgs)
        self.cmd(" ".join(_cmds))

    def apt_install(self, *pkgs, update: bool = False):
        """install debian packages using apt"""
        _cmds = []
        _cmds.append("sudo apt install")
        if update:
            _cmds.append("--upgrade")
        _cmds.extend(pkgs)
        self.cmd(" ".join(_cmds))

    def brew_install(self, *pkgs, update: bool = False):
        """install using homebrew"""
        _pkgs = " ".join(pkgs)
        if update:
            self.cmd("brew update")
        self.cmd(f"brew install {_pkgs}")

    def cmake_config(self, src_dir: Pathlike, build_dir: Pathlike, *scripts, **options):
        """activate cmake configuration / generation stage"""
        _cmds = [f"cmake -S {src_dir} -B {build_dir}"]
        if scripts:
            _cmds.append(" ".join(f"-C {path}" for path in scripts))
        if options:
            _cmds.append(" ".join(f"-D{k}={v}" for k, v in options.items()))
        self.cmd(" ".join(_cmds))

    def cmake_build(self, build_dir: Pathlike, release: bool = False):
        """activate cmake build stage"""
        _cmd = f"cmake --build {build_dir}"
        if release:
            _cmd += " --config Release"
        self.cmd(_cmd)

    def cmake_install(self, build_dir: Pathlike, prefix: Optional[str] = None):
        """activate cmake install stage"""
        _cmds = ["cmake --install", str(build_dir)]
        if prefix:
            _cmds.append(f"--prefix {prefix}")
        self.cmd(" ".join(_cmds))


# ----------------------------------------------------------------------------
# main classes

class Project(ShellCmd):
    """Utility class to hold project directory structure"""

    def __init__(self):
        self.cwd = Path.cwd()
        self.build = self.cwd / "build"
        # self.src = self.build / "repos"
        self.src = self.build
        self.thirdparty = self.cwd / "thirdparty"
        self.install = self.thirdparty
        self.dist = self.cwd / "dist"
        self.scripts = self.cwd / "scripts"
        self.tests = self.cwd / "tests"

    def setup(self):
        """create main project directories"""
        # self.bin.mkdir(exist_ok=True)
        self.build.mkdir(exist_ok=True)
        self.src.mkdir(exist_ok=True)
        self.install.mkdir(exist_ok=True)

    def clean(self):
        """prepare project for a partial rebuild"""
        self.remove(self.build)
        self.remove(self.dist)

    def reset(self):
        """prepare project for a full rebuild"""
        self.clean()
        self.remove(self.install)


class AbstractBuilder(ShellCmd):
    """Abstract builder class with additional methods common to subclasses."""

    name: str
    version: str
    repo_url: str
    download_url_template: str
    libs_static: list[str]
    depends_on: list[type["Builder"]]

    def __init__(
        self, version: Optional[str] = None, project: Optional[Project] = None
    ):
        self.version = version or self.version
        self.project = project or Project()
        self.log = logging.getLogger(self.__class__.__name__)

    def __repr__(self):
        return f"<{self.__class__.__name__} '{self.name}-{self.version}'>"

    # def __iter__(self):
    #     for dependency in self.depends_on:
    #         yield dependency
    #         yield from iter(dependency)

    @property
    def ver(self) -> str:
        """short python version: 3.11"""
        return ".".join(self.version.split(".")[:2])

    @property
    def ver_major(self) -> str:
        """major compoent of semantic version: 3 in 3.11.7"""
        return self.version.split(".")[0]

    @property
    def ver_minor(self) -> str:
        """minor compoent of semantic version: 11 in 3.11.7"""
        return self.version.split(".")[1]

    @property
    def ver_patch(self) -> str:
        """patch compoent of semantic version: 7 in 3.11.7"""
        return self.version.split(".")[2]

    @property
    def ver_nodot(self) -> str:
        """concat major and minor version components: 311 in 3.11.7"""
        return self.ver.replace(".", "")

    @property
    def name_version(self) -> str:
        """return name-<fullversion>: e.g. Python-3.11.7"""
        return f"{self.name}-{self.version}"

    @property
    def name_ver(self) -> str:
        """return name.lower-<ver>: e.g. python3.11"""
        return f"{self.name.lower()}{self.ver}"

    @property
    def download_url(self) -> str:
        """return download url with version interpolated"""
        return self.download_url_template.format(ver=self.version)

    @property
    def repo_branch(self) -> str:
        """return repo branch"""
        return self.name.lower()

    @property
    def src_dir(self) -> Path:
        """return extracted source folder of build target"""
        return self.project.src / self.name

    @property
    def build_dir(self) -> Path:
        """return 'build' folder src dir of build target"""
        return self.src_dir / "build"

    @property
    def prefix(self) -> Path:
        """builder prefix path"""
        return self.project.install / self.name.lower()

    @property
    def bin(self) -> Path:
        """builder bin path"""
        return self.prefix / "bin"

    @property
    def include(self) -> Path:
        """builder include path"""
        return self.prefix / "include"

    @property
    def lib(self) -> Path:
        """builder lib path"""
        return self.prefix / "lib"

    @property
    def executable_name(self) -> str:
        """executable name of buld target"""
        name = self.name.lower()
        if PLATFORM == "Windows":
            name = f"{self.name}.exe"
        return name

    @property
    def executable(self) -> Path:
        """executable path of buld target"""
        return self.bin / self.executable_name

    @property
    def libname(self) -> str:
        """library name prefix"""
        return f"lib{self.name}"

    @property
    def staticlib_name(self) -> str:
        """static libname"""
        suffix = ".a"
        if PLATFORM == "Windows":
            suffix = ".lib"
        return f"{self.libname}{suffix}"

    @property
    def dylib_name(self) -> str:
        """dynamic link libname"""
        if PLATFORM == "Darwin":
            return f"{self.libname}.dylib"
        if PLATFORM == "Linux":
            return f"{self.libname}.so"
        if PLATFORM == "Windows":
            return f"{self.libname}.dll"
        return self.fail("platform not supported")

    @property
    def dylib_linkname(self) -> str:
        """symlink to dylib"""
        if PLATFORM == "Darwin":
            return f"{self.libname}.dylib"
        if PLATFORM == "Linux":
            return f"{self.libname}.so"
        return self.fail("platform not supported")

    @property
    def dylib(self) -> Path:
        """dylib path"""
        return self.lib / self.dylib_name

    @property
    def dylib_link(self) -> Path:
        """dylib link path"""
        return self.lib / self.dylib_linkname

    @property
    def staticlib(self) -> Path:
        """staticlib path"""
        return self.lib / self.staticlib_name

    def libs_static_exist(self) -> bool:
        """check if all built stati libs already exist"""
        return all((self.lib / lib).exists() for lib in self.libs_static)

    def pre_process(self):
        """override by subclass if needed"""

    def setup(self):
        """setup build environment"""

    def configure(self):
        """configure build"""

    def build(self, shared: bool = False):
        """build target"""

    def install(self):
        """install target"""

    def clean(self):
        """clean build"""

    def post_process(self):
        """override by subclass if needed"""

    def process(self):
        """main builder process"""
        self.pre_process()
        self.setup()
        self.configure()
        self.build()
        self.install()
        self.clean()
        self.post_process()


class Builder(AbstractBuilder):
    """concrete builder class"""

    def setup(self):
        """setup build environment"""
        self.log.info(f"update from {self.name} main repo")
        self.project.setup()
        if self.version:
            self.git_clone(
                self.repo_url, branch=self.version, recurse=True, cwd=self.project.src
            )
        else:
            self.git_clone(self.repo_url, recurse=True, cwd=self.project.src)


# ----------------------------------------------------------------------------
# argdeclare


# option decorator
def option(*args, **kwds):
    def _decorator(func):
        _option = (args, kwds)
        if hasattr(func, "options"):
            func.options.append(_option)
        else:
            func.options = [_option]
        return func

    return _decorator


# bool option decorator
def opt(long, short, desc, **kwargs):
    return option(long, short, help=desc, action="store_true", **kwargs)


# arg decorator
arg = option


# combines option decorators
def option_group(*options):
    def _decorator(func):
        for option in options:
            func = option(func)
        return func

    return _decorator


class MetaCommander(type):
    def __new__(cls, classname, bases, classdict):
        classdict = dict(classdict)
        subcmds = {}
        for name, func in list(classdict.items()):
            if name.startswith("do_"):
                name = name[3:]
                subcmd = {"name": name, "func": func, "options": []}
                if hasattr(func, "options"):
                    subcmd["options"] = func.options
                subcmds[name] = subcmd
        classdict["_argparse_subcmds"] = subcmds
        return type.__new__(cls, classname, bases, classdict)    


class Application(ShellCmd, metaclass=MetaCommander):
    """cyllama build manager"""

    version = "0.0.4"
    epilog = ""
    default_args = ["--help"]

    def __init__(self):
        self.project = Project()
        self.log = logging.getLogger(self.__class__.__name__)

    def parse_args(self):
        parser = argparse.ArgumentParser(
            # prog = self.name,
            formatter_class=argparse.RawDescriptionHelpFormatter,
            description=self.__doc__,
            epilog=self.epilog,
        )
        return parser

    def cmdline(self):
        self.parser = self.parse_args()

        self.parser.add_argument(
            "-v", "--version", action="version", version="%(prog)s " + self.version
        )

        subparsers = self.parser.add_subparsers(
            title="subcommands",
            description="valid subcommands",
            help="additional help",
            metavar="",
        )

        for name in sorted(self._argparse_subcmds.keys()):
            subcmd = self._argparse_subcmds[name]
            subparser = subparsers.add_parser(
                subcmd["name"], help=subcmd["func"].__doc__
            )
            for args, kwds in subcmd["options"]:
                subparser.add_argument(*args, **kwds)
            subparser.set_defaults(func=subcmd["func"])

        if len(sys.argv) <= 1:
            options = self.parser.parse_args(self.default_args)
        else:
            options = self.parser.parse_args()

        self.options = options
        options.func(self, options)

    # ------------------------------------------------------------------------
    # setup

    def do_setup(self, args):
        """setup prerequisites"""
        # for Builder in [LlamaCppBuilder, WhisperCppBuilder, StableDiffusionCppBuilder]:
        for Builder in []:
            builder = Builder()
            builder.setup()


    # ------------------------------------------------------------------------
    # build

    @opt("-s", "--shared",  "build shared libraries")
    @opt("-a", "--all", "build all")
    def do_build(self, args):
        """build packages"""
        _builders = []

        for Builder in _builders:
            builder = Builder()
            builder.build()

        _cmd = f'"{PYTHON}" setup.py build_ext --inplace'
        if not args.shared:
            os.environ["STATIC"] = "1"
        self.cmd(_cmd)

    # ------------------------------------------------------------------------
    # test

    @opt("--pytest", "-p", "run pytest")
    def do_test(self, args):
        """test modules"""
        if args.pytest:
            self.cmd("pytest -vv tests")
        else:
            for t in self.project.tests.glob("test_*.py"):
                self.cmd(f'"{PYTHON}" {t}')

    # ------------------------------------------------------------------------
    # clean

    @opt("--reset", "-r", "reset project")
    @opt("--verbose", "-v", "verbose cleaning ops")
    def do_clean(self, args):
        """clean detritus"""
        cwd = self.project.cwd
        _targets = ["build", "dist", "venv", "MANIFEST.in", ".task"]
        if args.reset:
            _targets += ["python", "bin", "lib", "share", "wheels"]
        _pats = [".*_cache", "*.egg-info", "__pycache__", ".DS_Store"]
        for t in _targets:
            self.remove(cwd / t, silent=not args.verbose)
        for p in _pats:
            for m in cwd.glob(p):
                self.remove(m, silent=not args.verbose)
            for m in cwd.glob("**/" + p):
                self.remove(m, silent=not args.verbose)


if __name__ == "__main__":
    Application().cmdline()


