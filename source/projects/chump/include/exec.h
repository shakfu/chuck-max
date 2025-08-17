#define NOMINMAX // to get std::min/std::max working on windows
#include <cctype>
#include <chrono>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <random>
#include <sstream>
#include <stdlib.h>
#include <thread>
#include <tuple>
#include <vector>

#include "manager.h"
#include "util.h"

#if defined(_WIN32)
#include <Windows.h>
#else
#include <sys/ioctl.h>
#endif

using std::endl;

namespace fs = std::filesystem;

std::string printLogo();
void printPackage(Package pkg);
void printPackages(Manager *mgr);

void printPackage(Package pkg) { std::cout << pkg << std::endl; }

void printAllPackages(Manager *mgr);
void printInstalledPackages(Manager *mgr);
void printPackagesMultiLine(Manager *mgr, bool print_installed);

// https://stackoverflow.com/questions/19580877/how-to-truncate-a-string-formating-c
std::string truncate(std::string str, size_t width, bool show_ellipsis = true) {
  if (str.length() > width) {
    if (show_ellipsis) {
      str.resize(width);
      return str + "...";
    } else {
      str.resize(width);
      return str;
    }
  }
  return str;
}

// Function used in the `chump list` command
void printPackages(Manager *mgr, bool print_installed) {
  printPackagesMultiLine(mgr, print_installed);
  // if (print_installed)
  //   printInstalledPackages(mgr);
  // else
  //   printAllPackages(mgr);
}

void printPackagesMultiLine(Manager *mgr, bool print_installed) {
  vector<Package> packages = mgr->listPackages();
  // sort package list lexicographically
  std::sort(packages.begin(), packages.end());

  for (Package p : packages) {
    if (print_installed && !mgr->is_installed(p))
      continue;

    optional<PackageVersion> latest = mgr->latestPackageVersion(p.name);

    string latest_version = "Not available on this OS";
    if (latest)
      latest_version = latest.value().getVersionString();

    string installed = mgr->is_installed(p) ? "yes" : "no";

    fs::path install_path = mgr->install_path(p);

    optional<InstalledVersion> installed_version =
        mgr->open_installed_version_file(install_path / "version.json");

    std::cout << TC::bold(p.name) << std::endl;

    if (installed_version) {
      std::cout << " ├─ " << TC::green("Installed Version: ", TRUE)
                << installed_version.value().getVersionString();

      if (latest && installed_version.value().version() < latest.value()) {
        std::cout << " (update available!)\n";
      } else {
        std::cout << std::endl;
      }

      std::cout << " │  └─ " << TC::green("Install Path: ", TRUE)
                << install_path.string() << std::endl;
    }

    std::cout << " ├─ " << TC::blue("Latest Version: ", TRUE) << latest_version
              << std::endl;
    if (!p.keywords.empty()) {
      std::cout << " ├─ " << TC::blue("Keywords: ", TRUE) << "[";
      std::cout << p.keywords[0];

      for (size_t i = 1; i < p.keywords.size(); ++i) {
        std::cout << ", " << p.keywords[i];
      }
      std::cout << "]" << std::endl;
    }
    std::cout << " └─ " << TC::blue("Description: ", TRUE) << p.description
              << std::endl;
  }
}

// Print all packages, regardless of whether they're installed or not
void printAllPackages(Manager *mgr) {
  vector<Package> packages = mgr->listPackages();
  // sort package list lexicographically
  std::sort(packages.begin(), packages.end());

  std::cout << std::left << std::setw(20) << "Name"
            << "| " << std::setw(12) << "Latest Ver."
            << "| " << std::setw(12) << "Installed?"
            << "| " << std::setw(12) << "Description" << std::endl;

  std::cout << std::setfill('-'); // in-between line from header to table

  std::cout << std::right << std::setw(21) << "|" << std::setw(14) << "|"
            << std::setw(14) << "|" << std::setw(14) << "" << std::endl;

  std::cout << std::setfill(' ');

  for (Package p : packages) {
    optional<PackageVersion> latest = mgr->latestPackageVersion(p.name);

    string latest_version = "N/A";
    if (latest)
      latest_version = latest.value().getVersionString();

    string installed = mgr->is_installed(p) ? "yes" : "no";

    std::cout << std::left << std::setw(20) << truncate(p.name, 17, true)
              << "| " << std::setw(12) << latest_version << "| "
              << std::setw(12) << installed << "| " << std::setw(12)
              << truncate(p.description, 100, true) << std::endl;
  }
}

