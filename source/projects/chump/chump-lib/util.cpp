#include "util.h"

#include "package_list.h"

#include <regex>

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <sstream>
#include <system_error>

#include <mz.h>
#include <unzip.h>

#ifdef _WIN32
#include <Shlobj.h>
#include <io.h> // for _isatty()
#include <windows.h>
#else               // not windows
#include <unistd.h> // for isatty()
#endif              // #ifdef _WIN32

using std::map;

// Returns the path to the directory where a package will be installed
// TODO expand to other OS
fs::path packagePath(Package p, fs::path install_dir) {
  fs::path package_dir = install_dir / p.name;
  return package_dir;
}

fs::path chumpDir() {
  fs::path home = getHomeDirectory();
#ifdef _WIN32
  fs::path chump_dir = home / "Documents" / "ChucK" / "packages";
#else
  fs::path chump_dir = home / ".chuck" / "packages";
#endif

  return chump_dir;
}

std::string manifestURL(std::string base_url) {
  return base_url + "v" + std::to_string(MANIFEST_VERSION_NO) +
         "/manifest.json";
}

std::string whichOS() {
#if defined(_WIN32) || defined(_WIN64) || defined(__CYGWIN__)
  return "windows";
#elif defined(__APPLE__)
  return "mac";
#elif defined(__linux__)
  return "linux";
#else
  throw std::system_error(EINVAL, std::generic_category(),
                          "[chump]: unknown operating system");
#endif
}

Architecture whichArch() {
#if defined(__x86_64__) || defined(_M_X64)
  return X86_64;
#elif defined(i386) || defined(__i386__) || defined(__i386) || defined(_M_IX86)
  return X86;
#elif defined(__aarch64__) || defined(_M_ARM64)
  return ARM64;
#else // this shouldn't happen but whoops if it does
  throw std::system_error(
      EINVAL, std::generic_category(),
      "[chump]: chump does not support this system architecture");
#endif
}

// Get user's home directory.
std::filesystem::path getHomeDirectory() {
  std::filesystem::path home_dir;

#ifdef _WIN32
  // Windows-specific code to get the user's home directory
  TCHAR szPath[MAX_PATH];
  if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_PROFILE, NULL, 0, szPath))) {
    home_dir = szPath;
  } else {
    // If SHGetFolderPath fails, fallback to using environment variables
    const char *home_env = std::getenv("USERPROFILE");
    if (home_env != nullptr)
      home_dir = home_env;
    else
      throw std::runtime_error("Failed to get user's home directory");
  }
#else
  // Unix-like systems
  const char *home_env = std::getenv("HOME");
  if (home_env != nullptr)
    home_dir = home_env;
  else
    throw std::runtime_error("Failed to get user's home directory");
#endif

  return home_dir;
}

tuple<string, optional<string>> parsePackageName(string packageName) {
  std::regex pattern(
      "(.*)=(.*)"); // Regular expression to match "name=version" or just "name"
  std::smatch matches;
  string name;
  optional<string> version;

  if (std::regex_match(packageName, matches, pattern)) {
    name = matches[1];    // The part before '='
    version = matches[2]; // The part after '='
  } else {
    name = packageName;
  }

  return {name, version};
}

// https://stackoverflow.com/questions/62503197/check-if-path-contains-another-in-c
bool is_subpath(const fs::path &path, const fs::path &base) {
  if (path == base)
    return true; // edge case

  // Need to convert these paths to absolute or otherwise
  // there's inconsistencies between windows and unix.
  auto rel = fs::absolute(path).lexically_relative(fs::absolute(base));
  return !rel.empty() && rel.native()[0] != '.';
}

// Convert a file to a sha 256 hash.
// https://docs.openssl.org/3.0/man3/EVP_DigestInit/#examples
// Do I know what's really going on here? Sort of...
std::string hash_file(fs::path filename) {
  std::ifstream file(filename, std::ios::binary);
  if (!file) {
    throw std::runtime_error("Failed to open file: " + filename.string());
  }

  EVP_MD_CTX *md5Context = EVP_MD_CTX_new();
  EVP_MD_CTX_init(md5Context);
  EVP_DigestInit_ex(md5Context, EVP_sha256(), nullptr);

  const size_t bufferSize = 4096;
  char buffer[bufferSize];
  while (!file.eof()) {
    file.read(buffer, bufferSize);
    EVP_DigestUpdate(md5Context, buffer, file.gcount());
  }

  unsigned char result[EVP_MAX_MD_SIZE];
  unsigned int hash_len;

  EVP_DigestFinal_ex(md5Context, result, &hash_len);
  file.close();

  EVP_MD_CTX_free(md5Context);

  std::stringstream ss;
  for (unsigned int i = 0; i < hash_len; ++i) {
    ss << std::hex << std::setw(2) << std::setfill('0') << (int)result[i];
  }

  string hashed = ss.str();
  return hashed;
}

