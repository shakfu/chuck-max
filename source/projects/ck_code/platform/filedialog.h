// SPDX-License-Identifier: MIT
/* filedialog.h -- modal file panels. Main thread. Return a path or "".
 * filedialog.mm (macOS), filedialog_win.cpp (Windows). */

#pragma once

#include <string>

namespace ckc {

std::string save_dialog(const std::string &default_name); /* a .ck file */
std::string open_dialog();                                 /* any file */

} // namespace ckc