// Print only installed packages (and relevant information such as install path)
void printInstalledPackages(Manager *mgr) {
  vector<Package> packages = mgr->listPackages();
  // sort package list lexicographically
  std::sort(packages.begin(), packages.end());

  // get maximum package path length
  int max_path_len = string("Install Path").length();
  int max_ver_len = string("Ver.").length();

  // get the character width of the longest install path and version string
  for (Package p : packages) {
    // skip if not installed
    if (!mgr->is_installed(p))
      continue;

    fs::path install_path = mgr->install_path(p);
    int curr_path_len = install_path.string().length();

    max_path_len = std::max(max_path_len, curr_path_len);

    optional<InstalledVersion> installed_version =
        mgr->open_installed_version_file(install_path / "version.json");

    if (installed_version) {
      string version = installed_version.value().getVersionString();
      int curr_ver_len = version.length();

      max_ver_len = std::max(max_ver_len, curr_ver_len);
    }
  }

  std::cout << std::left << std::setw(20) << "Name"
            << "| " << std::setw(max_ver_len + 1) << "Ver."
            << "| " << std::setw(max_path_len + 1) << "Install Path"
            << "| " << std::setw(12) << "Description" << std::endl;

  std::cout << std::setfill('-'); // in-between line from header to table

  std::cout << std::right << std::setw(21) << "|" << std::setw(max_ver_len + 3)
            << "|" << std::setw(max_path_len + 3) << "|" << std::setw(14) << ""
            << std::endl;

  std::cout << std::setfill(' ');

  for (Package p : packages) {
    if (!mgr->is_installed(p))
      continue;

    fs::path install_path = mgr->install_path(p);

    optional<InstalledVersion> installed_version =
        mgr->open_installed_version_file(install_path / "version.json");

    string version = "N/A";
    if (installed_version)
      version = installed_version.value().getVersionString();

    std::cout << std::left << std::setw(20) << truncate(p.name, 17, true)
              << "| " << std::setw(max_ver_len + 1) << version << "| "
              << std::setw(max_path_len + 1) << install_path.string() << "| "
              << std::setw(12) << truncate(p.description, 100, true)
              << std::endl;
  }
}

