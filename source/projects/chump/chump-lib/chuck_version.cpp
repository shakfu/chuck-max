
#include "chuck_version.h"

#include <cstdio>
#include <iostream>
#include <regex>
#include <sstream>
#include <stdexcept>

ChuckVersion::ChuckVersion() {}

ChuckVersion::ChuckVersion(string version_string) {
  set_version(version_string);
}

ChuckVersion::ChuckVersion(int _mega, int _major, int _minor, int _patch) {
  mega = _mega;
  major = _major;
  minor = _minor;
  patch = _patch;
}

ChuckVersion ChuckVersion::makeSystemVersion() {
#ifdef _WIN32
  std::string cmd = "where chuck >nul 2>&1";
#else
  // Use 'which' command on Unix-like systems
  std::string cmd = "which chuck > /dev/null 2>&1";
#endif

  if (std::system(cmd.c_str()) != EXIT_SUCCESS) {
    throw std::runtime_error(
        "[chump]: chuck is not installed! please install chuck before using "
        "chump. installation instructions can be found at "
        "https://chuck.stanford.edu/");
  }

  // Query chuck to get verison string
#ifdef _WIN32
  FILE *pipe = _popen("chuck --query:version 2>&1", "r");
#else
  FILE *pipe = popen("chuck --query:version 2>&1", "r");
#endif
  if (!pipe) {
    throw std::runtime_error("[chump]: popen() failed!");
  }

  std::string version;

  constexpr int buffer_size = 128;
  char buffer[buffer_size];
  while (fgets(buffer, buffer_size, pipe) != nullptr) {
    version += buffer;
  }

#ifdef _WIN32
  auto status = _pclose(pipe);
#else
  auto status = pclose(pipe);
#endif
  if (status == -1) {
    throw std::runtime_error("pclose() failed!");
  }

  return ChuckVersion(version);
}

ChuckVersion ChuckVersion::makeVersion(string version_string) {
  return ChuckVersion(version_string);
}

void ChuckVersion::set_version(string version_string) {
  std::regex version_regex("(\\d+)\\.(\\d+)\\.(\\d+)\\.(\\d+)");
  std::smatch match;

  if (std::regex_search(version_string, match, version_regex)) {
    mega = std::stoi(match[1]);
    major = std::stoi(match[2]);
    minor = std::stoi(match[3]);
    patch = std::stoi(match[4]);
  } else {
    throw std::runtime_error("unable to construct version string from " +
                             version_string);
  }
}

string ChuckVersion::getVersionString() {
  std::ostringstream stringStream;
  stringStream << mega << "." << major << "." << minor << "." << patch;

  return stringStream.str();
}

void to_json(json &j, const ChuckVersion &v) {
  j = json{
      {"mega", v.mega},
      {"major", v.major},
      {"minor", v.minor},
      {"patch", v.patch},
  };
}

void from_json(const json &j, ChuckVersion &v) {
  j.at("mega").get_to(v.mega);
  j.at("major").get_to(v.major);
  j.at("minor").get_to(v.minor);
  j.at("patch").get_to(v.patch);
}

bool operator==(const ChuckVersion &lhs, const ChuckVersion &rhs) {
  return std::tie(lhs.mega, lhs.major, lhs.minor, lhs.patch) ==
         std::tie(rhs.mega, rhs.major, rhs.minor, rhs.patch);
}

bool operator!=(const ChuckVersion &lhs, const ChuckVersion &rhs) {
  return std::tie(lhs.mega, lhs.major, lhs.minor, lhs.patch) !=
         std::tie(rhs.mega, rhs.major, rhs.minor, rhs.patch);
}

bool operator<(const ChuckVersion &lhs, const ChuckVersion &rhs) {
  return std::tie(lhs.mega, lhs.major, lhs.minor, lhs.patch) <
         std::tie(rhs.mega, rhs.major, rhs.minor, rhs.patch);
}

bool operator<=(const ChuckVersion &lhs, const ChuckVersion &rhs) {
  return std::tie(lhs.mega, lhs.major, lhs.minor, lhs.patch) <=
         std::tie(rhs.mega, rhs.major, rhs.minor, rhs.patch);
}

bool operator>(const ChuckVersion &lhs, const ChuckVersion &rhs) {
  return std::tie(lhs.mega, lhs.major, lhs.minor, lhs.patch) >
         std::tie(rhs.mega, rhs.major, rhs.minor, rhs.patch);
}

