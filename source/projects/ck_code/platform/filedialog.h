// SPDX-License-Identifier: MIT
/* filedialog.h -- modal macOS file panels. Main thread. Return a POSIX path or "". */

#pragma once

#include <string>

namespace ckc {

std::string save_dialog(const std::string &default_name); /* restricted to .ck */
std::string open_dialog();                                 /* any file */

} // namespace ckc