void printPackage(Manager *mgr, Package p) {
  optional<PackageVersion> latest = mgr->latestPackageVersion(p.name);

  string latest_version = "Not available on this OS";
  if (latest)
    latest_version = latest.value().getVersionString();

  string installed = mgr->is_installed(p) ? "yes" : "no";

  fs::path install_path = mgr->install_path(p);

  optional<InstalledVersion> installed_version =
      mgr->open_installed_version_file(install_path / "version.json");

  std::cout << TC::bold(p.name) << std::endl;

  if (installed_version) {
    std::cout << " ├─ " << TC::green("Installed Version: ", TRUE)
              << installed_version.value().getVersionString();

    if (latest && installed_version.value().version() < latest.value()) {
      std::cout << " (update available!)\n";
    } else {
      std::cout << std::endl;
    }

    std::cout << " │  └─ " << TC::green("Install Path: ", TRUE) << install_path
              << std::endl;
  }

  std::cout << " ├─ " << TC::blue("Latest Version: ", TRUE) << latest_version
            << std::endl;
  if (!p.keywords.empty()) {
    std::cout << " ├─ " << TC::blue("Keywords: ", TRUE) << "[";
    std::cout << p.keywords[0];

    for (size_t i = 1; i < p.keywords.size(); ++i) {
      std::cout << ", " << p.keywords[i];
    }
    std::cout << "]" << std::endl;
  }

  std::cout << " ├─ " << TC::blue("Authors: ", TRUE);

  if (p.authors.size() == 1) {
    std::cout << p.authors[0] << "\n";
  } else if (p.authors.size() > 1) {
    std::cout << "[" << p.authors[0];

    for (size_t i = 1; i < p.authors.size(); ++i) {
      std::cout << ", " << p.authors[i];
    }
    std::cout << "]\n";
  }

  std::cout << " ├─ " << TC::blue("License: ", TRUE) << p.license << std::endl;
  ;
  std::cout << " ├─ " << TC::blue("Homepage: ", TRUE) << p.homepage
            << std::endl;
  std::cout << " ├─ " << TC::blue("Respository: ", TRUE) << p.repository
            << std::endl;

  std::cout << " ├─ " << TC::blue("Description: ", TRUE) << p.description
            << std::endl;
  std::cout << " │\n";

  std::cout << " └─ " << TC::blue("Versions ", TRUE) << std::endl;

  for (int i = 0; i < p.versions.size(); i++) {
    string prepend = "    │";
    if (i == p.versions.size() - 1) {
      std::cout << "    └─ " << TC::blue("Version: ", TRUE);
      prepend = "     ";
    } else {
      std::cout << "    ├─ " << TC::blue("Version: ", TRUE);
    }

    const auto &ver = p.versions[i];

    // std::cout << " └─ " << TC::blue("Versions ", TRUE) << std::endl;
    std::cout << ver.major << "." << ver.minor << "." << ver.patch << std::endl;
    std::cout << prepend << "  ├─ " << TC::blue("Min ChucK Version: ", TRUE)
              << ver.language_version_min << std::endl;

    if (ver.language_version_max) {
      std::cout << prepend << "  ├─ " << TC::blue("Max ChucK Version: ", TRUE)
                << ver.language_version_max.value() << std::endl;
    }
    if (ver.api_version) {
      std::cout << prepend << "  ├─ " << TC::blue("Chugin API Version: ", TRUE);
      std::cout << ver.api_version.value() << std::endl;
    }
    std::cout << prepend << "  ├─ " << TC::blue("Operating System: ", TRUE)
              << ver.os << std::endl;
    std::cout << prepend << "  ├─ " << TC::blue("Architecture: ", TRUE)
              << architectureToString.at(ver.arch) << std::endl;
    std::cout << prepend << "  └─ " << TC::blue("Files ", TRUE) << std::endl;

    for (int j = 0; j < ver.files.size(); j++) {
      if (j == ver.files.size() - 1)
        std::cout << prepend << "     └─ ";
      else
        std::cout << prepend << "     ├─ ";

      std::cout << ver.files[j].url << std::endl;
    }
  }
}

void get_terminal_size(int &width, int &height) {
  width = height = 0;
#if defined(_WIN32)
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
  width = (int)(csbi.srWindow.Right - csbi.srWindow.Left + 1);
  height = (int)(csbi.srWindow.Bottom - csbi.srWindow.Top + 1);
#elif defined(__linux__) || defined(__APPLE__)
  struct winsize w;
  ioctl(fileno(stdout), TIOCGWINSZ, &w);
  width = (int)(w.ws_col);
  height = (int)(w.ws_row);
#endif // Windows/Linux
}

void clear() {
#if defined _WIN32
  system("cls");
  // clrscr(); // including header file : conio.h
#elif defined(__LINUX__) || defined(__gnu_linux__) || defined(__linux__)
  system("clear");
  // std::cout<< u8"\033[2J\033[1;1H"; //Using ANSI Escape Sequences
#elif defined(__APPLE__)
  system("clear");
#endif
}