bool validate_manifest(fs::path manifest_path) {
  if (!fs::exists(manifest_path)) {
    std::cerr
        << "[chump]: unable to find package list (manifest.json), fetching..."
        << std::endl;
    return false;
  }

  try {
    PackageList p(manifest_path);
  } catch (const std::exception &e) {
    std::cerr << "[chump]: failed to validate manifest.json with the error: "
              << e.what() << std::endl;
    std::cerr << "[chump]: hint: try updating the chump package list (`chump "
                 "list -u`)"
              << std::endl;
    return false;
  }

  return true;
}

// Convert a FileType enum to the appropriate dir to store it in inside a
// package dir
fs::path fileTypeToDir(FileType f) {
  fs::path empty;

  switch (f) {
  case PACKAGE_FILE:
    return empty;
  case DATA_FILE:
    return fs::path("_data");
  case EXAMPLE_FILE:
    return fs::path("_examples");
  case DOCS_FILE:
    return fs::path("_docs");
  case DEPS_FILE:
    return fs::path("_deps");
  case ZIP_FILE:
    return empty;
  default:
    return empty;
  }
}

bool unzipFile(const std::string &zipPath, const std::string &outputDir) {
  unzFile zipFile = unzOpen64(zipPath.c_str());
  if (zipFile == nullptr) {
    std::cerr << "[chump]: error opening ZIP file: " << zipPath << std::endl;
    return false;
  }

  // Iterate through each file in the ZIP archive
  int result = unzGoToFirstFile(zipFile);
  while (result == UNZ_OK) {
    // Get file info
    unz_file_info64 fileInfo;
    char filename[256];
    unzGetCurrentFileInfo64(zipFile, &fileInfo, filename, sizeof(filename),
                            nullptr, 0, nullptr, 0);

    std::string fullPath = outputDir + "/" + filename;

    // Check if it's a directory and create it if necessary
    if (filename[strlen(filename) - 1] == '/') {
      std::filesystem::create_directories(fullPath);
    } else {
      // Extract file
      unzOpenCurrentFile(zipFile);

      fs::path dirs = fs::path(fullPath).parent_path();
      fs::create_directories(dirs);

      FILE *outFile = fopen(fullPath.c_str(), "wb");
      if (outFile == nullptr) {
        std::cerr << "[chump]: error opening output file: " << fullPath
                  << std::endl;
        unzCloseCurrentFile(zipFile);
        unzClose(zipFile);
        return false;
      }

      char buffer[8192];
      int bytesRead;
      while ((bytesRead = unzReadCurrentFile(zipFile, buffer, sizeof(buffer))) >
             0) {
        fwrite(buffer, 1, bytesRead, outFile);
      }

      fclose(outFile);
      unzCloseCurrentFile(zipFile);

      if (bytesRead < 0) {
        std::cerr << "[chump]: error reading ZIP file: " << filename
                  << std::endl;
        unzClose(zipFile);
        return false;
      }
    }

    // Move to the next file in the ZIP
    result = unzGoToNextFile(zipFile);
  }

  // Close the ZIP file
  unzClose(zipFile);

  return true;
}

// tolower
string to_lower(const string &str) {
  string s = str;
  std::transform(s.begin(), s.end(), s.begin(),
                 [](unsigned char c) { return std::tolower(c); });
  return s;
}

// Try to open a verison.json file
optional<InstalledVersion> getInstalledVersion(fs::path dir) {
  if (!fs::is_directory(dir)) {
    std::cerr << "[chump]: path " << dir << " is not a directory" << std::endl;
    return {};
  }

  fs::path json_path = dir / "version.json";

  if (!fs::exists(json_path)) {
    std::cerr << "[chump]: file " << json_path << " not found" << std::endl;
    return {};
  }

  std::ifstream f(json_path);

  if (!f.good()) {
    std::cerr << "[chump]: unable to open " << json_path << std::endl;
    f.close();
    return {};
  }

  InstalledVersion installed_version;
  try {
    json pkg_ver = json::parse(f);
    f.close();
    installed_version = pkg_ver.template get<InstalledVersion>();
  } catch (const std::exception &e) {
    f.close();
    std::cerr << "[chump]: exception throw when trying to parse " << json_path
              << ": " << e.what() << std::endl;
    return {};
  }

  return installed_version;
}

//-----------------------------------------------------------------------------
// name: ck_isatty() | 1.5.0.5 (ge) added
// desc: are we output to a TTY (teletype, character by character)
//       (vs say a file stream; helpful for determining if we should
//       suppress printing ANSI escapes codes, e.g., color codes
//       to the output stream
//-----------------------------------------------------------------------------
t_CKBOOL ck_isatty(int fd) {
#if defined(__PLATFORM_WINDOWS__)
  return _isatty(fd) != 0;
#else
  return isatty(fd) != 0;
#endif
}
//-----------------------------------------------------------------------------
// get a general sense if currently outputting to TTY
//-----------------------------------------------------------------------------
t_CKBOOL ck_isatty() {
  // let's test stderr, since much of chuck operates over it
#if defined(__PLATFORM_WINDOWS__)
  return ck_isatty(_fileno(stderr));
#elif defined(__PLATFORM_EMSCRIPTEN__)
  // emscripten always seems to return TRUE for isatty()
  // but then ioctl() does not seem to be working / implemented
  // so returning FALSE for now | use chuck param TTY_WIDTH_HINT
  return FALSE;
#else
  return ck_isatty(fileno(stderr));
#endif
}

