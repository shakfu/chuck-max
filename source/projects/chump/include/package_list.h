#ifndef __PACKAGELIST_H__
#define __PACKAGELIST_H__

#include <fstream>
#include <nlohmann/json.hpp>
#include <optional>

#include "package.h"
#include "util.h"
// #include "chuck_version.h"

using json = nlohmann::json;
using std::optional;
using std::string;

namespace fs = std::filesystem;

// Our super special manifest version
#define MANIFEST_VERSION_NO 1

// A PackageList is a database of available packages, including names,
// download urls, description, and other metadata.
class PackageList {
public:
  PackageList();
  PackageList(fs::path filepath);
  // For scripts - construct a packagelist given a vector of packages;
  PackageList(std::vector<Package> _packages);

public:
  optional<Package> find_package(string name);
  // return latest (maybe latest && most compatible?) version of package
  optional<PackageVersion> find_latest_package_version(string name,
                                                       string operating_system,
                                                       Architecture arch,
                                                       ChuckVersion ck_ver,
                                                       ApiVersion api_ver);

  // return specific version of package
  optional<PackageVersion> find_package_version(string name, string version);
  optional<PackageVersion> find_package_version(string name,
                                                PackageVersion version);
  std::vector<Package> get_packages();

  /* static void from_json(const json& j, PackageList& pkg_list); */

private:
  // TODO could be a more efficient lookup implementation
  std::vector<Package> packages;

  int manifest_api_version;
};

#endif