std::string jumbleColors(std::string str, int counter) {
  std::default_random_engine generator;
  std::uniform_int_distribution<int> distribution(0, 3);

  string output = "";

  for (std::string::size_type i = 0; i < str.size(); ++i) {
    int val = distribution(generator);
    val = (val + counter) % 4;
    // int val = rand() % 4;

    int bold = distribution(generator) % 2;

    std::string substr = str.substr(i, 1);

    int bg = distribution(generator);
    bg = (bg + counter) % 4;
    bg = (counter + i) % 4;

    if (std::isspace(str[i])) {
      output += substr;
      continue;
    }

    switch (val) {
    case 0:
      output += TC::green(substr, bold);
      break;
    case 1:
      output += TC::orange(substr, TRUE);
      break;
    case 2:
      output += TC::blue(substr, bold);
      break;
    case 3:
      output += TC::magenta(substr, bold);
      break;
    default:
      break;
    }
  }

  return output;
}

std::string riverColors(std::string str, int counter) {
  std::default_random_engine generator;
  std::uniform_int_distribution<int> distribution(0, 3);

  string output = "";

  for (std::string::size_type i = 0; i < str.size(); ++i) {
    int val = distribution(generator);
    val = (val + counter) % 4;
    // int val = rand() % 4;

    int bold = distribution(generator) % 2;

    std::string substr = str.substr(i, 1);

    int bg = distribution(generator);
    bg = (bg + counter) % 4;
    bg = (counter + i) % 4;

    if (std::isspace(str[i])) {
      switch (bg) {
      case 0:
        output += TC::set(TC::BG_CYAN);
        break;
      case 1:
        output += TC::set(TC::BG_MAGENTA);
        break;
      case 2:
        output += TC::set(TC::BG_WHITE);
        break;
      case 3:
        output += TC::set(TC::BG_DEFAULT);
        break;
      default:
        break;
      }
      output += substr + TC::reset();
      continue;
    }

    output += TC::set(TC::DIM);
    switch (val) {
    case 0:
      output += substr; // TC::default(substr, bold);
      break;
    case 1:
      output += TC::red(substr, TRUE);
      break;
    case 2:
      output += TC::color(TC::FG_BLACK, substr, bold);
      break;
    case 3:
      output += TC::magenta(substr, bold);
      break;
    default:
      break;
    }
  }

  return output;
}

std::string dimColors(std::string str, int counter) {
  std::default_random_engine generator;
  std::uniform_int_distribution<int> distribution(0, 3);

  string output = "";

  for (std::string::size_type i = 0; i < str.size(); ++i) {
    int val = distribution(generator);
    val = (val + counter) % 4;

    int bold = distribution(generator) % 2;

    std::string substr = str.substr(i, 1);

    int bg = distribution(generator);
    bg = (bg + counter) % 4;
    bg = (int(counter / 2) + i) % 4;

    output += TC::set(TC::DIM);

    if (std::isspace(str[i]) && substr != "\n") {
      switch (bg) {
      case 0:
        output += TC::red(".");
        break;
      case 1:
        output += TC::magenta(".");
        break;
      case 2:
        output += TC::color(TC::FG_BLACK, ".", bold);
        break;
      case 3:
        output += TC::color(TC::FG_WHITE, ".", bold);
        break;
      default:
        break;
      }
      output += TC::reset();
      continue;
    }

    if (std::isspace(str[i])) {
      output += substr;
      continue;
    }

    switch (val) {
    case 0:
      output += substr; // TC::default(substr, bold);
      break;
    case 1:
      output += TC::blue(substr, FALSE);
      break;
    case 2:
      output += TC::color(TC::FG_WHITE, substr, bold);
      break;
    case 3:
      output += TC::orange(substr, bold);
      break;
    default:
      break;
    }
  }

  return output;
}

