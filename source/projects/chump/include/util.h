#ifndef __UTIL_H__
#define __UTIL_H__

#include "chugin.h" // for chuck type defines
#include "package.h"

using std::optional;
using std::string;
using std::tuple;

namespace fs = std::filesystem;

fs::path packagePath(Package p, fs::path install_dir);

string whichOS();
Architecture whichArch();

fs::path getHomeDirectory();

fs::path chumpDir();

std::string manifestURL(std::string base_url);

tuple<string, optional<string>> parsePackageName(string packageName);

bool is_subpath(const fs::path &path, const fs::path &base);

std::string hash_file(fs::path filename);

// validate that the manifest.json is able to be loaded correctly
bool validate_manifest(fs::path manifest_path);

fs::path fileTypeToDir(FileType f);

bool unzipFile(const string &zipPath, const string &outputDir);

// tolower
string to_lower(const string &str);

// Try to open a verison.json file
optional<InstalledVersion> getInstalledVersion(fs::path dir);

// are we output to a terminal?
t_CKBOOL ck_isatty();

//-----------------------------------------------------------------------------
// name: class TC | 1.5.0.5 (ge) added
// desc: terminal code for enhanced output, like color text
//
// NOTE: by default TC's globalDisableOverride is ENGAGED, meaning for TC to
//       actually process terminal codes, it must be explicitly enabled;
//       not all output supports terminal escape sequences and some may even
//       print the escape sequence
//
//       when chuck is integrated, the recommended way to enable this:
//           `chuck->toggleGlobalColorTextoutput( true );`
//       where `chuck` is a ChucK instance; this will also set the
//       corresponding setting value inside the ChucK instance
//
//       calling TC::globalDisableOverride( false ) is not recommended!
//
// (still working on making TC more flexible!) more resources:
// https://stackoverflow.com/questions/4842424/list-of-ansi-color-escape-sequences
// https://stackoverflow.com/questions/2616906/how-do-i-output-coloured-text-to-a-linux-terminal
// https://cplusplus.com/forum/general/18200/
// FYI nifty tool: https://michurin.github.io/xterm256-color-picker/
//-----------------------------------------------------------------------------
class TC {
public:
  // reset all attributes
  static std::string reset() { return "\033[0m"; }
  // enable bold
  static std::string bold() { return std::string("\033[1m"); }
  // enable TC
  static void on();
  // disable TC (bypass)
  static void off();

public:
  // a more global switch, if engaged, will override on() for all TC:: methods
  // tranform text methods will bypass and return the input without
  // modification; TC::set*() will return empty strings; if this switch is NOT
  // engaged, will defer to on() and off() does not affect sequences constructed
  // manually outside of TC FYI this option is typically used on systems with no
  // color terminal capabilities also see command line flags --color --no-color
  static void globalDisableOverride(t_CKBOOL setTrueToEngage);

public:
  // transform text; make bold
  static std::string bold(const std::string &text);
  // transform text; make color, with optional bold attribute
  static std::string green(const std::string &text, t_CKBOOL bold = FALSE);
  static std::string orange(const std::string &text, t_CKBOOL bold = FALSE);
  static std::string blue(const std::string &text, t_CKBOOL bold = FALSE);
  static std::string red(const std::string &text, t_CKBOOL bold = FALSE);
  static std::string yellow(const std::string &text, t_CKBOOL bold = FALSE);
  static std::string magenta(const std::string &text, t_CKBOOL bold = FALSE);

public:
  // terminal codes; use with TC::set()
  enum TerminalCode {
    RESET_ALL = 0,
    BOLD = 1,
    DIM = 2,
    BLINK = 5,
    FG_BLACK = 30,
    FG_RED = 31,
    FG_GREEN = 32,
    FG_YELLOW = 33,
    FG_BLUE = 34,
    FG_MAGENTA = 35,
    FG_CYAN = 36,
    FG_WHITE = 37,
    FG_DEFAULT = 39,

    BG_RED = 41,
    BG_GREEN = 42,
    BG_YELLOW = 43,
    BG_BLUE = 44,
    BG_MAGENTA = 45,
    BG_CYAN = 46,
    BG_WHITE = 47,
    BG_DEFAULT = 49,

    FG_DARK_GRAY = 90,
    FG_LIGHT_RED = 91,
    FG_LIGHT_GREEN = 92,
    FG_LIGHT_YELLOW = 93,
    FG_LIGHT_BLUE = 94,
    FG_LIGHT_MAGENTA = 95,
    FG_LIGHT_CYAN = 96,
  };

  // set custom color
  static std::string color(TerminalCode code, const std::string &text,
                           t_CKBOOL bold = FALSE);
  // set a terminal code
  static std::string set(TerminalCode code);
  // set using an integer
  static std::string seti(t_CKUINT code);
  // set foreground default color
  static std::string set_fg_default();
  // set background default color
  static std::string set_bg_default();
  // set specific colors
  static std::string set_green(t_CKBOOL bold = FALSE);
  static std::string set_orange(t_CKBOOL bold = FALSE);
  static std::string set_blue(t_CKBOOL bold = FALSE);

protected:
  // whether TC is enabled
  static t_CKBOOL isEnabled;
  // global bypass
  static t_CKBOOL globalBypass;
};

#endif
