// SPDX-License-Identifier: MIT
/* paths.h -- path helpers for Finder drops and attribute values. */

#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace ckc {

/* `Macintosh HD:/Users/a/x.wav` -> `/Users/a/x.wav`; other input unchanged,
 * including a Windows drive (`C:/x.wav`). */
std::string posix_path(const std::string &s);

/* `~` or `~/...` -> home + rest. Empty home: unchanged. `~user` unchanged. */
std::string expand_home(const std::string &home, const std::string &s);

/* Trims whitespace and drops one pair of matching surrounding quotes. */
std::string unquote(const std::string &s);

/* Double-quoted ChucK string literal; escapes `\`, `"`, newline and CR. */
std::string chuck_string(const std::string &s);

/* Each name through posix_path, expand_home, chuck_string; space-joined. */
std::string drop_text(const std::string &home, const std::vector<std::string> &names);

/* A UTF-8 path for std::filesystem and fstream. A plain `std::string` path is
 * read in the ANSI code page on Windows, which breaks non-ASCII names. */
std::filesystem::path fs_path(const std::string &utf8);

} // namespace ckc
