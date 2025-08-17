//-----------------------------------------------------------------------------
// ChuMP (ChucK Manager of Programs) CLI main file
// author: Nick Shaheed
//   date: Fall 2024
//-----------------------------------------------------------------------------
#include "chuck_version.h"
#include "exec.h"
#include "manager.h"
#include "util.h"

#include <filesystem>
#include <sstream>
#include <string>
#include <vector>

#ifdef _WIN32
#include <Windows.h>
#endif

namespace fs = std::filesystem;
using std::cerr;
using std::string;
using std::vector;

#define BASE_URL "https://chuck.stanford.edu/release/chump/manifest/"

// indent string
static const string INDENT = "    ";
const string VERSION = "v0.0.2";

// function prototypes
void printUsage();

//-----------------------------------------------------------------------------
// name: struct ChumpArgTokens
// desc: chumpian set of tokenized args
//-----------------------------------------------------------------------------
struct ChumpArgTokens {
public:
  // get group name (first token, if there is one)
  std::string name() const;
  // test if an option string was specified
  t_CKBOOL has(const std::string &option) const;
  // test if an option was specified (in either form)
  t_CKBOOL has(const std::string &opt, const std::string &optAlt) const;
  // clear name and tokens
  void clear() { m_tokens.clear(); }
  // get options tokens as a vector
  const std::vector<std::string> &tokens() const { return m_tokens; }
  // get vector of non-option targets
  std::vector<std::string> targets() const;
  // append a token
  void append(const string &token) { m_tokens.push_back(token); }

public:
  // parsed options are in here
  std::vector<std::string> m_tokens;
};

//-----------------------------------------------------------------------------
// name: struct ChumpArgParser
// desc: chumpian argument parser
//-----------------------------------------------------------------------------
struct ChumpArgParser {
public:
  // parse command line argument; expect element 0 to be the app
  t_CKBOOL parse(int argc, const char **argv);
  // get options
  const ChumpArgTokens &options() const { return m_options; }
  // get command
  const ChumpArgTokens &command() const { return m_command; }

public:
  // get chump option
  t_CKBOOL getChumpOption(const string &opt);
  // get chump option (match either opt or optAlt)
  t_CKBOOL getChumpOption(const string &opt, const string &optAlt);
  // get non-option command target, e.g., the <package> to update
  string getCommandTarget(const string &command);
  // get non-option command targets, e.g., the <package> to update
  vector<string> getCommandTargets();
  // get command option, e.g., --installed for chump list
  t_CKBOOL getCommandOption(const string &command, const string &opt);
  // get command option (match either opt or optAlt)
  t_CKBOOL getCommandOption(const string &command, const string &opt,
                            const string &optAlt);
  // // get command options, e.g., --installed for chump list
  // vector<string> getCommandOptions();

protected:
  // [options]
  ChumpArgTokens m_options;
  // [command] tokens
  ChumpArgTokens m_command;
};