std::string bedtimeColors(std::string str, int counter) {
  std::default_random_engine generator;
  std::uniform_int_distribution<int> distribution(0, 3);

  string output = "";

  for (std::string::size_type i = 0; i < str.size(); ++i) {
    int val = distribution(generator);
    val = (val + counter) % 4;

    int bold = distribution(generator) % 2;

    std::string substr = str.substr(i, 1);

    int bg = distribution(generator);
    bg = (bg + counter) % 4;
    bg = (counter + i) % 4;

    output += TC::set(TC::DIM);

    if (std::isspace(str[i])) {
      switch (bg) {
      case 0:
        output += TC::red(".");
        break;
      case 1:
        output += TC::magenta(".");
        break;
      case 2:
        output += TC::color(TC::FG_BLACK, ".", bold);
        break;
      case 3:
        output += TC::color(TC::FG_WHITE, ".", bold);
        break;
      default:
        break;
      }
      output += TC::reset();
      continue;
    }

    switch (val) {
    case 0:
      output += substr; // TC::default(substr, bold);
      break;
    case 1:
      output += TC::red(substr, FALSE);
      break;
    case 2:
      output += TC::color(TC::FG_BLACK, substr, bold);
      break;
    case 3:
      output += TC::magenta(substr, bold);
      break;
    default:
      break;
    }
  }

  return output;
}

std::string bedtimeLogo(int counter) {
  // clang-format off
  std::string logo = R"(
                                                 ss
                                                 rox        zop
                                                 xor        ony
              tkjiimtz             yrljjjjjjox    oo       qnx
                  zsuckerlrwzzxsliiimty           sov     wor
                        xrmjjjjmsz                 oo     poz
                                                   row   yoq
                               xqnrutchumpmv        oo   tov
                          zskiijnpmklry             uot  oo
                       yohhmu                        pnyzop
             uljjjjjjhippy                           znopoy
                                                      vamp
                                                       vmr         zxwvx
                                                              zxxwwwy  ywwz
            oo  yy                                          wwwxxz       zxvx
        ysoooo  xw                                         xx               ww
      womsz     ywy                                        zwx              xx
     tmt         wx                                         ww              ww
    xnq          ww                   zz                    zwx             ww
   zno           ww                    yz     zz zyz         ww            xvz
   poy           ww                    zyz    xvuvvv   vx    ywxyxyzywxxwwxz
   oo            xw     zxy    yxz      zyz   xtv  wwwwzwvz  zuuxyxxy
   oo            zwx    wtw    zwx       yz   ywx  xtvy  wwz  xx
   tmntoooooopv   ww   wwww     yxz      zzz  zvw  zww   ww   xx
    tmoooxxywxy   ywz xwzxwz    zxyz     zzz  zvw       zwx   ww
                  zwx wx  ww    zz yyz   yy    ww       ywy   xw
                   xwvw    wvy  yy  zyyzyz     xx       xwz   zwy
                    vtx                z               zyz    xx
                                                               xw
                                                                xy
                --- ChuMP! the CHUck Manager of Packages ---
  )";
  // clang-format on

  auto num_lines = std::count(logo.begin(), logo.end(), '\n');
  num_lines += (!logo.empty() && logo.back() != '\n');

  logo = bedtimeColors(logo, counter);

  return logo;
}

