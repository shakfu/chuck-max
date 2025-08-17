// package.cpp

#include "package.h"
#include "util.h"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <sstream>
// #include <cstring>
namespace fs = std::filesystem;

// Equality operator overload
bool Package::operator==(const Package &other) const {
  return (name == other.name) && (authors == other.authors) &&
         (homepage == other.homepage) && (repository == other.repository) &&
         (license == other.license) && (description == other.description) &&
         (keywords == other.keywords) && (versions == other.versions);
}

// Less-than operator (for sorting)
bool Package::operator<(const Package &other) const {
  string name_lower = name;
  std::transform(name_lower.begin(), name_lower.end(), name_lower.begin(),
                 [](unsigned char c) { return std::tolower(c); });

  string other_lower = other.name;
  std::transform(other_lower.begin(), other_lower.end(), other_lower.begin(),
                 [](unsigned char c) { return std::tolower(c); });

  return name_lower < other_lower;
}

PackageVersion::PackageVersion() {}

PackageVersion::PackageVersion(string version) { setVersionString(version); }

PackageVersion::PackageVersion(int _major, int _minor, int _patch) {
  if (_major < 0 || _minor < 0 || _patch < 0) {
    throw std::invalid_argument(
        "Version numbers cannot be negative. (" + std::to_string(major) + "." +
        std::to_string(minor) + "." + std::to_string(patch) + ")");
  }

  major = _major;
  minor = _minor;
  patch = _patch;
}

PackageVersion::PackageVersion(string version, string _language_ver_min,
                               string _api_ver, string _os, Architecture _arch,
                               vector<File> _files) {
  setVersionString(version);

  language_version_min = _language_ver_min;
  api_version = _api_ver;
  files = _files;
  os = _os;
  arch = _arch;
}

PackageVersion::PackageVersion(string version, string _language_ver_min,
                               string _os, Architecture _arch,
                               vector<File> _files) {
  setVersionString(version);

  language_version_min = _language_ver_min;
  files = _files;
  os = _os;
  arch = _arch;
}

PackageVersion::PackageVersion(string version, string _language_ver_min,
                               string _language_ver_max, string _api_ver,
                               string _os, Architecture _arch,
                               vector<File> _files) {
  setVersionString(version);

  language_version_min = _language_ver_min;
  language_version_max = _language_ver_max;
  api_version = _api_ver;
  files = _files;
  os = _os;
  arch = _arch;
}

void PackageVersion::setVersionString(string versionStr) {
  vector<int> values;

  string token;
  string delimiter = ".";

  /*****/
  size_t last = 0;
  size_t next = 0;
  while ((next = versionStr.find(delimiter, last)) != string::npos) {
    token = versionStr.substr(last, next - last);

    try {
      int value = std::stoi(token);
      values.push_back(value);
    } catch (...) {
      throw std::invalid_argument("Version string must be in the format "
                                  "major.minor.patch (i.e. 1.2.3)");
    }

    last = next + 1;
  }

  token = versionStr.substr(last, versionStr.size() - last);
  int value = std::stoi(token);
  values.push_back(value);

  if (values.size() != 3) {
    throw std::invalid_argument("Invalid version string format (" + versionStr +
                                ")");
  }

  if (values[0] < 0 || values[1] < 0 || values[2] < 0) {
    throw std::invalid_argument("Version numbers cannot be negative. (" +
                                versionStr + ")");
  }

  major = values[0];
  minor = values[1];
  patch = values[2];
}

string PackageVersion::getVersionString() const {
  std::ostringstream stringStream;
  stringStream << major << "." << minor << "." << patch;

  return stringStream.str();
}

bool PackageVersion::operator==(const PackageVersion &other) const {
  return std::tie(major, minor, patch) ==
         std::tie(other.major, other.minor, other.patch);
  ;
}

bool PackageVersion::operator!=(const PackageVersion &other) const {
  return std::tie(major, minor, patch) !=
         std::tie(other.major, other.minor, other.patch);
  ;
}

bool PackageVersion::operator<(const PackageVersion &other) const {
  return std::tie(major, minor, patch) <
         std::tie(other.major, other.minor, other.patch);
  ;
}

