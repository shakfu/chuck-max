#ifndef __CHUCKVERSION_H__
#define __CHUCKVERSION_H__

#include <nlohmann/json.hpp>
#include <string>

using std::string;
using json = nlohmann::json;

struct ChuckVersion {
  ChuckVersion();
  ChuckVersion(string version_string);
  ChuckVersion(int mega, int major, int minor, int patch);

  static ChuckVersion makeSystemVersion();
  static ChuckVersion makeVersion(string version_string);

  // Format is mega.major.minor.patch (i.e. 1.5.2.4)
  int mega;
  int major;
  int minor;
  int patch;

  friend bool operator==(const ChuckVersion &lhs, const ChuckVersion &rhs);
  friend bool operator!=(const ChuckVersion &lhs, const ChuckVersion &rhs);
  friend bool operator<(const ChuckVersion &lhs, const ChuckVersion &rhs);
  friend bool operator<=(const ChuckVersion &lhs, const ChuckVersion &rhs);
  friend bool operator>(const ChuckVersion &lhs, const ChuckVersion &rhs);
  friend bool operator>=(const ChuckVersion &lhs, const ChuckVersion &rhs);

  void set_version(string version_string);
  string getVersionString();

  friend std::ostream &operator<<(std::ostream &os, const ChuckVersion &pkg);
};

void to_json(json &j, const ChuckVersion &v);
void from_json(const json &j, ChuckVersion &v);

struct ApiVersion {
  ApiVersion();
  ApiVersion(string version_string);
  ApiVersion(int major, int minor);

  static ApiVersion makeSystemVersion();
  static ApiVersion makeVersion(string version_string);

  // Format is major.minor (i.e. 10.1)
  int major;
  int minor;

  friend bool operator==(const ApiVersion &lhs, const ApiVersion &rhs);
  friend bool operator!=(const ApiVersion &lhs, const ApiVersion &rhs);
  friend bool operator<(const ApiVersion &lhs, const ApiVersion &rhs);
  friend bool operator<=(const ApiVersion &lhs, const ApiVersion &rhs);
  friend bool operator>(const ApiVersion &lhs, const ApiVersion &rhs);
  friend bool operator>=(const ApiVersion &lhs, const ApiVersion &rhs);

  void set_version(string version_string);
  string getVersionString();

  friend std::ostream &operator<<(std::ostream &os, const ApiVersion &pkg);
};

void to_json(json &j, const ApiVersion &v);
void from_json(const json &j, ApiVersion &v);

#endif