//-----------------------------------------------------------------------------
// static instantiation
//-----------------------------------------------------------------------------
t_CKBOOL TC::isEnabled = TRUE;
t_CKBOOL TC::globalBypass = TRUE;

//-----------------------------------------------------------------------------
// on/off switches
//-----------------------------------------------------------------------------
// void TC::on() { isEnabled = TRUE; }
// void TC::off() { isEnabled = FALSE; }
//-----------------------------------------------------------------------------
// a more global disable, overriding on() for the text transformation and
// TC::set*() the former will bypass and return the input without modification;
// the latter will return empty strings.
// does not affect sequences constructed manually outside of TC
// FYI this option is typically used on systems with no color terminal
// capabilities also see command line flags --color --no-color
//-----------------------------------------------------------------------------
void TC::globalDisableOverride(t_CKBOOL setTrueToEngage) {
  // set flag, as long as this is true, TC will always bypass,
  // regardless of on() and off() and isEnabled state
  globalBypass = setTrueToEngage;
}

//-----------------------------------------------------------------------------
// get bold escape sequence
//-----------------------------------------------------------------------------
std::string TC::bold(const std::string &text) {
  if (globalBypass || !isEnabled)
    return text;
  return TC::bold() + text + TC::reset();
}

//-----------------------------------------------------------------------------
// get color escape sequences
//-----------------------------------------------------------------------------
std::string TC::green(const std::string &text, t_CKBOOL bold) {
  if (globalBypass || !isEnabled)
    return text;
  return TC::set(FG_GREEN) + (bold ? TC::bold() : "") + text + TC::reset();
}

std::string TC::orange(const std::string &text, t_CKBOOL bold) {
  if (globalBypass || !isEnabled)
    return text;
  return std::string("\033[38;5;208m") + (bold ? TC::bold() : "") + text +
         TC::reset();
}

std::string TC::blue(const std::string &text, t_CKBOOL bold) {
  if (globalBypass || !isEnabled)
    return text;
  return std::string("\033[38;5;39m") + (bold ? TC::bold() : "") + text +
         TC::reset();
}

std::string TC::red(const std::string &text, t_CKBOOL bold) {
  if (globalBypass || !isEnabled)
    return text;
  return TC::set(FG_RED) + (bold ? TC::bold() : "") + text + TC::reset();
}

std::string TC::yellow(const std::string &text, t_CKBOOL bold) {
  if (globalBypass || !isEnabled)
    return text;
  return TC::set(FG_YELLOW) + (bold ? TC::bold() : "") + text + TC::reset();
}

std::string TC::magenta(const std::string &text, t_CKBOOL bold) {
  if (globalBypass || !isEnabled)
    return text;
  return std::string("\033[38;5;170m") + (bold ? TC::bold() : "") + text +
         TC::reset();
}

// set custom color
std::string TC::color(TC::TerminalCode code, const std::string &text,
                      t_CKBOOL bold) {
  if (globalBypass || !isEnabled)
    return text;
  return TC::set(code) + (bold ? TC::bold() : "") + text + TC::reset();
}

std::string TC::set_green(t_CKBOOL bold) {
  if (globalBypass || !isEnabled)
    return "";
  return TC::set(FG_GREEN) + (bold ? TC::bold() : "");
}

std::string TC::set_orange(t_CKBOOL bold) {
  if (globalBypass || !isEnabled)
    return "";
  return std::string("\033[38;5;208m") + (bold ? TC::bold() : "");
}

std::string TC::set_blue(t_CKBOOL bold) {
  if (globalBypass || !isEnabled)
    return "";
  return std::string("\033[38;5;39m") + (bold ? TC::bold() : "");
}

//-----------------------------------------------------------------------------
// set*() methods -- returns escape sequences o insert into output
//-----------------------------------------------------------------------------
// set a terminal code
std::string TC::set(TerminalCode code) {
  if (globalBypass || !isEnabled)
    return "";
  return std::string("\033[") + std::to_string(code) + "m";
}

// set using an integer
std::string TC::seti(t_CKUINT code) {
  if (globalBypass || !isEnabled)
    return "";
  return std::string("\033[") + std::to_string(code) + "m";
}

// set foreground default color
std::string TC::set_fg_default() {
  if (globalBypass || !isEnabled)
    return "";
  return TC::set(FG_DEFAULT);
}

// set background default color
std::string TC::set_bg_default() {
  if (globalBypass || !isEnabled)
    return "";
  return TC::set(BG_DEFAULT);
}
