#!/usr/bin/python3

import sys
import os
import platform
import subprocess
import posixpath

failures = 0
successes = 0
skipped = 0
skipTests = {}

# macos path
chugl_path = "--chugin:./build-release/ChuGL.chug"

if "CYGWIN" in platform.system() or "Windows" in platform.system():
    chugl_path = "--chugin:./build/Release/ChuGL.chug"
    # chugl_path = " "


exe = 'chuck'

def handle_directory(dir):
    print("")
    print(">>> Performing tests in %s <<<" % dir)

    global successes

    for filename in sorted(os.listdir(dir)):
        path = os.path.join(dir, filename)
        # 1.4.1.1 (added) nshaheed
        # split path into its components and then rejoin cast as a posix path
        path_components = os.path.normpath(path).lstrip(os.path.sep).split(os.path.sep)
        path = posixpath.join(*path_components)

        if os.path.isfile(path):
            if os.path.splitext(filename)[1] == ".ck":
                run_test(path, filename)
        
        # uncomment to test in subdirectories
        # elif os.path.isdir(path) and filename[0] != '.':
        #     handle_directory(path)


def run_test(path, filename):
    global successes
    global skipped

    print("> %s %s %s" % (exe, chugl_path, path))

    if filename in skipTests:
        skipped += 1
        return

    # NOTE: each command line argument is a new arg to check_output
    # e.g., chuck_output([exe, "--silent", "--disable-error-show-code", ...
    # NOTE: use --disable-error-show-code to not showing code on error
    # NOTE: use --no-color to explicitly disable ANSI escape codes (e.g.,
    # for color terminal text) from showing up in chuck output;
    # FYI chuck implicitly checks for TTY; if not will disable printing
    # escape codes; but leaving this note here FYI
    result = ""
    if chugl_path:
        result = subprocess.check_output([exe, chugl_path, "./test/T.ck",  path], stderr=subprocess.STDOUT).decode("utf-8")
    else:
        result = subprocess.check_output([exe, "./test/T.ck", path], stderr=subprocess.STDOUT).decode("utf-8")

    if os.path.isfile(path.replace(".ck", ".txt")):
        with open(path.replace(".ck", ".txt")) as answerFile:
            answer = answerFile.read()

        if answer.replace("\r\n", "\n") != result.replace("\r\n", "\n"):
            fail(filename, result)
        else:
            successes += 1
    else: 
        if result == "":
            successes += 1
        else:
            fail(filename, result)


def fail(test_name, output):
    global failures
    print("*** Test '%s' failed: ***" % test_name)
    print(output)
    failures += 1


def main():
    global chugl_path
    test_dir = './test/chugl-tests'

    if len(sys.argv) >= 2:
        chugl_path = sys.argv[1]

    print("=============Running ChuGL Tests=============")
    handle_directory(test_dir)

    if skipped > 0:
        print("Skipped " + str(skipped) + " tests: " + str(skipTests))

    if failures == 0:
        print("Success - all " + str(successes) + " tests passed!")
        sys.exit(0)
    else:
        print("Failure - " + str(failures) + " test(s) failed")
        sys.exit(-1)


if __name__ == "__main__":
    main()