std::string dimLogo(int counter) {
  int width = 0, height = 0;
  get_terminal_size(width, height);

  // 81 is the width of the chump logo
  int padding_len = (width - 81) / 2;
  if (padding_len < 0)
    padding_len = 0;

  std::string padding = "";

  for (int i = 0; i < padding_len; i++) {
    padding += " ";
  }

  // clang-format off
  std::vector<std::string> logo = {
      "                                                                                 ",
      "                                                 ss                              ",
      "                                                 rox        zop                  ",
      "                                                 xor        ony                  ",
      "              tkjiimtz             yrljjjjjjox    oo       qnx                   ",
      "                  zsuckerlrwzzxsliiimty           sov     wor                    ",
      "                        xrmjjjjmsz                 oo     poz                    ",
      "                                                   row   yoq                     ",
      "                               xqnrutchumpmv        oo   tov                     ",
      "                          zskiijnpmklry             uot  oo                      ",
      "                       yohhmu                        pnyzop                      ",
      "             uljjjjjjhippy                           znopoy                      ",
      "                                                      vamp                       ",
      "                                                       vmr         zxwvx         ",
      "                                                              zxxwwwy  ywwz      ",
      "            oo  yy                                          wwwxxz       zxvx    ",
      "        ysoooo  xw                                         xx               ww   ",
      "      womsz     ywy                                        zwx              xx   ",
      "     tmt         wx                                         ww              ww   ",
      "    xnq          ww                   zz                    zwx             ww   ",
      "   zno           ww                    yz     zz zyz         ww            xvz   ",
      "   poy           ww                    zyz    xvuvvv   vx    ywxyxyzywxxwwxz     ",
      "   oo            xw     zxy    yxz      zyz   xtv  wwwwzwvz  zuuxyxxy            ",
      "   oo            zwx    wtw    zwx       yz   ywx  xtvy  wwz  xx                 ",
      "   tmntoooooopv   ww   wwww     yxz      zzz  zvw  zww   ww   xx                 ",
      "    tmoooxxywxy   ywz xwzxwz    zxyz     zzz  zvw       zwx   ww                 ",
      "                  zwx wx  ww    zz yyz   yy    ww       ywy   xw                 ",
      "                   xwvw    wvy  yy  zyyzyz     xx       xwz   zwy                ",
      "                    vtx                z               zyz    xx                 ",
      "                                                               xw                ",
      "                                                                xy               ",
      "                --- ChuMP! the CHUck Manager of Packages ---                     ",
      "                                                                                 ",
  };
  // clang-format on

  std::string empty_line(width, ' ');
  int padding_h = (height - logo.size()) / 2;

  std::string logo_combined;

  for (int i = 0; i < padding_h; i++) {
    logo_combined += empty_line + "\n";
  }

  for (auto line : logo) {
    logo_combined += padding + line + padding + "\n";
  }

  for (int i = 0; i < padding_h; i++) {
    logo_combined += empty_line + "\n";
  }

  logo_combined = dimColors(logo_combined, counter);

  return logo_combined;
}

std::string riverLogo(int counter) {
  int width = 0, height = 0;
  get_terminal_size(width, height);

  // 81 is the width of the chump logo
  int padding_len = (width - 81) / 2;
  if (padding_len < 0)
    padding_len = 0;

  std::string padding = "";

  for (int i = 0; i < padding_len; i++) {
    padding += " ";
  }

  // clang-format off
  std::vector<std::string> logo = {
      "                                                 ss            ",
      "                                                 rox        zop ",
      "                                                 xor        ony ",
      "              tkjiimtz             yrljjjjjjox    oo       qnx ",
      "                  zsuckerlrwzzxsliiimty           sov     wor ",
      "                        xrmjjjjmsz                 oo     poz ",
      "                                                   row   yoq ",
      "                               xqnrutchumpmv        oo   tov ",
      "                          zskiijnpmklry             uot  oo ",
      "                       yohhmu                        pnyzop ",
      "             uljjjjjjhippy                           znopoy ",
      "                                                      vamp              ",
      "                                                       vmr         zxwvx ",
      "                                                              zxxwwwy  ywwz ",
      "            oo  yy                                          wwwxxz       zxvx ",
      "        ysoooo  xw                                         xx               ww ",
      "      womsz     ywy                                        zwx              xx ",
      "     tmt         wx                                         ww              ww ",
      "    xnq          ww                   zz                    zwx             ww ",
      "   zno           ww                    yz     zz zyz         ww            xvz ",
      "   poy           ww                    zyz    xvuvvv   vx    ywxyxyzywxxwwxz ",
      "   oo            xw     zxy    yxz      zyz   xtv  wwwwzwvz  zuuxyxxy       ",
      "   oo            zwx    wtw    zwx       yz   ywx  xtvy  wwz  xx ",
      "   tmntoooooopv   ww   wwww     yxz      zzz  zvw  zww   ww   xx ",
      "    tmoooxxywxy   ywz xwzxwz    zxyz     zzz  zvw       zwx   ww ",
      "                  zwx wx  ww    zz yyz   yy    ww       ywy   xw ",
      "                   xwvw    wvy  yy  zyyzyz     xx       xwz   zwy ",
      "                    vtx                z               zyz    xx ",
      "                                                               xw ",
      "                                                                xy ",
      "                --- ChuMP! the CHUck Manager of Packages ---      ",
  };
  // clang-format on
  std::string logo_combined;

  for (auto line : logo) {
    logo_combined += padding + line + "\n";
  }

  logo_combined = riverColors(logo_combined, counter);

  return logo_combined;
}

