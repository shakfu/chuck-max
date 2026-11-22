// SPDX-License-Identifier: MIT
/* clipboard_win.cpp -- see clipboard.h. Win32 CF_UNICODETEXT, converted to and
 * from UTF-8; newlines become CRLF on the way out, as Windows text expects. */

#include "platform/clipboard.h"

#include <windows.h>

std::string ckc::clipboard_text() {
	std::string out;
	if (!IsClipboardFormatAvailable(CF_UNICODETEXT) || !OpenClipboard(nullptr))
		return out;
	if (HANDLE h = GetClipboardData(CF_UNICODETEXT)) {
		if (const wchar_t *w = (const wchar_t *)GlobalLock(h)) {
			const int n = WideCharToMultiByte(CP_UTF8, 0, w, -1, nullptr, 0, nullptr, nullptr);
			if (n > 1) {
				out.resize((size_t)n - 1);
				WideCharToMultiByte(CP_UTF8, 0, w, -1, &out[0], n, nullptr, nullptr);
			}
			GlobalUnlock(h);
		}
	}
	CloseClipboard();
	return out; /* CRLF is folded by the caller */
}

void ckc::set_clipboard_text(const std::string &s) {
	std::string crlf;
	crlf.reserve(s.size());
	for (char c : s) {
		if (c == '\n')
			crlf += '\r';
		crlf += c;
	}
	const int n = MultiByteToWideChar(CP_UTF8, 0, crlf.data(), (int)crlf.size(), nullptr, 0);
	HGLOBAL g = GlobalAlloc(GMEM_MOVEABLE, ((size_t)n + 1) * sizeof(wchar_t));
	if (!g)
		return;
	wchar_t *w = (wchar_t *)GlobalLock(g);
	MultiByteToWideChar(CP_UTF8, 0, crlf.data(), (int)crlf.size(), w, n);
	w[n] = 0;
	GlobalUnlock(g);
	if (!OpenClipboard(nullptr)) {
		GlobalFree(g);
		return;
	}
	EmptyClipboard();
	if (!SetClipboardData(CF_UNICODETEXT, g))
		GlobalFree(g); /* on success the clipboard owns it */
	CloseClipboard();
}
