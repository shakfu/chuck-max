#include <chrono>
#include <filesystem>
#include <iostream>
#include <math.h>
#include <sstream>

#include "fetch.h"
#include "util.h"

#include <openssl/sha.h>

#define CHUMP_PROGRESS_BAR_WIDTH 50
#define CHUMP_PROGRESS_BAR_WIDTH_EXTRA (CHUMP_PROGRESS_BAR_WIDTH + 50)

Fetch::Fetch() { render = false; }

Fetch::Fetch(bool render_tui) { render = render_tui; }

// struct to pass progress data to libcurl progress callback
struct curl_progress {
  string packageName;
  string fileName;
};

// Callback function to update progress
int progressCallback(void *clientp, double dltotal, double dlnow,
                     double ultotal, double ulnow) {
  if (!clientp) {
    std::cerr << "[chump]: progress callback recieved empty clientp"
              << std::endl;
    return -1;
  }
  // metadata for the progress bar

  double progress = 0;
  if (dltotal >= 1.0)
    progress = (dlnow > 0) ? ((double)dlnow / (double)dltotal) : 0.0;

  int bar_width = CHUMP_PROGRESS_BAR_WIDTH; // Width of the progress bar
  int filled_width = progress * bar_width;

  // Get the current time point
  auto currentTime = std::chrono::system_clock::now();

  // Get the duration since the epoch
  auto durationSinceEpoch = currentTime.time_since_epoch();

  // Convert the duration to milliseconds
  auto milliseconds =
      std::chrono::duration_cast<std::chrono::milliseconds>(durationSinceEpoch);

  // Extract the milliseconds component
  long long milliseconds_count = milliseconds.count() % 1000;

  string line;

  if (milliseconds_count < 250) {
    line = "=>";
  } else if (milliseconds_count < 500) {
    line = "=v";
  } else if (milliseconds_count < 750) {
    line = "=<";
  } else {
    line = "=^";
  }

  line = TC::blue(line, TRUE);

  line += " [";
  for (int i = 0; i < bar_width; i++) {
    if (i < filled_width)
      line += TC::green("=", TRUE);
    else if (i == filled_width)
      line += TC::orange("v", TRUE);
    else
      line += " ";
  }

  line += string("] ") + std::to_string((int)(progress * 100 + .5)) +
          "%           ";
  std::cerr << "\r" << line.c_str();

  return 0;
}

//*******************************************
// Download file to proper package directory.
// Return true on success, False on failure.
//*******************************************
bool Fetch::fetch(std::string url, fs::path dir, Package package,
                  PackageVersion ver, fs::path temp_dir, FileType file_type,
                  string checksum) {
  if (!isURL(url)) {
    std::cerr << "[chump]: not a URL!" << std::endl;
    return false;
  }

  fs::path ft_dir = fileTypeToDir(file_type);

  // If the file is in a directory, create it
  fs::create_directory(temp_dir / ft_dir);
  fs::create_directory(temp_dir / ft_dir / dir);

  std::string package_name = package.name;

  // struct progress data;
  CURL *curl;
  FILE *fp;
  CURLcode res;

  fs::path filename = fs::path(url).filename();

  // Generate a unique temporary file name
  fs::path tempFilePath = temp_dir / dir / ft_dir / filename;

#ifdef _MSC_VER
  fp = _wfopen(tempFilePath.c_str(), L"wb");
#else
  fp = fopen(tempFilePath.string().c_str(), "wb");
#endif

  if (!fp) {
    std::cerr << "[chump]: error opening file for writing" << std::endl;
    return false;
  }

  // Initialize libcurl
  curl = curl_easy_init();

  if (curl) {
    // Set URL
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

    // Set callback function to write data
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);

    // Set file to write to
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

    struct curl_progress data;
    data.packageName = package.name;
    data.fileName = filename.string();

    // Set the progress callback function
    if (render) {
      curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, &data);
      curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, progressCallback);
    }
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);

    // We don't want to write the error to a file if the request fails
    curl_easy_setopt(curl, CURLOPT_FAILONERROR, true);

    // Perform the request
    string line = TC::orange("down-chucking package", TRUE) + " ";
    line += TC::bold(package.name) + " (" + ver.getVersionString() + ")" +
            TC::orange(" ├─ ", TRUE) + filename.string() + "\n";
    std::cerr << line;

    res = curl_easy_perform(curl);

    // Clean up
    curl_easy_cleanup(curl);
    fclose(fp);

    if (res != CURLE_OK) {
      std::cerr << "[chump]: failed to download: " << curl_easy_strerror(res)
                << std::endl;
      return false;
    }

  } else {
    std::cerr << "[chump]: failed to initialize libcurl" << std::endl;
    return false;
  }

  std::cerr << std::endl;
  if (hash_file(tempFilePath) != checksum) {
    std::cerr << "[chump]: the downloaded file (" << url
              << ") does not match expected hash - aborting" << std::endl;

    return false;
  }

  std::string line = string("   └─[") + TC::green("OK", TRUE) + "] " +
                     TC::blue(filename.string());
  std::cerr << "\r" << std::left << std::setw(CHUMP_PROGRESS_BAR_WIDTH_EXTRA)
            << line << std::endl;

  return true;
}