bool PackageVersion::operator<=(const PackageVersion &other) const {
  return std::tie(major, minor, patch) <=
         std::tie(other.major, other.minor, other.patch);
  ;
}

bool PackageVersion::operator>(const PackageVersion &other) const {
  return std::tie(major, minor, patch) >
         std::tie(other.major, other.minor, other.patch);
  ;
}

bool PackageVersion::operator>=(const PackageVersion &other) const {
  return std::tie(major, minor, patch) >=
         std::tie(other.major, other.minor, other.patch);
  ;
}

// Output stream operator overload
std::ostream &operator<<(std::ostream &os, const Package &pkg) {
  os << "Name: " << pkg.name << "\n"
     << "Authors: ";

  if (pkg.authors.size() == 1) {
    os << pkg.authors[0] << "\n";
  } else if (pkg.authors.size() > 1) {
    os << "[" << pkg.authors[0];

    for (size_t i = 1; i < pkg.authors.size(); ++i) {
      os << ", " << pkg.authors[i];
    }
    os << "]\n";
  }

  os << "Homepage: " << pkg.homepage << "\n"
     << "Repository: " << pkg.repository << "\n"
     << "License: " << pkg.license << "\n"
     << "Description: " << pkg.description << "\n"
     << "Keywords: [";

  if (!pkg.keywords.empty()) {
    os << pkg.keywords[0];

    for (size_t i = 1; i < pkg.keywords.size(); ++i) {
      os << ", " << pkg.keywords[i];
    }
  }

  os << "]\nVersions:\n";
  if (!pkg.versions.empty()) {

    for (const auto &ver : pkg.versions) {
      os << "    "
         << "Version: " << ver.major << "." << ver.minor << "." << ver.patch
         << "\n";

      if (ver.api_version)
        os << "    "
           << "API Version: " << ver.api_version.value() << "\n";
      os << "    "
         << "Language Version (min): " << ver.language_version_min << "\n";

      if (ver.language_version_max) {
        os << "    "
           << "Language Version (max): " << ver.language_version_max.value()
           << "\n";
      }
      os << "    "
         << "Operating System: " << ver.os << "\n"
         << "    "
         << "Files: [";

      if (!ver.files.empty()) {
        os << ver.files[0].url;

        for (size_t i = 1; i < ver.files.size(); ++i) {
          os << "    "
             << ", " << ver.files[i].url;
        }
      }
      os << "]\n\n";
    }
  }
  return os;
}

std::ostream &operator<<(std::ostream &os, const PackageVersion &ver) {
  os << "Version: " << ver.major << "." << ver.minor << "." << ver.patch
     << "\n";

  if (ver.api_version)
    os << "API Version: " << ver.api_version.value() << "\n";
  os << "Language Version (min): " << ver.language_version_min << "\n";

  if (ver.language_version_max) {
    os << "Language Version (max): " << ver.language_version_max.value()
       << "\n";
  }
  os << "Operating System: " << ver.os << "\n"
     << "Files: [";

  if (!ver.files.empty()) {
    os << ver.files[0].url;

    for (size_t i = 1; i < ver.files.size(); ++i) {
      os << ", " << ver.files[i].url;
    }
  }
  os << "]\n";

  return os;
}

void to_json(json &j, const PackageVersion &p) {
  string version = p.getVersionString();
  j = json{
      {"version", version}, {"language_version_min", p.language_version_min},
      {"os", p.os},         {"arch", architectureToString.at(p.arch)},
      {"files", p.files},
  };

  if (p.language_version_max) {
    j["language_version_max"] = p.language_version_max.value();
  }
  if (p.api_version) {
    j["api_version"] = p.api_version.value();
  }
}

void from_json(const json &j, PackageVersion &p) {
  p.setVersionString(j.at("version"));
  j.at("language_version_min").get_to(p.language_version_min);

  if (j.contains("api_version") && !j["api_version"].is_null()) {
    ApiVersion api_ver = j.at("api_version");
    p.api_version = api_ver;
  }

  if (j.contains("language_version_max") &&
      !j["language_version_max"].is_null()) {
    ChuckVersion language_version_max = j.at("language_version_max");
    p.language_version_max = language_version_max;
  }

  j.at("os").get_to(p.os);

  if (j.contains("arch") && !j["arch"].is_null()) {
    p.arch = stringToArchitecture.at(j["arch"]);
  } else {
    p.arch = ARCH_ALL;
  }

  j.at("files").get_to(p.files);
}

