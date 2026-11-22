// SPDX-License-Identifier: MIT
/* filedialog.mm -- see filedialog.h. Manual reference counting (no ARC). */

#include "platform/filedialog.h"

#import <AppKit/AppKit.h>
#import <UniformTypeIdentifiers/UniformTypeIdentifiers.h>

static std::string url_path(NSURL *url) {
	if (!url || ![url isFileURL])
		return "";
	const char *p = [[url path] fileSystemRepresentation];
	return p ? p : "";
}

std::string ckc::save_dialog(const std::string &default_name) {
	std::string out;
	@autoreleasepool {
		NSSavePanel *panel = [NSSavePanel savePanel];
		UTType *ck = [UTType typeWithFilenameExtension:@"ck"]; /* macOS 11+; the minimum is 12 */
		if (ck)
			[panel setAllowedContentTypes:@[ ck ]];
		NSString *name = [NSString stringWithUTF8String:default_name.c_str()];
		if (name)
			[panel setNameFieldStringValue:name];
		if ([panel runModal] == NSModalResponseOK)
			out = url_path([panel URL]);
	}
	return out;
}

std::string ckc::open_dialog() {
	std::string out;
	@autoreleasepool {
		NSOpenPanel *panel = [NSOpenPanel openPanel];
		[panel setCanChooseFiles:YES];
		[panel setCanChooseDirectories:NO];
		[panel setAllowsMultipleSelection:NO];
		if ([panel runModal] == NSModalResponseOK)
			out = url_path([[panel URLs] firstObject]);
	}
	return out;
}