//-----------------------------------------------------------------------------
// name: main()
// desc: command line chuck entry point
//-----------------------------------------------------------------------------
int main(int argc, const char **argv) {
#ifdef _WIN32
  // Windows cmd lines (powershell and cmd) need to be set up to handle utf8
  // Set console code page to UTF-8 so console known how to interpret string
  // data
  SetConsoleOutputCP(CP_UTF8);

  // Enable buffering to prevent VS from chopping up UTF-8 byte sequences
  setvbuf(stdout, nullptr, _IOFBF, 1000);
#endif
  // check if we output to TTY (teletype interactive terminal)
  // if not disable color teriminal to avoid outputing
  // ANSI escape codes to the output stream, which would
  // show up in | and >
  t_CKBOOL colorTerminal = ck_isatty();
  // set the color terminal global setting
  TC::globalDisableOverride(!colorTerminal);

  // a chump parser
  ChumpArgParser parser;
  // parse args
  if (!parser.parse(argc, argv))
    return -1;

  // help / about
  if (parser.getChumpOption("--help", "-h") ||
      parser.getChumpOption("--about")) {
    // print usage
    printUsage();
    // peace out
    return 0;
  }
  if (parser.getChumpOption("--version", "-v")) {
    // print version no.
    cerr << "chump " + TC::orange(VERSION, TRUE) << endl;
    // give up the ghost
    return 0;
  }

  /****************************************************************
   * Chump setup
   ****************************************************************/

  // get chump dir
  fs::path path = chumpDir();
  // create it
  fs::create_directories(path);
  // manager pointer
  Manager *manager = NULL;

  // build manager and run command
  fs::path pkg_path = chumpDir() / "manifest.json";
  // URL to the manifest file

  std::string manifest_url = manifestURL(BASE_URL);

  // info package name
  string info_package_name = parser.getCommandTarget("info");
  // list -i
  bool installed_flag = parser.getCommandOption("list", "-i", "--installed");
  // install package names
  vector<string> install_package_names = parser.getCommandTargets();
  // uninstall package names
  vector<string> uninstall_package_names = parser.getCommandTargets();
  // force -f
  bool uninstall_force = parser.getCommandOption("uninstall", "-f", "--force");
  // update package name
  string update_package_name = parser.getCommandTarget("update");

  // the subcommand
  string subcommand = parser.command().name();
  size_t n_targets = parser.getCommandTargets().size();

  // if there's no subcommand then print error message
  if (subcommand == "") {
    cerr << "[chump]: " << TC::orange("insufficient arguments...", TRUE)
         << endl;
    cerr << "(run `chump --help` for more information)" << endl;
    return 0;
  }

  // validate subcommand is real and error out. this needs to be done before the
  // manifest is validated
  if (subcommand != "help" && subcommand != "list" && subcommand != "info" &&
      subcommand != "install" && subcommand != "install-local" &&
      subcommand != "uninstall" && subcommand != "update" &&
      subcommand != "logo") {
    cerr << "[chump]: " << TC::blue(subcommand, TRUE)
         << TC::orange(" is not a valid chump command...", TRUE) << endl;
    cerr << "(run `chump --help` for more information)" << endl;

    return 0;
  }

  // help subcommand doesn't need manager initialization
  if (subcommand == "help") {
    if (n_targets != 0) {
      cerr << "[chump]: " << TC::blue(subcommand, TRUE)
           << TC::orange(" does not support subcommands...", TRUE) << endl;
      cerr << "(run `chump --help` for more information)" << endl;
      return 1;
    }
    // print usage
    printUsage();
    // peace out
    return 0;
  }

  try {
    // create manager
    manager =
        new Manager(pkg_path.string(), path, ChuckVersion::makeSystemVersion(),
                    ApiVersion::makeSystemVersion(), whichOS(), whichArch(),
                    manifest_url, true);

    if (!validate_manifest(pkg_path)) {
      throw std::invalid_argument("Manifest file (" + pkg_path.string() +
                                  "/manifest.json) is invalid");
    }
  } catch (const std::exception &e) {
    // print error message
    std::cerr << e.what() << std::endl;
    // done
    return 1;
  }

  // match subcommands
  if (subcommand == "info") {
    // check
    if (info_package_name == "") {
      cerr << "[chump]: " << TC::blue(subcommand, TRUE)
           << TC::orange(" requires additional argument...", TRUE) << endl;
      cerr << "(run `chump --help` for more information)" << endl;
      return 1;
    } else if (n_targets != 1) {
      cerr << "[chump]: " << TC::blue(subcommand, TRUE)
           << TC::orange(" has too many arguments, follows the form 'chump "
                         "info <package-name>'...",
                         TRUE)
           << endl;
      cerr << "(run `chump --help` for more information)" << endl;
      return 1;
    }

    // get optional package
    optional<Package> pkg = manager->getPackage(info_package_name);
    // check
    if (!pkg) {
      std::cerr << "unable to find package " << info_package_name << std::endl;
      return 1;
    }
    printPackage(manager, pkg.value());
  } else if (subcommand == "list") {
    if (n_targets != 0) {
      cerr << "[chump]: " << TC::blue(subcommand, TRUE)
           << TC::orange(" does not support subcommands...", TRUE) << endl;
      cerr << "(run `chump --help` for more information)" << endl;
      return 1;
    }

    printPackages(manager, installed_flag);
  } else if (subcommand == "install") {
    // check
    if (install_package_names.size() == 0) {
      cerr << "[chump]: " << TC::blue(subcommand, TRUE)
           << TC::orange(" requires additional argument...", TRUE) << endl;
      cerr << "(run `chump --help` for more information)" << endl;
      return 1;
    }

    // install a package by name
    for (string install_package_name : install_package_names) {
      try {
        manager->install(install_package_name);
      } catch (const std::exception &e) {
        cerr << e.what() << endl;
      }
    }
  } else if (subcommand == "install-local") {
    vector<string> targets = parser.command().targets();

    // validate target size
    if (targets.size() != 3) {
      cerr << "[chump]: " << TC::blue(subcommand, TRUE)
           << TC::orange(" requires exactly 3 arguments...", TRUE) << endl;
      cerr << "(run `chump --help` for more information)" << endl;
      return 1;
    }

    // install a package by name
    manager->install_local(targets[0], targets[1], targets[2]);
  } else if (subcommand == "uninstall") {
    // check
    if (install_package_names.size() == 0) {
      cerr << "[chump]: " << TC::blue(subcommand, TRUE)
           << TC::orange(" requires additional argument...", TRUE) << endl;
      cerr << "(run `chump --help` for more information)" << endl;
      return 1;
    }

    // uninstall a package by name
    for (string uninstall_package_name : uninstall_package_names) {
      try {
        manager->uninstall(uninstall_package_name, uninstall_force);
      } catch (const std::exception &e) {
        cerr << e.what() << endl;
      }
    }
  } else if (subcommand == "update") {
    if (n_targets != 1) {
      cerr << "[chump]: " << TC::blue(subcommand, TRUE)
           << TC::orange(" requires additional argument...", TRUE) << endl;
      cerr << "(run `chump --help` for more information)" << endl;
      return 1;
    }

    // check
    if (update_package_name == "") {
      cerr << "[chump]: " << TC::blue(subcommand, TRUE)
           << TC::orange(" requires additional argument...", TRUE) << endl;
      cerr << "(run `chump --help` for more information)" << endl;
      return 1;
    }
    // update package
    manager->update(update_package_name);
  } else if (subcommand == "logo") {
    // get target
    string target = parser.getCommandTarget("logo");

    // check
    if (target == "") {
      cerr << "[chump]: " << TC::blue(subcommand, TRUE)
           << TC::orange(" requires additional argument...", TRUE) << endl;
      cerr << "(run `chump --help` for more information)" << endl;
      return 1;
    } else if (n_targets != 1) {
      cerr << "[chump]: " << TC::blue(subcommand, TRUE)
           << TC::orange(" has too many arguments, follows the form 'chump "
                         "logo <mode>'...",
                         TRUE)
           << endl;
      cerr << "(run `chump --help` for more information)" << endl;
      return 1;
    }

    // check target
    if (target == "cereal") {
      int counter = 0;
      while (true) {
        std::string logo = jumbleLogo(counter);
        std::cout << logo;

        auto num_lines = std::count(logo.begin(), logo.end(), '\n');
        for (int i = 0; i < num_lines; i++) {
          std::cout << "\x1b[F"; // move the cursor up one line
        }
        counter++;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
      }
    } else if (target == "river") {
      int counter = 0;
      while (true) {
        std::string logo = riverLogo(counter);
        std::cout << logo;

        auto num_lines = std::count(logo.begin(), logo.end(), '\n');

        for (int i = 0; i < num_lines; i++) {
          std::cout << "\x1b[F"; // Move the cursor up one line
        }
        counter++;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
      }
    } else if (target == "bedtime") {
      int counter = 0;
      while (true) {
        std::string logo = bedtimeLogo(counter);

        int width = 0, height = 0;
        get_terminal_size(width, height);

        size_t i = 0;
        while (i < logo.length()) {
          std::cout << logo.substr(i, width);
          i += width;
          std::this_thread::sleep_for(std::chrono::milliseconds(2));
        }

        counter++;
      }
    } else if (target == "dim") {
      clear();
      int counter = 0;
      while (true) {
        std::string logo = dimLogo(counter);
        std::cout << logo;

        auto num_lines = std::count(logo.begin(), logo.end(), '\n');

        for (int i = 0; i < num_lines; i++) {
          std::cout << "\x1b[F"; // Move the cursor up one line
        }
        counter++;
        std::this_thread::sleep_for(std::chrono::milliseconds(120));
      }
    }
  } else if (subcommand != "") {
    cerr << "[chump]: " << TC::blue(subcommand, TRUE)
         << TC::orange(" is not a valid chump command...", TRUE) << endl;
    cerr << "(run `chump --help` for more information)" << endl;
  } else {
    cerr << "[chump]: " << TC::orange("insufficient arguments...", TRUE)
         << endl;
    cerr << "(run `chump --help` for more information)" << endl;
  }

  // done
  return 0;
}