std::string jumbleLogo(int counter) {
  int width = 0, height = 0;
  get_terminal_size(width, height);

  // 81 is the width of the chump logo
  int padding_len = (width - 81) / 2;
  if (padding_len < 0)
    padding_len = 0;

  std::string padding = "";

  for (int i = 0; i < padding_len; i++) {
    padding += " ";
  }

  // clang-format off
  std::vector<std::string> logo = {
      "                                                 ss",
      "                                                 rox        zop",
      "                                                 xor        ony",
      "              tkjiimtz             yrljjjjjjox    oo       qnx",
      "                  zsuckerlrwzzxsliiimty           sov     wor",
      "                        xrmjjjjmsz                 oo     poz",
      "                                                   row   yoq",
      "                               xqnrutchumpmv        oo   tov",
      "                          zskiijnpmklry             uot  oo",
      "                       yohhmu                        pnyzop",
      "             uljjjjjjhippy                           znopoy",
      "                                                      vamp",
      "                                                       vmr         zxwvx",
      "                                                              zxxwwwy  ywwz",
      "            oo  yy                                          wwwxxz       zxvx",
      "        ysoooo  xw                                         xx               ww",
      "      womsz     ywy                                        zwx              xx",
      "     tmt         wx                                         ww              ww",
      "    xnq          ww                   zz                    zwx             ww",
      "   zno           ww                    yz     zz zyz         ww            xvz",
      "   poy           ww                    zyz    xvuvvv   vx    ywxyxyzywxxwwxz",
      "   oo            xw     zxy    yxz      zyz   xtv  wwwwzwvz  zuuxyxxy",
      "   oo            zwx    wtw    zwx       yz   ywx  xtvy  wwz  xx",
      "   tmntoooooopv   ww   wwww     yxz      zzz  zvw  zww   ww   xx",
      "    tmoooxxywxy   ywz xwzxwz    zxyz     zzz  zvw       zwx   ww",
      "                  zwx wx  ww    zz yyz   yy    ww       ywy   xw",
      "                   xwvw    wvy  yy  zyyzyz     xx       xwz   zwy",
      "                    vtx                z               zyz    xx",
      "                                                               xw",
      "                                                                xy",
      "                --- ChuMP! the CHUck Manager of Packages ---",
  };
  // clang-format on

  std::string logo_combined;

  for (auto line : logo) {
    logo_combined += padding + line + "\n";
  }

  logo_combined = jumbleColors(logo_combined, counter);

  return logo_combined;
}