// Callback function to write data into a file
size_t Fetch::write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
  size_t written = fwrite(ptr, size, nmemb, stream);
  return written;
}

bool Fetch::isURL(std::string path) {
  // https://www.geeksforgeeks.org/check-if-an-url-is-valid-or-not-using-regular-expression/#
  const std::regex pattern(
      "((http|https)://)(www.)?[a-zA-Z0-9@:%._\\+~#?&//"
      "=]{2,256}\\.[a-z]{2,6}\\b([-a-zA-Z0-9@:%._\\+~#?&//=]*)");

  return regex_match(path, pattern);
}

bool Fetch::fetch_manifest(std::string url, fs::path dir) {
  if (!isURL(url)) {
    std::cerr << "[chump]: not a URL!" << std::endl;
    return false;
  }

  // struct progress data;
  CURL *curl;
  FILE *fp;
  CURLcode res;

  fs::path tempFilePath = dir / "manifest.json";

#ifdef _MSC_VER
  fp = _wfopen(tempFilePath.c_str(), L"wb");
#else
  fp = fopen(tempFilePath.string().c_str(), "wb");
#endif

  if (!fp) {
    std::cerr << "[chump]: error opening file for writing manifest.json"
              << std::endl;
    return false;
  }

  // Initialize libcurl
  curl = curl_easy_init();

  if (curl) {
    // Set URL
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

    // Set callback function to write data
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);

    // Set file to write to
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

    struct curl_progress data;
    data.packageName = "manifest";
    data.fileName = "manifest.json";

    // We don't want to write the error to a file if the request fails
    curl_easy_setopt(curl, CURLOPT_FAILONERROR, true);

    // Perform the request
    res = curl_easy_perform(curl);

    // Clean up
    curl_easy_cleanup(curl);
    fclose(fp);

    if (res != CURLE_OK) {
      std::cerr << "[chump]: failed to download manifest.json: "
                << curl_easy_strerror(res) << std::endl;
      return false;
    }

  } else {
    std::cerr << "[chump]: failed to initialize libcurl" << std::endl;
    return false;
  }

  return true;
}

struct MemoryStruct {
  char *memory;
  size_t size;
};

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb,
                                  void *userp) {
  size_t realsize = size * nmemb;
  auto &mem = *static_cast<std::string *>(userp);
  mem.append(static_cast<char *>(contents), realsize);
  return realsize;
}

optional<int> Fetch::fetch_newest_manifest_version(string url) {
  if (!isURL(url)) {
    std::cerr << "[chump]: not a URL!" << std::endl;
    return {};
  }

  CURL *curl_handle;
  CURLcode res;

  std::string chunk;

  curl_global_init(CURL_GLOBAL_ALL);

  /* init the curl session */
  curl_handle = curl_easy_init();

  /* specify URL to get */
  curl_easy_setopt(curl_handle, CURLOPT_URL, url.c_str());

  /* send all data to this function  */
  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

  /* we pass our 'chunk' struct to the callback function */
  curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &chunk);

  /* some servers do not like requests that are made without a user-agent
     field, so we provide one */
  curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

  /* get it! */
  res = curl_easy_perform(curl_handle);

  /* check for errors */
  if (res != CURLE_OK) {
    fprintf(stderr, "[chump]: failed to fetch latest manifest version: %s\n",
            curl_easy_strerror(res));
    return {};
  }

  /* cleanup curl stuff */
  curl_easy_cleanup(curl_handle);

  // free(chunk.memory);

  /* we are done with libcurl, so clean it up */
  curl_global_cleanup();

  try {
    int ver_no = std::stoi(chunk);
    return ver_no;
  } catch (std::invalid_argument const &ex) {
    std::cerr << "[chump]: failed to fetch latest manifest version: "
              << ex.what() << std::endl;
    return {};
  }

  return {};
}