void to_json(json &j, const Package &p) {
  j = json{{"name", p.name},
           {"homepage", p.homepage},
           {"repository", p.repository},
           {"authors", p.authors},
           {"license", p.license},
           {"description", p.description},
           {"keywords", p.keywords},
           {"versions", p.versions}};

  // optional fields
  if (p.long_description) {
    j["long_description"] = p.long_description.value();
  }
  if (p.doc_url) {
    j["doc_url"] = p.doc_url.value();
  }
}

void from_json(const json &j, Package &p) {
  j.at("name").get_to(p.name);
  j.at("homepage").get_to(p.homepage);
  j.at("repository").get_to(p.repository);
  j.at("authors").get_to(p.authors);
  j.at("license").get_to(p.license);
  j.at("description").get_to(p.description);
  j.at("keywords").get_to(p.keywords);

  // optional and array fields
  if (j.contains("versions")) {
    j.at("versions").get_to(p.versions);
  }

  if (j.contains("long_description") && !j["long_description"].is_null()) {
    p.long_description = j["long_description"];
  }

  if (j.contains("doc_url") && !j["doc_url"].is_null()) {
    p.doc_url = j["doc_url"];
  }
}

// find the latest compatible version
optional<PackageVersion> Package::latest_version(string os, Architecture arch,
                                                 ChuckVersion language_version,
                                                 ApiVersion api_version) {
  optional<PackageVersion> latest_version;

  for (PackageVersion version : versions) {
    ChuckVersion ck_min_ver(version.language_version_min);

    optional<ApiVersion> api;

    if (version.api_version)
      api = ApiVersion(version.api_version.value());

    // filter out bad candidates
    if (version.os != "any" && version.os != os)
      continue;
    if (language_version < ck_min_ver)
      continue;

    // special cases for macos universal binaries and ARCH_ANY
    if (version.arch != ARCH_ALL && version.arch != arch) {
      // The version is a different arch and it's not a mac universal binary
      if (version.arch != MAC_UNIVERSAL)
        continue;

      // The version is a mac universal binary and our arch is not a mac arch
      if (version.arch == MAC_UNIVERSAL && (arch != X86_64 && arch != ARM64))
        continue;
    }

    if (version.language_version_max) {
      ChuckVersion ck_max_ver(version.language_version_max.value());
      if (language_version > ck_max_ver)
        continue;
    }

    if (api && api != api_version)
      continue;

    if (version > latest_version) {
      latest_version = version;
    }
  }
  return latest_version;
}

// find latest version available in OS
optional<PackageVersion> Package::latest_version(string os) {
  optional<PackageVersion> latest_version;

  for (PackageVersion version : versions) {
    if (version.os != "any" && version.os != os)
      continue;
    if (version > latest_version) {
      latest_version = version;
    }
  }
  return latest_version;
}

optional<PackageVersion> Package::version(PackageVersion ver, string os,
                                          ChuckVersion language_ver,
                                          ApiVersion api_ver) {
  optional<PackageVersion> returned_version;

  for (PackageVersion version : versions) {
    ChuckVersion ck_min_ver(version.language_version_min);

    optional<ApiVersion> api;

    if (version.api_version)
      api = ApiVersion(version.api_version.value());

    // filter out bad candidates
    if (version.os != "any" && version.os != os)
      continue;
    if (language_ver < ck_min_ver)
      continue;

    if (version.language_version_max) {
      ChuckVersion ck_max_ver(version.language_version_max.value());
      if (language_ver > ck_max_ver)
        continue;
    }

    if (api && api != api_ver)
      continue;

    if (version == ver) {
      returned_version = version;
      break;
    }
  }

  return returned_version;
}

void to_json(json &j, const File &f) {
  j = json{{"url", f.url},
           {"file_type", filetypeToString.at(f.file_type)},
           {"local_dir", f.local_dir},
           {"checksum", f.checksum}};
}

