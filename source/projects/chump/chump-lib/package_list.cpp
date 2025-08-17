
#include "package_list.h"

PackageList::PackageList() {
  // throw std::runtime_error("not implemented");
}

PackageList::PackageList(fs::path filepath) {
  std::ifstream f(filepath);

  // TODO better error checks
  if (!f.good()) {
    throw std::invalid_argument("Unable to open Package List \"" +
                                filepath.string() + "\"");
  }

  json data = json::parse(f);
  f.close();

  // parse all the packages
  json j_packages = data["packages"];

  if (data.contains("manifest-version")) {
    if (data["manifest-version"].is_number_integer()) {
      manifest_api_version = data["manifest-version"];
    } else {
      throw std::invalid_argument("'manifest-version' field in " +
                                  filepath.string() +
                                  " must be an integer, exiting...");
    }
  } else {
    throw std::invalid_argument(
        filepath.string() + " does not contain a manifest-version, exiting...");
  }

  for (auto &j_package : j_packages) {
    Package p = j_package.get<Package>();

    packages.push_back(p);
  }
}

PackageList::PackageList(std::vector<Package> _package) { packages = _package; }

optional<Package> PackageList::find_package(string name) {
  for (auto package : packages) {
    if (package.name == name) {
      return package;
    }
  }

  return {};
}

optional<PackageVersion>
PackageList::find_latest_package_version(string name, string os,
                                         Architecture arch, ChuckVersion ck_ver,
                                         ApiVersion api_ver) {
  // TODO get highest version (how do I do that?)
  for (auto package : packages) {
    if (package.name == name) {
      return package.latest_version(os, arch, ck_ver, api_ver);
    }
  }
  return {};
}

optional<PackageVersion> PackageList::find_package_version(string name,
                                                           string version) {
  PackageVersion want(version);

  for (auto package : packages) {
    if (package.name == name) {
      for (auto package_version : package.versions) {
        if (package_version == want) {
          return package_version;
        }
      }
    }
  }
  return {};
}

optional<PackageVersion>
PackageList::find_package_version(string name, PackageVersion version) {
  for (auto package : packages) {
    if (package.name == name) {
      for (auto package_version : package.versions) {
        if (package_version == version) {
          return package_version;
        }
      }
    }
  }
  return {};
}

std::vector<Package> PackageList::get_packages() { return packages; }
