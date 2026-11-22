// SPDX-License-Identifier: MIT
/* clipboard.h -- the system clipboard as plain UTF-8 text. Main thread.
 * clipboard.mm (macOS), clipboard_win.cpp (Windows). */

#pragma once

#include <string>

namespace ckc {

std::string clipboard_text(); /* "" when there is no text */
void set_clipboard_text(const std::string &s);

} // namespace ckc