//-----------------------------------------------------------------------------
// name: printUsage()
// desc: print usage for chump
//-----------------------------------------------------------------------------
void printUsage() {
  // logo + usage line
  string usage = printLogo() + "\n" + "usage: " + TC::bold("chump") + " " +
                 TC::orange("[options]", TRUE) + " " +
                 TC::blue("[command]", TRUE);
  // print it, with two end lines
  cerr << usage << endl << endl;
  // options
  cerr << "options:" << endl;
  cerr << INDENT << TC::orange("--help/-h", TRUE)
       << "                        print this help message and exit" << endl;
  cerr << endl;
  // commands
  cerr << "commands:" << endl;
  cerr << INDENT << TC::blue("help", TRUE)
       << "                             (same as `chump --help`)" << endl;
  cerr << INDENT << TC::blue("list", TRUE)
       << "                             list available packages" << endl;
  cerr << INDENT << "  └─" << TC::blue(" --installed/-i", TRUE)
       << "                └─ list only installed packages" << endl;
  cerr << INDENT << TC::blue("info", TRUE)
       << " <package>                   display information about <package>"
       << endl;
  cerr << INDENT << TC::blue("install", TRUE)
       << " <package>                download and install <package>" << endl;
  cerr << INDENT << TC::blue("install-local", TRUE)
       << " <def> <ver> <zip>  install a locally defined package" << endl;
  cerr << INDENT << "  └─" << TC::blue(" <def>", TRUE)
       << "                         └─ json file with package metadata" << endl;
  cerr << INDENT << "  └─" << TC::blue(" <ver>", TRUE)
       << "                         └─ json file of version definition" << endl;
  cerr << INDENT << "  └─" << TC::blue(" <zip>", TRUE)
       << "                         └─ zip file of all package contents"
       << endl;
  cerr << INDENT << TC::blue("uninstall", TRUE)
       << " <package>              uninstall <package>" << endl;
  cerr << INDENT << "  └─" << TC::blue(" --force/-f", TRUE)
       << "                    └─ delete package dir (and all files inside it)"
       << endl;
  cerr << INDENT << TC::blue("update", TRUE)
       << " <package>                 update <package> to latest version"
       << endl;
  cerr << INDENT
       << "                                   (compatible with chuck version "
          "and OS)"
       << endl;
  cerr << INDENT << TC::blue("logo", TRUE)
       << " <mode>                      behold the chump logo" << endl;
  cerr << INDENT << "  └─" << TC::blue(" cereal", TRUE)
       << "                        └─ a fruity breakfast treat" << endl;
  cerr << INDENT << "  └─" << TC::blue(" river", TRUE)
       << "                         └─ flowing & ebbing" << endl;
  cerr << INDENT << "  └─" << TC::blue(" bedtime", TRUE)
       << "                       └─ zzzzz...." << endl;
  cerr << INDENT << "  └─" << TC::blue(" dim", TRUE)
       << "                           └─ what's there?" << endl;
  cerr << endl;
  cerr << "chump version: " + TC::orange(VERSION, TRUE) << endl;
  cerr << endl;
}