std::string printLogo() {
  std::ostringstream o;
  // clang-format off
  o << TC::green("                                                 ") << TC::orange("              ") << "               " << "\n";
  o << TC::green("                                                 ") << TC::orange("ss            ") << "               " << "\n";
  o << TC::green("                                                 ") << TC::orange("rox        zop") << "               " << "\n";
  o << TC::green("                                                 ") << TC::orange("xor        ony") << "               " << "\n";
  o << TC::green("              tkjiimtz             yrljjjjjjox   ") << TC::orange(" oo       qnx ") << "               " << "\n";
  o << TC::green("                  zsuckerlrwzzxsliiimty          ") << TC::orange(" sov     wor  ") << "               " << "\n";
  o << TC::green("                        xrmjjjjmsz               ") << TC::orange("  oo     poz  ") << "               " << "\n";
  o << TC::green("                                                 ") << TC::orange("  row   yoq   ") << "               " << "\n";
  o << TC::green("                               xqnrutchumpmv     ") << TC::orange("   oo   tov   ") << "               " << "\n";
  o << TC::green("                          zskiijnpmklry          ") << TC::orange("   uot  oo    ") << "               " << "\n";
  o << TC::green("                       yohhmu                    ") << TC::orange("    pnyzop    ") << "               " << "\n";
  o << TC::green("             uljjjjjjhippy                       ") << TC::orange("    znopoy    ") << "               " << "\n";
  o << TC::green("                                                 ") << TC::orange("     vamp     ") << "               " << "\n";
  o << TC::green("                                                 ") << TC::orange("      vmr     ") <<                                  TC::orange("    zxwvx      ") << "\n";
  o << TC::green("                                                 ") << TC::orange("             ") <<                                  TC::orange("zxxwwwy  ywwz   ") << "\n";
  o << TC::blue("            oo ") << TC::red(" yy            ") << TC::orange("              ") << TC::magenta("                ") << TC::orange("wwwxxz       zxvx ") << "\n";
  o << TC::blue("        ysoooo ") << TC::red(" xw            ") << TC::orange("              ") << TC::magenta("              ") << TC::orange(" xx               ww") << "\n";
  o << TC::blue("      womsz    ") << TC::red(" ywy           ") << TC::orange("              ") << TC::magenta("              ") << TC::orange(" zwx              xx") << "\n";
  o << TC::blue("     tmt       ") << TC::red("  wx           ") << TC::orange("              ") << TC::magenta("               ") << TC::orange(" ww              ww") << "\n";
  o << TC::blue("    xnq        ") << TC::red("  ww           ") << TC::orange("        zz    ") << TC::magenta("               ") << TC::orange(" zwx             ww") << "\n";
  o << TC::blue("   zno         ") << TC::red("  ww           ") << TC::orange("         yz   ") << TC::magenta("  zz zyz        ") << TC::orange(" ww            xvz") << "\n";
  o << TC::blue("   poy         ") << TC::red("  ww           ") << TC::orange("         zyz  ") << TC::magenta("  xvuvvv   vx   ") << TC::orange(" ywxyxyzywxxwwxz  ") << "\n";
  o << TC::blue("   oo          ") << TC::red("  xw     zxy   ") << TC::orange(" yxz      zyz ") << TC::magenta("  xtv  wwwwzwvz ") << TC::orange(" zuuxyxxy         ") << "\n";
  o << TC::blue("   oo          ") << TC::red("  zwx    wtw   ") << TC::orange(" zwx       yz ") << TC::magenta("  ywx  xtvy  wwz") << TC::orange("  xx              ") << "\n";
  o << TC::blue("   tmntoooooopv") << TC::red("   ww   wwww   ") << TC::orange("  yxz      zzz") << TC::magenta("  zvw  zww   ww ") << TC::orange("  xx              ") << "\n";
  o << TC::blue("    tmoooxxywxy") << TC::red("   ywz xwzxwz  ") << TC::orange("  zxyz     zzz") << TC::magenta("  zvw       zwx ") << TC::orange("  ww              ") << "\n";
  o << TC::blue("               ") << TC::red("   zwx wx  ww  ") << TC::orange("  zz yyz   yy ") << TC::magenta("   ww       ywy ") << TC::orange("  xw              ") << "\n";
  o << TC::blue("               ") << TC::red("    xwvw    wvy") << TC::orange("  yy  zyyzyz  ") << TC::magenta("   xx       xwz ") << TC::orange("  zwy             ") << "\n";
  o << TC::blue("               ") << TC::red("     vtx       ") << TC::orange("         z    ") << TC::magenta("           zyz  ") << TC::orange("  xx              ") << "\n";
  o << TC::blue("               ") << TC::red("               ") << TC::orange("              ") << TC::magenta("                ") << TC::orange("   xw             ") << "\n";
  o << TC::blue("               ") << TC::red("               ") << TC::orange("              ") << TC::magenta("                ") << TC::orange("    xy            ") << "\n";
  o << "                --- ChuMP! the CHUck Manager of Packages ---                  " << "\n";
  // clang-format on
  return o.str();
}
