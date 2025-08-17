#ifndef __FETCH_H__
#define __FETCH_H__

#include "package.h"

#include <curl/curl.h>
#include <nlohmann/json.hpp>

#include <cstdlib>
#include <fstream>
#include <optional>
#include <regex>

using json = nlohmann::json;

using std::optional;
using std::string;

namespace fs = std::filesystem;

// Fetch should download into a temp dir, not get a package object
// Return file object?
// I don't think installing needs to be broken out

// Abstract base class
class FetchBase {
public:
  virtual ~FetchBase() = default;
  virtual bool fetch(string data, fs::path dir, Package package,
                     PackageVersion ver, fs::path temp_dir, FileType file_type,
                     string checksum) = 0;
  virtual bool fetch_manifest(string url, fs::path dir) = 0;
  virtual optional<int> fetch_newest_manifest_version(string url) = 0;
};

class Fetch : public FetchBase {
public:
  Fetch();
  Fetch(bool render_tui);
  // download a package file from a remote host
  bool fetch(string data, fs::path dir, Package package, PackageVersion ver,
             fs::path temp_dir, FileType file_type, string checksum);
  // download the manifest from a remote host
  bool fetch_manifest(string url, fs::path dir);

  // ping ccrma servers to find out what is the
  // newest available manifest version is. This
  // is to allows chump to inform users if there is
  // a newer version of chump that will be needed
  // to access a new manifest verison, in case
  // there is a breaking change to the manifest schema.
  optional<int> fetch_newest_manifest_version(string url);

public:
  bool isURL(string path);

private:
  static size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream);
  fs::path chump_dir;
  bool render;
};

#endif
