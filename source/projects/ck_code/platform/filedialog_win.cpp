// SPDX-License-Identifier: MIT
/* filedialog_win.cpp -- see filedialog.h. Max's own file dialogs, so no Win32
 * dialog code: they return a Max path ID and a name, made absolute here. */

#include "platform/filedialog.h"

#include "ext.h"

/* Absolute path of `name` in Max path `vol`, or "". */
static std::string absolute(short vol, const char *name) {
	char full[MAX_PATH_CHARS];
	if (path_toabsolutesystempath(vol, name, full) != MAX_ERR_NONE)
		return "";
	return full;
}

std::string ckc::save_dialog(const std::string &default_name) {
	char name[MAX_FILENAME_CHARS];
	strncpy_zero(name, default_name.c_str(), MAX_FILENAME_CHARS);
	short vol = 0;
	t_fourcc type = 0;
	if (saveasdialog_extended(name, &vol, &type, nullptr, 0) != 0)
		return ""; /* cancelled */
	std::string path = absolute(vol, name);
	/* no type filter here, so add the extension the macOS panel enforces */
	const size_t slash = path.find_last_of("/\\");
	if (!path.empty() && path.find('.', slash == std::string::npos ? 0 : slash) == std::string::npos)
		path += ".ck";
	return path;
}

std::string ckc::open_dialog() {
	char name[MAX_FILENAME_CHARS] = "";
	short vol = 0;
	t_fourcc type = 0;
	if (::open_dialog(name, &vol, &type, nullptr, 0) != 0)
		return ""; /* cancelled */
	return absolute(vol, name);
}