void from_json(const json &j, File &f) {
  j.at("url").get_to(f.url);
  j.at("checksum").get_to(f.checksum);

  if (f.checksum == "") {
    throw std::invalid_argument("checksum filed for " + f.url +
                                " should not be be empty");
  }

  f.file_type = stringToFiletype.at(j["file_type"]);

  string local_dir = j["local_dir"];

  if (!is_subpath(local_dir, "./")) {
    throw std::invalid_argument(
        "file location (" + local_dir +
        ") can't be stored outside of package directory");
  }
  j["local_dir"].get_to(f.local_dir);
}

InstalledVersion::InstalledVersion(Package pkg, PackageVersion v) {
  name = pkg.name;
  authors = pkg.authors;
  homepage = pkg.homepage;
  repository = pkg.repository;
  license = pkg.license;
  description = pkg.description;
  keywords = pkg.keywords;

  major = v.major;
  minor = v.minor;
  patch = v.patch;

  api_version = v.api_version;
  language_version_min = v.language_version_min;
  language_version_max = v.language_version_max;
  os = v.os;
  arch = v.arch;
}

InstalledVersion::InstalledVersion() {}

// Export a PackageVersion. This is used for metadata and comparisons
// so we won't include files.
PackageVersion InstalledVersion::version() {
  PackageVersion v(major, minor, patch);
  v.api_version = api_version;
  v.language_version_min = language_version_min;
  v.language_version_max = language_version_max;
  v.os = os;

  return v;
}

// Export a Package.
Package InstalledVersion::package() {
  Package pkg;

  pkg.name = name;
  pkg.authors = authors;
  pkg.homepage = homepage;
  pkg.repository = repository;
  pkg.license = license;
  pkg.description = description;
  pkg.keywords = keywords;

  return pkg;
}

string InstalledVersion::getVersionString() const {
  std::ostringstream stringStream;
  stringStream << major << "." << minor << "." << patch;

  return stringStream.str();
}

void to_json(json &j, const InstalledVersion &p) {
  j = json{
      {"name", p.name},
      {"homepage", p.homepage},
      {"repository", p.repository},
      {"authors", p.authors},
      {"license", p.license},
      {"description", p.description},
      {"keywords", p.keywords},
      {"version", {{"major", p.major}, {"minor", p.minor}, {"patch", p.patch}}},
      {"language_version_min", p.language_version_min},
      {"os", p.os},
      {"arch", architectureToString.at(p.arch)},
      {"files", p.files}};

  // optional fields
  if (p.language_version_max) {
    j["language_version_max"] = p.language_version_max.value();
  }
  if (p.api_version) {
    j["api_version"] = p.api_version.value();
  }
  if (p.long_description) {
    j["long_description"] = p.long_description.value();
  }
  if (p.doc_url) {
    j["doc_url"] = p.doc_url.value();
  }
}

void from_json(const json &j, InstalledVersion &p) {
  j.at("name").get_to(p.name);
  j.at("homepage").get_to(p.homepage);
  j.at("repository").get_to(p.repository);
  j.at("authors").get_to(p.authors);
  j.at("license").get_to(p.license);
  j.at("description").get_to(p.description);
  j.at("keywords").get_to(p.keywords);

  j.at("version").at("major").get_to(p.major);
  j.at("version").at("minor").get_to(p.minor);
  j.at("version").at("patch").get_to(p.patch);

  j.at("language_version_min").get_to(p.language_version_min);

  if (j.contains("api_version") && !j["api_version"].is_null()) {
    ApiVersion api_ver = j.at("api_version");
    p.api_version = api_ver;
  }

  if (j.contains("language_version_max") &&
      !j["language_version_max"].is_null()) {
    ChuckVersion language_version_max = j.at("language_version_max");
    p.language_version_max = language_version_max;
  }

  j.at("os").get_to(p.os);

  if (j.contains("arch") && !j["arch"].is_null()) {
    p.arch = stringToArchitecture.at(j["arch"]);
  } else {
    p.arch = ARCH_ALL;
  }

  j.at("files").get_to(p.files);

  // optional site-specific fields
  if (j.contains("long_description") && !j["long_description"].is_null()) {
    p.long_description = j["long_description"];
  }

  if (j.contains("doc_url") && !j["doc_url"].is_null()) {
    p.doc_url = j["doc_url"];
  }
}
