// The manager interface

#ifndef __MANAGER_H__
#define __MANAGER_H__

#include <string>
#include <vector>

#include "chuck_version.h"
#include "fetch.h"
#include "package.h"
#include "package_list.h"

using std::optional;
using std::string;
using std::tuple;
namespace fs = std::filesystem;

// only support the local path for now (to a spec json file)

class Manager {
public:
  // constructor
  Manager(std::string package_list_path, fs::path package_install_dir,
          ChuckVersion ck_ver, ApiVersion api_ver, std::string system_os,
          Architecture arch, std::string manifest_url, bool render_tui);

public:
  // return a list of all packages (installed and available)
  std::vector<Package> listPackages();

  optional<Package> getPackage(string name);
  optional<PackageVersion> latestPackageVersion(string name);

  bool install(string packageName);
  bool install_local(fs::path pkgDefn, fs::path pkgVer, fs::path pkgZip);
  bool uninstall(string packageName, bool force);
  bool update(string packageName);
  bool update_manifest(); // Download a new package manifest

  bool is_installed(Package pkg);
  fs::path install_path(Package pkg);

public:
  // the fetcher implementation
  FetchBase *fetch;

private:
  PackageList *package_list;
  ChuckVersion language_version;
  ApiVersion api_version;
  fs::path chump_dir;
  std::string os;
  Architecture arch;
  std::string manifest_url;

public:
  // attempts to open json files
  optional<Package> open_package_file(fs::path path);
  optional<InstalledVersion> open_installed_version_file(fs::path path);
  optional<PackageVersion> open_package_version_file(fs::path path);
};

#endif