//-----------------------------------------------------------------------------
// name: name()
// desc: get group name (first token, if there is one)
//-----------------------------------------------------------------------------
std::string ChumpArgTokens::name() const {
  return m_tokens.size() ? m_tokens[0] : "";
}

//-----------------------------------------------------------------------------
// name: has()
// desc: test if an option was specified (in either form)
//-----------------------------------------------------------------------------
t_CKBOOL ChumpArgTokens::has(const std::string &opt,
                             const std::string &optAlt) const {
  if (opt != "" && std::find(m_tokens.begin(), m_tokens.end(), to_lower(opt)) !=
                       m_tokens.end())
    return TRUE;
  if (optAlt != "" && std::find(m_tokens.begin(), m_tokens.end(),
                                to_lower(optAlt)) != m_tokens.end())
    return TRUE;
  return FALSE;
}

//-----------------------------------------------------------------------------
// name: has()
// desc: test if an option was specified (in either form)
//-----------------------------------------------------------------------------
t_CKBOOL ChumpArgTokens::has(const std::string &opt) const {
  return has(opt, "");
}

//-----------------------------------------------------------------------------
// name: targets()
// desc: get vector of non-option targets
//-----------------------------------------------------------------------------
std::vector<std::string> ChumpArgTokens::targets() const {
  // return
  vector<string> results;

  // loop, skipping over first token (that's the command)
  for (t_CKUINT t = 1; t < m_tokens.size(); t++) {
    // skip over options (tokens that begin with -)
    if (m_tokens[t].size() == 0 || m_tokens[t][0] == '-')
      continue;
    // append token as target
    results.push_back(m_tokens[t]);
  }

  // return results
  return results;
}