bool operator>=(const ChuckVersion &lhs, const ChuckVersion &rhs) {
  return std::tie(lhs.mega, lhs.major, lhs.minor, lhs.patch) >=
         std::tie(rhs.mega, rhs.major, rhs.minor, rhs.patch);
}

// Output stream operator overload
std::ostream &operator<<(std::ostream &os, const ChuckVersion &ver) {
  os << ver.mega << "." << ver.major << "." << ver.minor << "." << ver.patch;

  return os;
}

ApiVersion::ApiVersion() {}

ApiVersion::ApiVersion(string version_string) { set_version(version_string); }

ApiVersion::ApiVersion(int _major, int _minor) {
  major = _major;
  minor = _minor;
}

ApiVersion ApiVersion::makeSystemVersion() {
#ifdef _WIN32
  std::string cmd = "where chuck >nul 2>&1";
#else
  // Use 'which' command on Unix-like systems
  std::string cmd = "which chuck > /dev/null 2>&1";
#endif

  if (std::system(cmd.c_str()) != EXIT_SUCCESS) {
    throw std::runtime_error(
        "[chump]: chuck is not installed! please install chuck before using "
        "chump. installation instructions can be found at "
        "https://chuck.stanford.edu/");
  }

  // Query chuck to get api version string
#ifdef _WIN32
  FILE *pipe = _popen("chuck --query:version_chugin_api 2>&1", "r");
#else
  FILE *pipe = popen("chuck --query:version_chugin_api 2>&1", "r");
#endif
  if (!pipe) {
    throw std::runtime_error("popen() failed!");
  }

  std::string version;

  constexpr int buffer_size = 128;
  char buffer[buffer_size];
  while (fgets(buffer, buffer_size, pipe) != nullptr) {
    version += buffer;
  }

#ifdef _WIN32
  auto status = _pclose(pipe);
#else
  auto status = pclose(pipe);
#endif
  if (status == -1) {
    throw std::runtime_error("pclose() failed!");
  }

  return ApiVersion(version);
}

ApiVersion ApiVersion::makeVersion(string version_string) {
  return ApiVersion(version_string);
}

void ApiVersion::set_version(string version_string) {
  std::regex version_regex("(\\d+)\\.(\\d+)\\s*$");
  std::smatch match;

  if (std::regex_search(version_string, match, version_regex)) {
    major = std::stoi(match[1]);
    minor = std::stoi(match[2]);
  } else {
    throw std::runtime_error("unable to construct version string from " +
                             version_string);
  }
}

string ApiVersion::getVersionString() {
  std::ostringstream stringStream;
  stringStream << major << "." << minor;

  return stringStream.str();
}

void to_json(json &j, const ApiVersion &v) {
  j = json{
      {"major", v.major},
      {"minor", v.minor},
  };
}

void from_json(const json &j, ApiVersion &v) {
  j.at("major").get_to(v.major);
  j.at("minor").get_to(v.minor);
}

bool operator==(const ApiVersion &lhs, const ApiVersion &rhs) {
  return std::tie(lhs.major, lhs.minor) == std::tie(rhs.major, rhs.minor);
}

bool operator!=(const ApiVersion &lhs, const ApiVersion &rhs) {
  return std::tie(lhs.major, lhs.minor) != std::tie(rhs.major, rhs.minor);
}

bool operator<(const ApiVersion &lhs, const ApiVersion &rhs) {
  return std::tie(lhs.major, lhs.minor) < std::tie(rhs.major, rhs.minor);
}

bool operator<=(const ApiVersion &lhs, const ApiVersion &rhs) {
  return std::tie(lhs.major, lhs.minor) <= std::tie(rhs.major, rhs.minor);
}

bool operator>(const ApiVersion &lhs, const ApiVersion &rhs) {
  return std::tie(lhs.major, lhs.minor) > std::tie(rhs.major, rhs.minor);
}

bool operator>=(const ApiVersion &lhs, const ApiVersion &rhs) {
  return std::tie(lhs.major, lhs.minor) >= std::tie(rhs.major, rhs.minor);
}

// Output stream operator overload
std::ostream &operator<<(std::ostream &os, const ApiVersion &ver) {
  os << ver.major << "." << ver.minor;

  return os;
}