//-----------------------------------------------------------------------------
// name: getChumpOption()
// desc: get chump option
//-----------------------------------------------------------------------------
t_CKBOOL ChumpArgParser::getChumpOption(const string &opt,
                                        const string &optAlt) {
  return m_options.has(opt, optAlt);
}

//-----------------------------------------------------------------------------
// name: getChumpOption()
// desc: get chump option
//-----------------------------------------------------------------------------
t_CKBOOL ChumpArgParser::getChumpOption(const string &opt) {
  return getChumpOption(opt, "");
}

//-----------------------------------------------------------------------------
// name: getCommandTarget()
// desc: get non-option command target, e.g., the <package> to update
//-----------------------------------------------------------------------------
string ChumpArgParser::getCommandTarget(const string &command) {
  // get non-option targets
  vector<string> result = m_command.targets();
  // return first element, if there is one
  return result.size() ? result[0] : "";
}

//-----------------------------------------------------------------------------
// name: getCommandTargets()
// desc: get non-option command targets, e.g., the <package> to update
//-----------------------------------------------------------------------------
vector<string> ChumpArgParser::getCommandTargets() {
  // get non-option targets
  vector<string> result = m_command.targets();
  return result;
}

//-----------------------------------------------------------------------------
// name: getCommandOption()
// desc: get command option, e.g., --installed for chump list
//-----------------------------------------------------------------------------
t_CKBOOL ChumpArgParser::getCommandOption(const string &command,
                                          const string &opt,
                                          const string &optAlt) {
  // check name
  if (m_command.name() != command)
    return FALSE;
  // check for presence of option
  return m_command.has(opt, optAlt);
}

//-----------------------------------------------------------------------------
// name: getCommandOption()
// desc: get non-option command target, e.g., the <package> to update
//-----------------------------------------------------------------------------
t_CKBOOL ChumpArgParser::getCommandOption(const string &command,
                                          const string &opt) {
  return getCommandOption(command, opt, "");
}

//-----------------------------------------------------------------------------
// enum arg parse state
//-----------------------------------------------------------------------------
enum ChumpArgState {
  ARG_START = 0,
  ARG_OPTIONS_COMMAND = 1,
  ARG_COMMAND_PAYLOAD = 2,
  ARG_DONE = 3
};
//-----------------------------------------------------------------------------
// name: parse()
// desc: parse command line argument; expect element 0 to be the app
//-----------------------------------------------------------------------------
t_CKBOOL ChumpArgParser::parse(int argc, const char **argv) {
  // reset everything
  m_options.clear();
  m_command.clear();
  // starting state
  ChumpArgState state = ARG_START;

  // go through array of args
  for (t_CKINT i = 0; i < argc; i++) {
    // check state
    switch (state) {
    case ARG_START: {
      // put app name into options
      m_options.append(argv[0]);
      // move to next state
      state = ARG_OPTIONS_COMMAND;
      break;
    }

    case ARG_OPTIONS_COMMAND: {
      // convert arg to c++ string, and lowercase it
      string arg = to_lower(argv[i]);
      // as long as we have args that start with -
      if (arg[0] == '-') {
        // append to options
        m_options.append(arg);
      } else {
        // this is a command; push to command token array
        m_command.append(arg);
        // move to command
        state = ARG_COMMAND_PAYLOAD;
      }
      break;
    }

    case ARG_COMMAND_PAYLOAD: {
      // convert arg to c++ string as is (no lower case yet)
      string arg = argv[i];
      // lower case only if option
      if (arg[0] == '-')
        arg = to_lower(arg);
      // push to command token array
      m_command.append(arg);
      break;
    }

    default: {
      // internal error
      cerr << "[chump]: (internal error) unexpected state in argument parser..."
           << endl;
      return FALSE;
    }
    }
  }

  // set state to done
  state = ARG_DONE;

  return TRUE;
}
