
// auto-generated chugin query from dear_bindings cimgui.json
// DO NOT EDIT
// author: Andrew Zhu Aday

#include <chuck/chugin.h>

// Wrap this in a namespace to keep it separate from the C++ API
namespace cimgui
{
#include <cimgui/cimgui.h>
}

void ulib_cimgui_query(Chuck_DL_Query* QUERY)
{
    QUERY->begin_class(QUERY, "UI_WindowFlags", "Object");
    QUERY->doc_class(
      QUERY,
      "Flags for ImGui::Begin().\n(Those are per-window flags. There are "
      "shared flags in ImGuiIO: io.ConfigWindowsResizeFromEdges and "
      "io.ConfigWindowsMoveFromTitleBarOnly).\n");
    static t_CKINT ImGuiWindowFlags_None = 0;
    QUERY->add_svar(QUERY, "int", "None", true, &ImGuiWindowFlags_None);
    static t_CKINT ImGuiWindowFlags_NoTitleBar = 1;
    QUERY->add_svar(QUERY, "int", "NoTitleBar", true,
                    &ImGuiWindowFlags_NoTitleBar);
    QUERY->doc_var(QUERY, "Disable title-bar");
    static t_CKINT ImGuiWindowFlags_NoResize = 2;
    QUERY->add_svar(QUERY, "int", "NoResize", true, &ImGuiWindowFlags_NoResize);
    QUERY->doc_var(QUERY, "Disable user resizing with the lower-right grip");
    static t_CKINT ImGuiWindowFlags_NoMove = 4;
    QUERY->add_svar(QUERY, "int", "NoMove", true, &ImGuiWindowFlags_NoMove);
    QUERY->doc_var(QUERY, "Disable user moving the window");
    static t_CKINT ImGuiWindowFlags_NoScrollbar = 8;
    QUERY->add_svar(QUERY, "int", "NoScrollbar", true,
                    &ImGuiWindowFlags_NoScrollbar);
    QUERY->doc_var(QUERY,
                   "Disable scrollbars (window can still scroll with mouse or "
                   "programmatically)");
    static t_CKINT ImGuiWindowFlags_NoScrollWithMouse = 16;
    QUERY->add_svar(QUERY, "int", "NoScrollWithMouse", true,
                    &ImGuiWindowFlags_NoScrollWithMouse);
    QUERY->doc_var(QUERY,
                   "Disable user vertically scrolling with mouse wheel. On "
                   "child window, mouse wheel will be forwarded to the parent "
                   "unless NoScrollbar is also set.");
    static t_CKINT ImGuiWindowFlags_NoCollapse = 32;
    QUERY->add_svar(QUERY, "int", "NoCollapse", true,
                    &ImGuiWindowFlags_NoCollapse);
    QUERY->doc_var(
      QUERY,
      "Disable user collapsing window by double-clicking on it. Also referred "
      "to as Window Menu Button (e.g. within a docking node).");
    static t_CKINT ImGuiWindowFlags_AlwaysAutoResize = 64;
    QUERY->add_svar(QUERY, "int", "AlwaysAutoResize", true,
                    &ImGuiWindowFlags_AlwaysAutoResize);
    QUERY->doc_var(QUERY, "Resize every window to its content every frame");
    static t_CKINT ImGuiWindowFlags_NoBackground = 128;
    QUERY->add_svar(QUERY, "int", "NoBackground", true,
                    &ImGuiWindowFlags_NoBackground);
    QUERY->doc_var(
      QUERY,
      "Disable drawing background color (WindowBg, etc.) and outside border. "
      "Similar as using SetNextWindowBgAlpha(0.0f).");
    static t_CKINT ImGuiWindowFlags_NoSavedSettings = 256;
    QUERY->add_svar(QUERY, "int", "NoSavedSettings", true,
                    &ImGuiWindowFlags_NoSavedSettings);
    QUERY->doc_var(QUERY, "Never load/save settings in .ini file");
    static t_CKINT ImGuiWindowFlags_NoMouseInputs = 512;
    QUERY->add_svar(QUERY, "int", "NoMouseInputs", true,
                    &ImGuiWindowFlags_NoMouseInputs);
    QUERY->doc_var(QUERY,
                   "Disable catching mouse, hovering test with pass through.");
    static t_CKINT ImGuiWindowFlags_MenuBar = 1024;
    QUERY->add_svar(QUERY, "int", "MenuBar", true, &ImGuiWindowFlags_MenuBar);
    QUERY->doc_var(QUERY, "Has a menu-bar");
    static t_CKINT ImGuiWindowFlags_HorizontalScrollbar = 2048;
    QUERY->add_svar(QUERY, "int", "HorizontalScrollbar", true,
                    &ImGuiWindowFlags_HorizontalScrollbar);
    QUERY->doc_var(QUERY,
                   "Allow horizontal scrollbar to appear (off by default). You "
                   "may use SetNextWindowContentSize(ImVec2(width,0.0f)); "
                   "prior to calling Begin() to specify width. Read code in "
                   "imgui_demo in the \"Horizontal Scrolling\" section.");
    static t_CKINT ImGuiWindowFlags_NoFocusOnAppearing = 4096;
    QUERY->add_svar(QUERY, "int", "NoFocusOnAppearing", true,
                    &ImGuiWindowFlags_NoFocusOnAppearing);
    QUERY->doc_var(
      QUERY,
      "Disable taking focus when transitioning from hidden to visible state");
    static t_CKINT ImGuiWindowFlags_NoBringToFrontOnFocus = 8192;
    QUERY->add_svar(QUERY, "int", "NoBringToFrontOnFocus", true,
                    &ImGuiWindowFlags_NoBringToFrontOnFocus);
    QUERY->doc_var(QUERY,
                   "Disable bringing window to front when taking focus (e.g. "
                   "clicking on it or programmatically giving it focus)");
    static t_CKINT ImGuiWindowFlags_AlwaysVerticalScrollbar = 16384;
    QUERY->add_svar(QUERY, "int", "AlwaysVerticalScrollbar", true,
                    &ImGuiWindowFlags_AlwaysVerticalScrollbar);
    QUERY->doc_var(
      QUERY, "Always show vertical scrollbar (even if ContentSize.y < Size.y)");
    static t_CKINT ImGuiWindowFlags_AlwaysHorizontalScrollbar = 32768;
    QUERY->add_svar(QUERY, "int", "AlwaysHorizontalScrollbar", true,
                    &ImGuiWindowFlags_AlwaysHorizontalScrollbar);
    QUERY->doc_var(
      QUERY,
      "Always show horizontal scrollbar (even if ContentSize.x < Size.x)");
    static t_CKINT ImGuiWindowFlags_NoNavInputs = 65536;
    QUERY->add_svar(QUERY, "int", "NoNavInputs", true,
                    &ImGuiWindowFlags_NoNavInputs);
    QUERY->doc_var(QUERY, "No gamepad/keyboard navigation within the window");
    static t_CKINT ImGuiWindowFlags_NoNavFocus = 131072;
    QUERY->add_svar(QUERY, "int", "NoNavFocus", true,
                    &ImGuiWindowFlags_NoNavFocus);
    QUERY->doc_var(QUERY,
                   "No focusing toward this window with gamepad/keyboard "
                   "navigation (e.g. skipped by CTRL+TAB)");
    static t_CKINT ImGuiWindowFlags_UnsavedDocument = 262144;
    QUERY->add_svar(QUERY, "int", "UnsavedDocument", true,
                    &ImGuiWindowFlags_UnsavedDocument);
    QUERY->doc_var(
      QUERY,
      "Display a dot next to the title. When used in a tab/docking context, "
      "tab is selected when clicking the X + closure is not assumed (will wait "
      "for user to stop submitting the tab). Otherwise closure is assumed when "
      "pressing the X, so if you keep submitting the tab may reappear at end "
      "of tab bar.");
    static t_CKINT ImGuiWindowFlags_NoDocking = 524288;
    QUERY->add_svar(QUERY, "int", "NoDocking", true,
                    &ImGuiWindowFlags_NoDocking);
    QUERY->doc_var(QUERY, "Disable docking of this window");
    static t_CKINT ImGuiWindowFlags_NoNav = 196608;
    QUERY->add_svar(QUERY, "int", "NoNav", true, &ImGuiWindowFlags_NoNav);
    static t_CKINT ImGuiWindowFlags_NoDecoration = 43;
    QUERY->add_svar(QUERY, "int", "NoDecoration", true,
                    &ImGuiWindowFlags_NoDecoration);
    static t_CKINT ImGuiWindowFlags_NoInputs = 197120;
    QUERY->add_svar(QUERY, "int", "NoInputs", true, &ImGuiWindowFlags_NoInputs);
    QUERY->end_class(QUERY);

    QUERY->begin_class(QUERY, "UI_ChildFlags", "Object");
    QUERY->doc_class(
      QUERY,
      "Flags for ImGui::BeginChild().\n(Legacy: bit 0 must always correspond "
      "to ImGuiChildFlags_Border to be backward compatible with old API using "
      "'bool border = false'..\nAbout using AutoResizeX/AutoResizeY flags:.\n- "
      "May be combined with SetNextWindowSizeConstraints() to set a min/max "
      "size for each axis (see \"Demo->Child->Auto-resize with "
      "Constraints\")..\n- Size measurement for a given axis is only performed "
      "when the child window is within visible boundaries, or is just "
      "appearing..\n- This allows BeginChild() to return false when not within "
      "boundaries (e.g. when scrolling), which is more optimal. BUT it won't "
      "update its auto-size while clipped..\nWhile not perfect, it is a better "
      "default behavior as the always-on performance gain is more valuable "
      "than the occasional \"resizing after becoming visible again\" "
      "glitch..\n- You may also use ImGuiChildFlags_AlwaysAutoResize to force "
      "an update even when child window is not in view..\nHOWEVER PLEASE "
      "UNDERSTAND THAT DOING SO WILL PREVENT BeginChild() FROM EVER RETURNING "
      "FALSE, disabling benefits of coarse clipping..\n");
    static t_CKINT ImGuiChildFlags_None = 0;
    QUERY->add_svar(QUERY, "int", "None", true, &ImGuiChildFlags_None);
    static t_CKINT ImGuiChildFlags_Border = 1;
    QUERY->add_svar(QUERY, "int", "Border", true, &ImGuiChildFlags_Border);
    QUERY->doc_var(QUERY,
                   "Show an outer border and enable WindowPadding. (IMPORTANT: "
                   "this is always == 1 == true for legacy reason)");
    static t_CKINT ImGuiChildFlags_AlwaysUseWindowPadding = 2;
    QUERY->add_svar(QUERY, "int", "AlwaysUseWindowPadding", true,
                    &ImGuiChildFlags_AlwaysUseWindowPadding);
    QUERY->doc_var(
      QUERY,
      "Pad with style.WindowPadding even if no border are drawn (no padding by "
      "default for non-bordered child windows because it makes more sense)");
    static t_CKINT ImGuiChildFlags_ResizeX = 4;
    QUERY->add_svar(QUERY, "int", "ResizeX", true, &ImGuiChildFlags_ResizeX);
    QUERY->doc_var(
      QUERY,
      "Allow resize from right border (layout direction). Enable .ini saving "
      "(unless ImGuiWindowFlags_NoSavedSettings passed to window flags)");
    static t_CKINT ImGuiChildFlags_ResizeY = 8;
    QUERY->add_svar(QUERY, "int", "ResizeY", true, &ImGuiChildFlags_ResizeY);
    QUERY->doc_var(QUERY,
                   "Allow resize from bottom border (layout direction). \"");
    static t_CKINT ImGuiChildFlags_AutoResizeX = 16;
    QUERY->add_svar(QUERY, "int", "AutoResizeX", true,
                    &ImGuiChildFlags_AutoResizeX);
    QUERY->doc_var(QUERY,
                   "Enable auto-resizing width. Read \"IMPORTANT: Size "
                   "measurement\" details above.");
    static t_CKINT ImGuiChildFlags_AutoResizeY = 32;
    QUERY->add_svar(QUERY, "int", "AutoResizeY", true,
                    &ImGuiChildFlags_AutoResizeY);
    QUERY->doc_var(QUERY,
                   "Enable auto-resizing height. Read \"IMPORTANT: Size "
                   "measurement\" details above.");
    static t_CKINT ImGuiChildFlags_AlwaysAutoResize = 64;
    QUERY->add_svar(QUERY, "int", "AlwaysAutoResize", true,
                    &ImGuiChildFlags_AlwaysAutoResize);
    QUERY->doc_var(QUERY,
                   "Combined with AutoResizeX/AutoResizeY. Always measure size "
                   "even when child is hidden, always return true, always "
                   "disable clipping optimization! NOT RECOMMENDED.");
    static t_CKINT ImGuiChildFlags_FrameStyle = 128;
    QUERY->add_svar(QUERY, "int", "FrameStyle", true,
                    &ImGuiChildFlags_FrameStyle);
    QUERY->doc_var(QUERY,
                   "Style the child window like a framed item: use FrameBg, "
                   "FrameRounding, FrameBorderSize, FramePadding instead of "
                   "ChildBg, ChildRounding, ChildBorderSize, WindowPadding.");
    QUERY->end_class(QUERY);

    QUERY->begin_class(QUERY, "UI_InputTextFlags", "Object");
    QUERY->doc_class(
      QUERY,
      "Flags for ImGui::InputText().\n(Those are per-item flags. There are "
      "shared flags in ImGuiIO: io.ConfigInputTextCursorBlink and "
      "io.ConfigInputTextEnterKeepActive).\n");
    static t_CKINT ImGuiInputTextFlags_None = 0;
    QUERY->add_svar(QUERY, "int", "None", true, &ImGuiInputTextFlags_None);
    static t_CKINT ImGuiInputTextFlags_CharsDecimal = 1;
    QUERY->add_svar(QUERY, "int", "CharsDecimal", true,
                    &ImGuiInputTextFlags_CharsDecimal);
    QUERY->doc_var(QUERY, "Allow 0123456789.+-*/");
    static t_CKINT ImGuiInputTextFlags_CharsHexadecimal = 2;
    QUERY->add_svar(QUERY, "int", "CharsHexadecimal", true,
                    &ImGuiInputTextFlags_CharsHexadecimal);
    QUERY->doc_var(QUERY, "Allow 0123456789ABCDEFabcdef");
    static t_CKINT ImGuiInputTextFlags_CharsUppercase = 4;
    QUERY->add_svar(QUERY, "int", "CharsUppercase", true,
                    &ImGuiInputTextFlags_CharsUppercase);
    QUERY->doc_var(QUERY, "Turn a..z into A..Z");
    static t_CKINT ImGuiInputTextFlags_CharsNoBlank = 8;
    QUERY->add_svar(QUERY, "int", "CharsNoBlank", true,
                    &ImGuiInputTextFlags_CharsNoBlank);
    QUERY->doc_var(QUERY, "Filter out spaces, tabs");
    static t_CKINT ImGuiInputTextFlags_AutoSelectAll = 16;
    QUERY->add_svar(QUERY, "int", "AutoSelectAll", true,
                    &ImGuiInputTextFlags_AutoSelectAll);
    QUERY->doc_var(QUERY, "Select entire text when first taking mouse focus");
    static t_CKINT ImGuiInputTextFlags_EnterReturnsTrue = 32;
    QUERY->add_svar(QUERY, "int", "EnterReturnsTrue", true,
                    &ImGuiInputTextFlags_EnterReturnsTrue);
    QUERY->doc_var(QUERY,
                   "Return 'true' when Enter is pressed (as opposed to every "
                   "time the value was modified). Consider looking at the "
                   "IsItemDeactivatedAfterEdit() function.");
    static t_CKINT ImGuiInputTextFlags_CallbackCompletion = 64;
    QUERY->add_svar(QUERY, "int", "CallbackCompletion", true,
                    &ImGuiInputTextFlags_CallbackCompletion);
    QUERY->doc_var(QUERY, "Callback on pressing TAB (for completion handling)");
    static t_CKINT ImGuiInputTextFlags_CallbackHistory = 128;
    QUERY->add_svar(QUERY, "int", "CallbackHistory", true,
                    &ImGuiInputTextFlags_CallbackHistory);
    QUERY->doc_var(
      QUERY, "Callback on pressing Up/Down arrows (for history handling)");
    static t_CKINT ImGuiInputTextFlags_CallbackAlways = 256;
    QUERY->add_svar(QUERY, "int", "CallbackAlways", true,
                    &ImGuiInputTextFlags_CallbackAlways);
    QUERY->doc_var(QUERY,
                   "Callback on each iteration. User code may query cursor "
                   "position, modify text buffer.");
    static t_CKINT ImGuiInputTextFlags_CallbackCharFilter = 512;
    QUERY->add_svar(QUERY, "int", "CallbackCharFilter", true,
                    &ImGuiInputTextFlags_CallbackCharFilter);
    QUERY->doc_var(
      QUERY,
      "Callback on character inputs to replace or discard them. Modify "
      "'EventChar' to replace or discard, or return 1 in callback to discard.");
    static t_CKINT ImGuiInputTextFlags_AllowTabInput = 1024;
    QUERY->add_svar(QUERY, "int", "AllowTabInput", true,
                    &ImGuiInputTextFlags_AllowTabInput);
    QUERY->doc_var(QUERY,
                   "Pressing TAB input a '\\t' character into the text field");
    static t_CKINT ImGuiInputTextFlags_CtrlEnterForNewLine = 2048;
    QUERY->add_svar(QUERY, "int", "CtrlEnterForNewLine", true,
                    &ImGuiInputTextFlags_CtrlEnterForNewLine);
    QUERY->doc_var(
      QUERY,
      "In multi-line mode, unfocus with Enter, add new line with Ctrl+Enter "
      "(default is opposite: unfocus with Ctrl+Enter, add line with Enter).");
    static t_CKINT ImGuiInputTextFlags_NoHorizontalScroll = 4096;
    QUERY->add_svar(QUERY, "int", "NoHorizontalScroll", true,
                    &ImGuiInputTextFlags_NoHorizontalScroll);
    QUERY->doc_var(QUERY, "Disable following the cursor horizontally");
    static t_CKINT ImGuiInputTextFlags_AlwaysOverwrite = 8192;
    QUERY->add_svar(QUERY, "int", "AlwaysOverwrite", true,
                    &ImGuiInputTextFlags_AlwaysOverwrite);
    QUERY->doc_var(QUERY, "Overwrite mode");
    static t_CKINT ImGuiInputTextFlags_ReadOnly = 16384;
    QUERY->add_svar(QUERY, "int", "ReadOnly", true,
                    &ImGuiInputTextFlags_ReadOnly);
    QUERY->doc_var(QUERY, "Read-only mode");
    static t_CKINT ImGuiInputTextFlags_Password = 32768;
    QUERY->add_svar(QUERY, "int", "Password", true,
                    &ImGuiInputTextFlags_Password);
    QUERY->doc_var(QUERY, "Password mode, display all characters as '*'");
    static t_CKINT ImGuiInputTextFlags_NoUndoRedo = 65536;
    QUERY->add_svar(QUERY, "int", "NoUndoRedo", true,
                    &ImGuiInputTextFlags_NoUndoRedo);
    QUERY->doc_var(QUERY,
                   "Disable undo/redo. Note that input text owns the text data "
                   "while active, if you want to provide your own undo/redo "
                   "stack you need e.g. to call ClearActiveID().");
    static t_CKINT ImGuiInputTextFlags_CharsScientific = 131072;
    QUERY->add_svar(QUERY, "int", "CharsScientific", true,
                    &ImGuiInputTextFlags_CharsScientific);
    QUERY->doc_var(QUERY,
                   "Allow 0123456789.+-*/eE (Scientific notation input)");
    static t_CKINT ImGuiInputTextFlags_CallbackResize = 262144;
    QUERY->add_svar(QUERY, "int", "CallbackResize", true,
                    &ImGuiInputTextFlags_CallbackResize);
    QUERY->doc_var(
      QUERY,
      "Callback on buffer capacity changes request (beyond 'buf_size' "
      "parameter value), allowing the string to grow. Notify when the string "
      "wants to be resized (for string types which hold a cache of their "
      "Size). You will be provided a new BufSize in the callback and NEED to "
      "honor it. (see misc/cpp/imgui_stdlib.h for an example of using this)");
    static t_CKINT ImGuiInputTextFlags_CallbackEdit = 524288;
    QUERY->add_svar(QUERY, "int", "CallbackEdit", true,
                    &ImGuiInputTextFlags_CallbackEdit);
    QUERY->doc_var(QUERY,
                   "Callback on any edit (note that InputText() already "
                   "returns true on edit, the callback is useful mainly to "
                   "manipulate the underlying buffer while focus is active)");
    static t_CKINT ImGuiInputTextFlags_EscapeClearsAll = 1048576;
    QUERY->add_svar(QUERY, "int", "EscapeClearsAll", true,
                    &ImGuiInputTextFlags_EscapeClearsAll);
    QUERY->doc_var(
      QUERY,
      "Escape key clears content if not empty, and deactivate otherwise "
      "(contrast to default behavior of Escape to revert)");
    QUERY->end_class(QUERY);

    QUERY->begin_class(QUERY, "UI_TreeNodeFlags", "Object");
    QUERY->doc_class(
      QUERY, "Flags for ImGui::TreeNodeEx(), ImGui::CollapsingHeader*().\n");
    static t_CKINT ImGuiTreeNodeFlags_None = 0;
    QUERY->add_svar(QUERY, "int", "None", true, &ImGuiTreeNodeFlags_None);
    static t_CKINT ImGuiTreeNodeFlags_Selected = 1;
    QUERY->add_svar(QUERY, "int", "Selected", true,
                    &ImGuiTreeNodeFlags_Selected);
    QUERY->doc_var(QUERY, "Draw as selected");
    static t_CKINT ImGuiTreeNodeFlags_Framed = 2;
    QUERY->add_svar(QUERY, "int", "Framed", true, &ImGuiTreeNodeFlags_Framed);
    QUERY->doc_var(QUERY,
                   "Draw frame with background (e.g. for CollapsingHeader)");
    static t_CKINT ImGuiTreeNodeFlags_AllowOverlap = 4;
    QUERY->add_svar(QUERY, "int", "AllowOverlap", true,
                    &ImGuiTreeNodeFlags_AllowOverlap);
    QUERY->doc_var(
      QUERY, "Hit testing to allow subsequent widgets to overlap this one");
    static t_CKINT ImGuiTreeNodeFlags_NoTreePushOnOpen = 8;
    QUERY->add_svar(QUERY, "int", "NoTreePushOnOpen", true,
                    &ImGuiTreeNodeFlags_NoTreePushOnOpen);
    QUERY->doc_var(
      QUERY,
      "Don't do a TreePush() when open (e.g. for CollapsingHeader) = no extra "
      "indent nor pushing on ID stack");
    static t_CKINT ImGuiTreeNodeFlags_NoAutoOpenOnLog = 16;
    QUERY->add_svar(QUERY, "int", "NoAutoOpenOnLog", true,
                    &ImGuiTreeNodeFlags_NoAutoOpenOnLog);
    QUERY->doc_var(
      QUERY,
      "Don't automatically and temporarily open node when Logging is active "
      "(by default logging will automatically open tree nodes)");
    static t_CKINT ImGuiTreeNodeFlags_DefaultOpen = 32;
    QUERY->add_svar(QUERY, "int", "DefaultOpen", true,
                    &ImGuiTreeNodeFlags_DefaultOpen);
    QUERY->doc_var(QUERY, "Default node to be open");
    static t_CKINT ImGuiTreeNodeFlags_OpenOnDoubleClick = 64;
    QUERY->add_svar(QUERY, "int", "OpenOnDoubleClick", true,
                    &ImGuiTreeNodeFlags_OpenOnDoubleClick);
    QUERY->doc_var(QUERY, "Need double-click to open node");
    static t_CKINT ImGuiTreeNodeFlags_OpenOnArrow = 128;
    QUERY->add_svar(QUERY, "int", "OpenOnArrow", true,
                    &ImGuiTreeNodeFlags_OpenOnArrow);
    QUERY->doc_var(QUERY,
                   "Only open when clicking on the arrow part. If "
                   "ImGuiTreeNodeFlags_OpenOnDoubleClick is also set, "
                   "single-click arrow or double-click all box to open.");
    static t_CKINT ImGuiTreeNodeFlags_Leaf = 256;
    QUERY->add_svar(QUERY, "int", "Leaf", true, &ImGuiTreeNodeFlags_Leaf);
    QUERY->doc_var(
      QUERY, "No collapsing, no arrow (use as a convenience for leaf nodes).");
    static t_CKINT ImGuiTreeNodeFlags_Bullet = 512;
    QUERY->add_svar(QUERY, "int", "Bullet", true, &ImGuiTreeNodeFlags_Bullet);
    QUERY->doc_var(
      QUERY,
      "Display a bullet instead of arrow. IMPORTANT: node can still be marked "
      "open/close if you don't set the _Leaf flag!");
    static t_CKINT ImGuiTreeNodeFlags_FramePadding = 1024;
    QUERY->add_svar(QUERY, "int", "FramePadding", true,
                    &ImGuiTreeNodeFlags_FramePadding);
    QUERY->doc_var(QUERY,
                   "Use FramePadding (even for an unframed text node) to "
                   "vertically align text baseline to regular widget height. "
                   "Equivalent to calling AlignTextToFramePadding().");
    static t_CKINT ImGuiTreeNodeFlags_SpanAvailWidth = 2048;
    QUERY->add_svar(QUERY, "int", "SpanAvailWidth", true,
                    &ImGuiTreeNodeFlags_SpanAvailWidth);
    QUERY->doc_var(QUERY,
                   "Extend hit box to the right-most edge, even if not framed. "
                   "This is not the default in order to allow adding other "
                   "items on the same line without using AllowOverlap mode.");
    static t_CKINT ImGuiTreeNodeFlags_SpanFullWidth = 4096;
    QUERY->add_svar(QUERY, "int", "SpanFullWidth", true,
                    &ImGuiTreeNodeFlags_SpanFullWidth);
    QUERY->doc_var(QUERY,
                   "Extend hit box to the left-most and right-most edges "
                   "(cover the indent area).");
    static t_CKINT ImGuiTreeNodeFlags_SpanTextWidth = 8192;
    QUERY->add_svar(QUERY, "int", "SpanTextWidth", true,
                    &ImGuiTreeNodeFlags_SpanTextWidth);
    QUERY->doc_var(QUERY,
                   "Narrow hit box + narrow hovering highlight, will only "
                   "cover the label text.");
    static t_CKINT ImGuiTreeNodeFlags_SpanAllColumns = 16384;
    QUERY->add_svar(QUERY, "int", "SpanAllColumns", true,
                    &ImGuiTreeNodeFlags_SpanAllColumns);
    QUERY->doc_var(QUERY,
                   "Frame will span all columns of its container table (text "
                   "will still fit in current column)");
    static t_CKINT ImGuiTreeNodeFlags_NavLeftJumpsBackHere = 32768;
    QUERY->add_svar(QUERY, "int", "NavLeftJumpsBackHere", true,
                    &ImGuiTreeNodeFlags_NavLeftJumpsBackHere);
    QUERY->doc_var(
      QUERY,
      "(WIP) Nav: left direction may move to this TreeNode() from any of its "
      "child (items submitted between TreeNode and TreePop)");
    static t_CKINT ImGuiTreeNodeFlags_CollapsingHeader = 26;
    QUERY->add_svar(QUERY, "int", "CollapsingHeader", true,
                    &ImGuiTreeNodeFlags_CollapsingHeader);
    QUERY->end_class(QUERY);

    QUERY->begin_class(QUERY, "UI_PopupFlags", "Object");
    QUERY->doc_class(
      QUERY,
      "Flags for OpenPopup*(), BeginPopupContext*(), IsPopupOpen() "
      "functions..\n- To be backward compatible with older API which took an "
      "'int mouse_button = 1' argument instead of 'ImGuiPopupFlags "
      "flags',.\nwe need to treat small flags values as a mouse button index, "
      "so we encode the mouse button in the first few bits of the flags..\nIt "
      "is therefore guaranteed to be legal to pass a mouse button index in "
      "ImGuiPopupFlags..\n- For the same reason, we exceptionally default the "
      "ImGuiPopupFlags argument of BeginPopupContextXXX functions to 1 instead "
      "of 0..\nIMPORTANT: because the default parameter is 1 "
      "(==ImGuiPopupFlags_MouseButtonRight), if you rely on the default "
      "parameter.\nand want to use another flag, you need to pass in the "
      "ImGuiPopupFlags_MouseButtonRight flag explicitly..\n- Multiple buttons "
      "currently cannot be combined/or-ed in those functions (we could allow "
      "it later)..\n");
    static t_CKINT ImGuiPopupFlags_None = 0;
    QUERY->add_svar(QUERY, "int", "None", true, &ImGuiPopupFlags_None);
    static t_CKINT ImGuiPopupFlags_MouseButtonLeft = 0;
    QUERY->add_svar(QUERY, "int", "MouseButtonLeft", true,
                    &ImGuiPopupFlags_MouseButtonLeft);
    QUERY->doc_var(
      QUERY,
      "For BeginPopupContext*(): open on Left Mouse release. Guaranteed to "
      "always be == 0 (same as ImGuiMouseButton_Left)");
    static t_CKINT ImGuiPopupFlags_MouseButtonRight = 1;
    QUERY->add_svar(QUERY, "int", "MouseButtonRight", true,
                    &ImGuiPopupFlags_MouseButtonRight);
    QUERY->doc_var(
      QUERY,
      "For BeginPopupContext*(): open on Right Mouse release. Guaranteed to "
      "always be == 1 (same as ImGuiMouseButton_Right)");
    static t_CKINT ImGuiPopupFlags_MouseButtonMiddle = 2;
    QUERY->add_svar(QUERY, "int", "MouseButtonMiddle", true,
                    &ImGuiPopupFlags_MouseButtonMiddle);
    QUERY->doc_var(
      QUERY,
      "For BeginPopupContext*(): open on Middle Mouse release. Guaranteed to "
      "always be == 2 (same as ImGuiMouseButton_Middle)");
    static t_CKINT ImGuiPopupFlags_NoReopen = 32;
    QUERY->add_svar(QUERY, "int", "NoReopen", true, &ImGuiPopupFlags_NoReopen);
    QUERY->doc_var(
      QUERY,
      "For OpenPopup*(), BeginPopupContext*(): don't reopen same popup if "
      "already open (won't reposition, won't reinitialize navigation)");
    static t_CKINT ImGuiPopupFlags_NoOpenOverExistingPopup = 128;
    QUERY->add_svar(QUERY, "int", "NoOpenOverExistingPopup", true,
                    &ImGuiPopupFlags_NoOpenOverExistingPopup);
    QUERY->doc_var(
      QUERY,
      "For OpenPopup*(), BeginPopupContext*(): don't open if there's already a "
      "popup at the same level of the popup stack");
    static t_CKINT ImGuiPopupFlags_NoOpenOverItems = 256;
    QUERY->add_svar(QUERY, "int", "NoOpenOverItems", true,
                    &ImGuiPopupFlags_NoOpenOverItems);
    QUERY->doc_var(QUERY,
                   "For BeginPopupContextWindow(): don't return true when "
                   "hovering items, only when hovering empty space");
    static t_CKINT ImGuiPopupFlags_AnyPopupId = 1024;
    QUERY->add_svar(QUERY, "int", "AnyPopupId", true,
                    &ImGuiPopupFlags_AnyPopupId);
    QUERY->doc_var(QUERY,
                   "For IsPopupOpen(): ignore the ImGuiID parameter and test "
                   "for any popup.");
    static t_CKINT ImGuiPopupFlags_AnyPopupLevel = 2048;
    QUERY->add_svar(QUERY, "int", "AnyPopupLevel", true,
                    &ImGuiPopupFlags_AnyPopupLevel);
    QUERY->doc_var(QUERY,
                   "For IsPopupOpen(): search/test at any level of the popup "
                   "stack (default test in the current level)");
    static t_CKINT ImGuiPopupFlags_AnyPopup = 3072;
    QUERY->add_svar(QUERY, "int", "AnyPopup", true, &ImGuiPopupFlags_AnyPopup);
    QUERY->end_class(QUERY);

    QUERY->begin_class(QUERY, "UI_SelectableFlags", "Object");
    QUERY->doc_class(QUERY, "Flags for ImGui::Selectable().\n");
    static t_CKINT ImGuiSelectableFlags_None = 0;
    QUERY->add_svar(QUERY, "int", "None", true, &ImGuiSelectableFlags_None);
    static t_CKINT ImGuiSelectableFlags_DontClosePopups = 1;
    QUERY->add_svar(QUERY, "int", "DontClosePopups", true,
                    &ImGuiSelectableFlags_DontClosePopups);
    QUERY->doc_var(QUERY, "Clicking this doesn't close parent popup window");
    static t_CKINT ImGuiSelectableFlags_SpanAllColumns = 2;
    QUERY->add_svar(QUERY, "int", "SpanAllColumns", true,
                    &ImGuiSelectableFlags_SpanAllColumns);
    QUERY->doc_var(QUERY,
                   "Frame will span all columns of its container table (text "
                   "will still fit in current column)");
    static t_CKINT ImGuiSelectableFlags_AllowDoubleClick = 4;
    QUERY->add_svar(QUERY, "int", "AllowDoubleClick", true,
                    &ImGuiSelectableFlags_AllowDoubleClick);
    QUERY->doc_var(QUERY, "Generate press events on double clicks too");
    static t_CKINT ImGuiSelectableFlags_Disabled = 8;
    QUERY->add_svar(QUERY, "int", "Disabled", true,
                    &ImGuiSelectableFlags_Disabled);
    QUERY->doc_var(QUERY, "Cannot be selected, display grayed out text");
    static t_CKINT ImGuiSelectableFlags_AllowOverlap = 16;
    QUERY->add_svar(QUERY, "int", "AllowOverlap", true,
                    &ImGuiSelectableFlags_AllowOverlap);
    QUERY->doc_var(
      QUERY,
      "(WIP) Hit testing to allow subsequent widgets to overlap this one");
    QUERY->end_class(QUERY);

    QUERY->begin_class(QUERY, "UI_ComboFlags", "Object");
    QUERY->doc_class(QUERY, "Flags for ImGui::BeginCombo().\n");
    static t_CKINT ImGuiComboFlags_None = 0;
    QUERY->add_svar(QUERY, "int", "None", true, &ImGuiComboFlags_None);
    static t_CKINT ImGuiComboFlags_PopupAlignLeft = 1;
    QUERY->add_svar(QUERY, "int", "PopupAlignLeft", true,
                    &ImGuiComboFlags_PopupAlignLeft);
    QUERY->doc_var(QUERY, "Align the popup toward the left by default");
    static t_CKINT ImGuiComboFlags_HeightSmall = 2;
    QUERY->add_svar(QUERY, "int", "HeightSmall", true,
                    &ImGuiComboFlags_HeightSmall);
    QUERY->doc_var(
      QUERY,
      "Max ~4 items visible. Tip: If you want your combo popup to be a "
      "specific size you can use SetNextWindowSizeConstraints() prior to "
      "calling BeginCombo()");
    static t_CKINT ImGuiComboFlags_HeightRegular = 4;
    QUERY->add_svar(QUERY, "int", "HeightRegular", true,
                    &ImGuiComboFlags_HeightRegular);
    QUERY->doc_var(QUERY, "Max ~8 items visible (default)");
    static t_CKINT ImGuiComboFlags_HeightLarge = 8;
    QUERY->add_svar(QUERY, "int", "HeightLarge", true,
                    &ImGuiComboFlags_HeightLarge);
    QUERY->doc_var(QUERY, "Max ~20 items visible");
    static t_CKINT ImGuiComboFlags_HeightLargest = 16;
    QUERY->add_svar(QUERY, "int", "HeightLargest", true,
                    &ImGuiComboFlags_HeightLargest);
    QUERY->doc_var(QUERY, "As many fitting items as possible");
    static t_CKINT ImGuiComboFlags_NoArrowButton = 32;
    QUERY->add_svar(QUERY, "int", "NoArrowButton", true,
                    &ImGuiComboFlags_NoArrowButton);
    QUERY->doc_var(
      QUERY, "Display on the preview box without the square arrow button");
    static t_CKINT ImGuiComboFlags_NoPreview = 64;
    QUERY->add_svar(QUERY, "int", "NoPreview", true,
                    &ImGuiComboFlags_NoPreview);
    QUERY->doc_var(QUERY, "Display only a square arrow button");
    static t_CKINT ImGuiComboFlags_WidthFitPreview = 128;
    QUERY->add_svar(QUERY, "int", "WidthFitPreview", true,
                    &ImGuiComboFlags_WidthFitPreview);
    QUERY->doc_var(QUERY, "Width dynamically calculated from preview contents");
    QUERY->end_class(QUERY);

    QUERY->begin_class(QUERY, "UI_TabBarFlags", "Object");
    QUERY->doc_class(QUERY, "Flags for ImGui::BeginTabBar().\n");
    static t_CKINT ImGuiTabBarFlags_None = 0;
    QUERY->add_svar(QUERY, "int", "None", true, &ImGuiTabBarFlags_None);
    static t_CKINT ImGuiTabBarFlags_Reorderable = 1;
    QUERY->add_svar(QUERY, "int", "Reorderable", true,
                    &ImGuiTabBarFlags_Reorderable);
    QUERY->doc_var(QUERY,
                   "Allow manually dragging tabs to re-order them + New tabs "
                   "are appended at the end of list");
    static t_CKINT ImGuiTabBarFlags_AutoSelectNewTabs = 2;
    QUERY->add_svar(QUERY, "int", "AutoSelectNewTabs", true,
                    &ImGuiTabBarFlags_AutoSelectNewTabs);
    QUERY->doc_var(QUERY, "Automatically select new tabs when they appear");
    static t_CKINT ImGuiTabBarFlags_TabListPopupButton = 4;
    QUERY->add_svar(QUERY, "int", "TabListPopupButton", true,
                    &ImGuiTabBarFlags_TabListPopupButton);
    QUERY->doc_var(QUERY, "Disable buttons to open the tab list popup");
    static t_CKINT ImGuiTabBarFlags_NoCloseWithMiddleMouseButton = 8;
    QUERY->add_svar(QUERY, "int", "NoCloseWithMiddleMouseButton", true,
                    &ImGuiTabBarFlags_NoCloseWithMiddleMouseButton);
    QUERY->doc_var(QUERY,
                   "Disable behavior of closing tabs (that are submitted with "
                   "p_open != NULL) with middle mouse button. You may handle "
                   "this behavior manually on user's side with if "
                   "(IsItemHovered() && IsMouseClicked(2)) *p_open = false.");
    static t_CKINT ImGuiTabBarFlags_NoTabListScrollingButtons = 16;
    QUERY->add_svar(QUERY, "int", "NoTabListScrollingButtons", true,
                    &ImGuiTabBarFlags_NoTabListScrollingButtons);
    QUERY->doc_var(QUERY,
                   "Disable scrolling buttons (apply when fitting policy is "
                   "ImGuiTabBarFlags_FittingPolicyScroll)");
    static t_CKINT ImGuiTabBarFlags_NoTooltip = 32;
    QUERY->add_svar(QUERY, "int", "NoTooltip", true,
                    &ImGuiTabBarFlags_NoTooltip);
    QUERY->doc_var(QUERY, "Disable tooltips when hovering a tab");
    static t_CKINT ImGuiTabBarFlags_FittingPolicyResizeDown = 64;
    QUERY->add_svar(QUERY, "int", "FittingPolicyResizeDown", true,
                    &ImGuiTabBarFlags_FittingPolicyResizeDown);
    QUERY->doc_var(QUERY, "Resize tabs when they don't fit");
    static t_CKINT ImGuiTabBarFlags_FittingPolicyScroll = 128;
    QUERY->add_svar(QUERY, "int", "FittingPolicyScroll", true,
                    &ImGuiTabBarFlags_FittingPolicyScroll);
    QUERY->doc_var(QUERY, "Add scroll buttons when tabs don't fit");
    QUERY->end_class(QUERY);

    QUERY->begin_class(QUERY, "UI_TabItemFlags", "Object");
    QUERY->doc_class(QUERY, "Flags for ImGui::BeginTabItem().\n");
    static t_CKINT ImGuiTabItemFlags_None = 0;
    QUERY->add_svar(QUERY, "int", "None", true, &ImGuiTabItemFlags_None);
    static t_CKINT ImGuiTabItemFlags_UnsavedDocument = 1;
    QUERY->add_svar(QUERY, "int", "UnsavedDocument", true,
                    &ImGuiTabItemFlags_UnsavedDocument);
    QUERY->doc_var(QUERY,
                   "Display a dot next to the title + set "
                   "ImGuiTabItemFlags_NoAssumedClosure.");
    static t_CKINT ImGuiTabItemFlags_SetSelected = 2;
    QUERY->add_svar(QUERY, "int", "SetSelected", true,
                    &ImGuiTabItemFlags_SetSelected);
    QUERY->doc_var(QUERY,
                   "Trigger flag to programmatically make the tab selected "
                   "when calling BeginTabItem()");
    static t_CKINT ImGuiTabItemFlags_NoCloseWithMiddleMouseButton = 4;
    QUERY->add_svar(QUERY, "int", "NoCloseWithMiddleMouseButton", true,
                    &ImGuiTabItemFlags_NoCloseWithMiddleMouseButton);
    QUERY->doc_var(QUERY,
                   "Disable behavior of closing tabs (that are submitted with "
                   "p_open != NULL) with middle mouse button. You may handle "
                   "this behavior manually on user's side with if "
                   "(IsItemHovered() && IsMouseClicked(2)) *p_open = false.");
    static t_CKINT ImGuiTabItemFlags_NoPushId = 8;
    QUERY->add_svar(QUERY, "int", "NoPushId", true,
                    &ImGuiTabItemFlags_NoPushId);
    QUERY->doc_var(
      QUERY, "Don't call PushID()/PopID() on BeginTabItem()/EndTabItem()");
    static t_CKINT ImGuiTabItemFlags_NoTooltip = 16;
    QUERY->add_svar(QUERY, "int", "NoTooltip", true,
                    &ImGuiTabItemFlags_NoTooltip);
    QUERY->doc_var(QUERY, "Disable tooltip for the given tab");
    static t_CKINT ImGuiTabItemFlags_NoReorder = 32;
    QUERY->add_svar(QUERY, "int", "NoReorder", true,
                    &ImGuiTabItemFlags_NoReorder);
    QUERY->doc_var(
      QUERY,
      "Disable reordering this tab or having another tab cross over this tab");
    static t_CKINT ImGuiTabItemFlags_Leading = 64;
    QUERY->add_svar(QUERY, "int", "Leading", true, &ImGuiTabItemFlags_Leading);
    QUERY->doc_var(QUERY,
                   "Enforce the tab position to the left of the tab bar (after "
                   "the tab list popup button)");
    static t_CKINT ImGuiTabItemFlags_Trailing = 128;
    QUERY->add_svar(QUERY, "int", "Trailing", true,
                    &ImGuiTabItemFlags_Trailing);
    QUERY->doc_var(QUERY,
                   "Enforce the tab position to the right of the tab bar "
                   "(before the scrolling buttons)");
    static t_CKINT ImGuiTabItemFlags_NoAssumedClosure = 256;
    QUERY->add_svar(QUERY, "int", "NoAssumedClosure", true,
                    &ImGuiTabItemFlags_NoAssumedClosure);
    QUERY->doc_var(
      QUERY,
      "Tab is selected when trying to close + closure is not immediately "
      "assumed (will wait for user to stop submitting the tab). Otherwise "
      "closure is assumed when pressing the X, so if you keep submitting the "
      "tab may reappear at end of tab bar.");
    QUERY->end_class(QUERY);

    QUERY->begin_class(QUERY, "UI_FocusedFlags", "Object");
    QUERY->doc_class(QUERY, "Flags for ImGui::IsWindowFocused().\n");
    static t_CKINT ImGuiFocusedFlags_None = 0;
    QUERY->add_svar(QUERY, "int", "None", true, &ImGuiFocusedFlags_None);
    static t_CKINT ImGuiFocusedFlags_ChildWindows = 1;
    QUERY->add_svar(QUERY, "int", "ChildWindows", true,
                    &ImGuiFocusedFlags_ChildWindows);
    QUERY->doc_var(QUERY,
                   "Return true if any children of the window is focused");
    static t_CKINT ImGuiFocusedFlags_RootWindow = 2;
    QUERY->add_svar(QUERY, "int", "RootWindow", true,
                    &ImGuiFocusedFlags_RootWindow);
    QUERY->doc_var(
      QUERY,
      "Test from root window (top most parent of the current hierarchy)");
    static t_CKINT ImGuiFocusedFlags_AnyWindow = 4;
    QUERY->add_svar(QUERY, "int", "AnyWindow", true,
                    &ImGuiFocusedFlags_AnyWindow);
    QUERY->doc_var(
      QUERY,
      "Return true if any window is focused. Important: If you are trying to "
      "tell how to dispatch your low-level inputs, do NOT use this. Use "
      "'io.WantCaptureMouse' instead! Please read the FAQ!");
    static t_CKINT ImGuiFocusedFlags_NoPopupHierarchy = 8;
    QUERY->add_svar(QUERY, "int", "NoPopupHierarchy", true,
                    &ImGuiFocusedFlags_NoPopupHierarchy);
    QUERY->doc_var(
      QUERY,
      "Do not consider popup hierarchy (do not treat popup emitter as parent "
      "of popup) (when used with _ChildWindows or _RootWindow)");
    static t_CKINT ImGuiFocusedFlags_DockHierarchy = 16;
    QUERY->add_svar(QUERY, "int", "DockHierarchy", true,
                    &ImGuiFocusedFlags_DockHierarchy);
    QUERY->doc_var(
      QUERY,
      "Consider docking hierarchy (treat dockspace host as parent of docked "
      "window) (when used with _ChildWindows or _RootWindow)");
    static t_CKINT ImGuiFocusedFlags_RootAndChildWindows = 3;
    QUERY->add_svar(QUERY, "int", "RootAndChildWindows", true,
                    &ImGuiFocusedFlags_RootAndChildWindows);
    QUERY->end_class(QUERY);

    QUERY->begin_class(QUERY, "UI_HoveredFlags", "Object");
    QUERY->doc_class(
      QUERY,
      "Flags for ImGui::IsItemHovered(), ImGui::IsWindowHovered().\nNote: if "
      "you are trying to check whether your mouse should be dispatched to Dear "
      "ImGui or to your app, you should use 'io.WantCaptureMouse' instead! "
      "Please read the FAQ!.\nNote: windows with the ImGuiWindowFlags_NoInputs "
      "flag are ignored by IsWindowHovered() calls..\n");
    static t_CKINT ImGuiHoveredFlags_None = 0;
    QUERY->add_svar(QUERY, "int", "None", true, &ImGuiHoveredFlags_None);
    QUERY->doc_var(QUERY,
                   "Return true if directly over the item/window, not "
                   "obstructed by another window, not obstructed by an active "
                   "popup or modal blocking inputs under them.");
    static t_CKINT ImGuiHoveredFlags_ChildWindows = 1;
    QUERY->add_svar(QUERY, "int", "ChildWindows", true,
                    &ImGuiHoveredFlags_ChildWindows);
    QUERY->doc_var(QUERY,
                   "IsWindowHovered() only: Return true if any children of the "
                   "window is hovered");
    static t_CKINT ImGuiHoveredFlags_RootWindow = 2;
    QUERY->add_svar(QUERY, "int", "RootWindow", true,
                    &ImGuiHoveredFlags_RootWindow);
    QUERY->doc_var(QUERY,
                   "IsWindowHovered() only: Test from root window (top most "
                   "parent of the current hierarchy)");
    static t_CKINT ImGuiHoveredFlags_AnyWindow = 4;
    QUERY->add_svar(QUERY, "int", "AnyWindow", true,
                    &ImGuiHoveredFlags_AnyWindow);
    QUERY->doc_var(
      QUERY, "IsWindowHovered() only: Return true if any window is hovered");
    static t_CKINT ImGuiHoveredFlags_NoPopupHierarchy = 8;
    QUERY->add_svar(QUERY, "int", "NoPopupHierarchy", true,
                    &ImGuiHoveredFlags_NoPopupHierarchy);
    QUERY->doc_var(QUERY,
                   "IsWindowHovered() only: Do not consider popup hierarchy "
                   "(do not treat popup emitter as parent of popup) (when used "
                   "with _ChildWindows or _RootWindow)");
    static t_CKINT ImGuiHoveredFlags_DockHierarchy = 16;
    QUERY->add_svar(QUERY, "int", "DockHierarchy", true,
                    &ImGuiHoveredFlags_DockHierarchy);
    QUERY->doc_var(QUERY,
                   "IsWindowHovered() only: Consider docking hierarchy (treat "
                   "dockspace host as parent of docked window) (when used with "
                   "_ChildWindows or _RootWindow)");
    static t_CKINT ImGuiHoveredFlags_AllowWhenBlockedByPopup = 32;
    QUERY->add_svar(QUERY, "int", "AllowWhenBlockedByPopup", true,
                    &ImGuiHoveredFlags_AllowWhenBlockedByPopup);
    QUERY->doc_var(QUERY,
                   "Return true even if a popup window is normally blocking "
                   "access to this item/window");
    static t_CKINT ImGuiHoveredFlags_AllowWhenBlockedByActiveItem = 128;
    QUERY->add_svar(QUERY, "int", "AllowWhenBlockedByActiveItem", true,
                    &ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);
    QUERY->doc_var(QUERY,
                   "Return true even if an active item is blocking access to "
                   "this item/window. Useful for Drag and Drop patterns.");
    static t_CKINT ImGuiHoveredFlags_AllowWhenOverlappedByItem = 256;
    QUERY->add_svar(QUERY, "int", "AllowWhenOverlappedByItem", true,
                    &ImGuiHoveredFlags_AllowWhenOverlappedByItem);
    QUERY->doc_var(
      QUERY,
      "IsItemHovered() only: Return true even if the item uses AllowOverlap "
      "mode and is overlapped by another hoverable item.");
    static t_CKINT ImGuiHoveredFlags_AllowWhenOverlappedByWindow = 512;
    QUERY->add_svar(QUERY, "int", "AllowWhenOverlappedByWindow", true,
                    &ImGuiHoveredFlags_AllowWhenOverlappedByWindow);
    QUERY->doc_var(QUERY,
                   "IsItemHovered() only: Return true even if the position is "
                   "obstructed or overlapped by another window.");
    static t_CKINT ImGuiHoveredFlags_AllowWhenDisabled = 1024;
    QUERY->add_svar(QUERY, "int", "AllowWhenDisabled", true,
                    &ImGuiHoveredFlags_AllowWhenDisabled);
    QUERY->doc_var(
      QUERY, "IsItemHovered() only: Return true even if the item is disabled");
    static t_CKINT ImGuiHoveredFlags_NoNavOverride = 2048;
    QUERY->add_svar(QUERY, "int", "NoNavOverride", true,
                    &ImGuiHoveredFlags_NoNavOverride);
    QUERY->doc_var(QUERY,
                   "IsItemHovered() only: Disable using gamepad/keyboard "
                   "navigation state when active, always query mouse");
    static t_CKINT ImGuiHoveredFlags_AllowWhenOverlapped = 768;
    QUERY->add_svar(QUERY, "int", "AllowWhenOverlapped", true,
                    &ImGuiHoveredFlags_AllowWhenOverlapped);
    static t_CKINT ImGuiHoveredFlags_RectOnly = 928;
    QUERY->add_svar(QUERY, "int", "RectOnly", true,
                    &ImGuiHoveredFlags_RectOnly);
    static t_CKINT ImGuiHoveredFlags_RootAndChildWindows = 3;
    QUERY->add_svar(QUERY, "int", "RootAndChildWindows", true,
                    &ImGuiHoveredFlags_RootAndChildWindows);
    static t_CKINT ImGuiHoveredFlags_ForTooltip = 4096;
    QUERY->add_svar(QUERY, "int", "ForTooltip", true,
                    &ImGuiHoveredFlags_ForTooltip);
    QUERY->doc_var(QUERY,
                   "Shortcut for standard flags when using IsItemHovered() + "
                   "SetTooltip() sequence.");
    static t_CKINT ImGuiHoveredFlags_Stationary = 8192;
    QUERY->add_svar(QUERY, "int", "Stationary", true,
                    &ImGuiHoveredFlags_Stationary);
    QUERY->doc_var(
      QUERY,
      "Require mouse to be stationary for style.HoverStationaryDelay (~0.15 "
      "sec) _at least one time_. After this, can move on same item/window. "
      "Using the stationary test tends to reduces the need for a long delay.");
    static t_CKINT ImGuiHoveredFlags_DelayNone = 16384;
    QUERY->add_svar(QUERY, "int", "DelayNone", true,
                    &ImGuiHoveredFlags_DelayNone);
    QUERY->doc_var(QUERY,
                   "IsItemHovered() only: Return true immediately (default). "
                   "As this is the default you generally ignore this.");
    static t_CKINT ImGuiHoveredFlags_DelayShort = 32768;
    QUERY->add_svar(QUERY, "int", "DelayShort", true,
                    &ImGuiHoveredFlags_DelayShort);
    QUERY->doc_var(
      QUERY,
      "IsItemHovered() only: Return true after style.HoverDelayShort elapsed "
      "(~0.15 sec) (shared between items) + requires mouse to be stationary "
      "for style.HoverStationaryDelay (once per item).");
    static t_CKINT ImGuiHoveredFlags_DelayNormal = 65536;
    QUERY->add_svar(QUERY, "int", "DelayNormal", true,
                    &ImGuiHoveredFlags_DelayNormal);
    QUERY->doc_var(
      QUERY,
      "IsItemHovered() only: Return true after style.HoverDelayNormal elapsed "
      "(~0.40 sec) (shared between items) + requires mouse to be stationary "
      "for style.HoverStationaryDelay (once per item).");
    static t_CKINT ImGuiHoveredFlags_NoSharedDelay = 131072;
    QUERY->add_svar(QUERY, "int", "NoSharedDelay", true,
                    &ImGuiHoveredFlags_NoSharedDelay);
    QUERY->doc_var(QUERY,
                   "IsItemHovered() only: Disable shared delay system where "
                   "moving from one item to the next keeps the previous timer "
                   "for a short time (standard for tooltips with long delays)");
    QUERY->end_class(QUERY);

    QUERY->begin_class(QUERY, "UI_DockNodeFlags", "Object");
    QUERY->doc_class(
      QUERY,
      "Flags for ImGui::DockSpace(), shared/inherited by child nodes..\n(Some "
      "flags can be applied to individual nodes directly).\nFIXME-DOCK: Also "
      "see ImGuiDockNodeFlagsPrivate_ which may involve using the WIP and "
      "internal DockBuilder api..\n");
    static t_CKINT ImGuiDockNodeFlags_None = 0;
    QUERY->add_svar(QUERY, "int", "None", true, &ImGuiDockNodeFlags_None);
    static t_CKINT ImGuiDockNodeFlags_KeepAliveOnly = 1;
    QUERY->add_svar(QUERY, "int", "KeepAliveOnly", true,
                    &ImGuiDockNodeFlags_KeepAliveOnly);
    QUERY->doc_var(
      QUERY,
      "Don't display the dockspace node but keep it alive. Windows docked into "
      "this dockspace node won't be undocked.");
    static t_CKINT ImGuiDockNodeFlags_NoDockingOverCentralNode = 4;
    QUERY->add_svar(QUERY, "int", "NoDockingOverCentralNode", true,
                    &ImGuiDockNodeFlags_NoDockingOverCentralNode);
    QUERY->doc_var(QUERY,
                   "Disable docking over the Central Node, which will be "
                   "always kept empty.");
    static t_CKINT ImGuiDockNodeFlags_PassthruCentralNode = 8;
    QUERY->add_svar(QUERY, "int", "PassthruCentralNode", true,
                    &ImGuiDockNodeFlags_PassthruCentralNode);
    QUERY->doc_var(
      QUERY,
      "Enable passthru dockspace: 1) DockSpace() will render a "
      "ImGuiCol_WindowBg background covering everything excepted the Central "
      "Node when empty. Meaning the host window should probably use "
      "SetNextWindowBgAlpha(0.0f) prior to Begin() when using this. 2) When "
      "Central Node is empty: let inputs pass-through + won't display a "
      "DockingEmptyBg background. See demo for details.");
    static t_CKINT ImGuiDockNodeFlags_NoDockingSplit = 16;
    QUERY->add_svar(QUERY, "int", "NoDockingSplit", true,
                    &ImGuiDockNodeFlags_NoDockingSplit);
    QUERY->doc_var(QUERY,
                   "Disable other windows/nodes from splitting this node.");
    static t_CKINT ImGuiDockNodeFlags_NoResize = 32;
    QUERY->add_svar(QUERY, "int", "NoResize", true,
                    &ImGuiDockNodeFlags_NoResize);
    QUERY->doc_var(
      QUERY,
      "Saved  Disable resizing node using the splitter/separators. Useful with "
      "programmatically setup dockspaces.");
    static t_CKINT ImGuiDockNodeFlags_AutoHideTabBar = 64;
    QUERY->add_svar(QUERY, "int", "AutoHideTabBar", true,
                    &ImGuiDockNodeFlags_AutoHideTabBar);
    QUERY->doc_var(QUERY,
                   "Tab bar will automatically hide when there is a single "
                   "window in the dock node.");
    static t_CKINT ImGuiDockNodeFlags_NoUndocking = 128;
    QUERY->add_svar(QUERY, "int", "NoUndocking", true,
                    &ImGuiDockNodeFlags_NoUndocking);
    QUERY->doc_var(QUERY, "Disable undocking this node.");
    QUERY->end_class(QUERY);

    QUERY->begin_class(QUERY, "UI_DragDropFlags", "Object");
    QUERY->doc_class(QUERY,
                     "Flags for ImGui::BeginDragDropSource(), "
                     "ImGui::AcceptDragDropPayload().\n");
    static t_CKINT ImGuiDragDropFlags_None = 0;
    QUERY->add_svar(QUERY, "int", "None", true, &ImGuiDragDropFlags_None);
    static t_CKINT ImGuiDragDropFlags_SourceNoPreviewTooltip = 1;
    QUERY->add_svar(QUERY, "int", "SourceNoPreviewTooltip", true,
                    &ImGuiDragDropFlags_SourceNoPreviewTooltip);
    QUERY->doc_var(
      QUERY,
      "Disable preview tooltip. By default, a successful call to "
      "BeginDragDropSource opens a tooltip so you can display a preview or "
      "description of the source contents. This flag disables this behavior.");
    static t_CKINT ImGuiDragDropFlags_SourceNoDisableHover = 2;
    QUERY->add_svar(QUERY, "int", "SourceNoDisableHover", true,
                    &ImGuiDragDropFlags_SourceNoDisableHover);
    QUERY->doc_var(
      QUERY,
      "By default, when dragging we clear data so that IsItemHovered() will "
      "return false, to avoid subsequent user code submitting tooltips. This "
      "flag disables this behavior so you can still call IsItemHovered() on "
      "the source item.");
    static t_CKINT ImGuiDragDropFlags_SourceNoHoldToOpenOthers = 4;
    QUERY->add_svar(QUERY, "int", "SourceNoHoldToOpenOthers", true,
                    &ImGuiDragDropFlags_SourceNoHoldToOpenOthers);
    QUERY->doc_var(
      QUERY,
      "Disable the behavior that allows to open tree nodes and collapsing "
      "header by holding over them while dragging a source item.");
    static t_CKINT ImGuiDragDropFlags_SourceAllowNullID = 8;
    QUERY->add_svar(QUERY, "int", "SourceAllowNullID", true,
                    &ImGuiDragDropFlags_SourceAllowNullID);
    QUERY->doc_var(
      QUERY,
      "Allow items such as Text(), Image() that have no unique identifier to "
      "be used as drag source, by manufacturing a temporary identifier based "
      "on their window-relative position. This is extremely unusual within the "
      "dear imgui ecosystem and so we made it explicit.");
    static t_CKINT ImGuiDragDropFlags_SourceExtern = 16;
    QUERY->add_svar(QUERY, "int", "SourceExtern", true,
                    &ImGuiDragDropFlags_SourceExtern);
    QUERY->doc_var(
      QUERY,
      "External source (from outside of dear imgui), won't attempt to read "
      "current item/window info. Will always return true. Only one Extern "
      "source can be active simultaneously.");
    static t_CKINT ImGuiDragDropFlags_SourceAutoExpirePayload = 32;
    QUERY->add_svar(QUERY, "int", "SourceAutoExpirePayload", true,
                    &ImGuiDragDropFlags_SourceAutoExpirePayload);
    QUERY->doc_var(
      QUERY,
      "Automatically expire the payload if the source cease to be submitted "
      "(otherwise payloads are persisting while being dragged)");
    static t_CKINT ImGuiDragDropFlags_AcceptBeforeDelivery = 1024;
    QUERY->add_svar(QUERY, "int", "AcceptBeforeDelivery", true,
                    &ImGuiDragDropFlags_AcceptBeforeDelivery);
    QUERY->doc_var(QUERY,
                   "AcceptDragDropPayload() will returns true even before the "
                   "mouse button is released. You can then call IsDelivery() "
                   "to test if the payload needs to be delivered.");
    static t_CKINT ImGuiDragDropFlags_AcceptNoDrawDefaultRect = 2048;
    QUERY->add_svar(QUERY, "int", "AcceptNoDrawDefaultRect", true,
                    &ImGuiDragDropFlags_AcceptNoDrawDefaultRect);
    QUERY->doc_var(
      QUERY,
      "Do not draw the default highlight rectangle when hovering over target.");
    static t_CKINT ImGuiDragDropFlags_AcceptNoPreviewTooltip = 4096;
    QUERY->add_svar(QUERY, "int", "AcceptNoPreviewTooltip", true,
                    &ImGuiDragDropFlags_AcceptNoPreviewTooltip);
    QUERY->doc_var(QUERY,
                   "Request hiding the BeginDragDropSource tooltip from the "
                   "BeginDragDropTarget site.");
    static t_CKINT ImGuiDragDropFlags_AcceptPeekOnly = 3072;
    QUERY->add_svar(QUERY, "int", "AcceptPeekOnly", true,
                    &ImGuiDragDropFlags_AcceptPeekOnly);
    QUERY->doc_var(
      QUERY, "For peeking ahead and inspecting the payload before delivery.");
    QUERY->end_class(QUERY);

    QUERY->begin_class(QUERY, "UI_DataType", "Object");
    QUERY->doc_class(QUERY, "A primary data type.\n");
    static t_CKINT ImGuiDataType_S8 = 0;
    QUERY->add_svar(QUERY, "int", "S8", true, &ImGuiDataType_S8);
    QUERY->doc_var(QUERY, "signed char / char (with sensible compilers)");
    static t_CKINT ImGuiDataType_U8 = 1;
    QUERY->add_svar(QUERY, "int", "U8", true, &ImGuiDataType_U8);
    QUERY->doc_var(QUERY, "unsigned char");
    static t_CKINT ImGuiDataType_S16 = 2;
    QUERY->add_svar(QUERY, "int", "S16", true, &ImGuiDataType_S16);
    QUERY->doc_var(QUERY, "short");
    static t_CKINT ImGuiDataType_U16 = 3;
    QUERY->add_svar(QUERY, "int", "U16", true, &ImGuiDataType_U16);
    QUERY->doc_var(QUERY, "unsigned short");
    static t_CKINT ImGuiDataType_S32 = 4;
    QUERY->add_svar(QUERY, "int", "S32", true, &ImGuiDataType_S32);
    QUERY->doc_var(QUERY, "int");
    static t_CKINT ImGuiDataType_U32 = 5;
    QUERY->add_svar(QUERY, "int", "U32", true, &ImGuiDataType_U32);
    QUERY->doc_var(QUERY, "unsigned int");
    static t_CKINT ImGuiDataType_S64 = 6;
    QUERY->add_svar(QUERY, "int", "S64", true, &ImGuiDataType_S64);
    QUERY->doc_var(QUERY, "long long / __int64");
    static t_CKINT ImGuiDataType_U64 = 7;
    QUERY->add_svar(QUERY, "int", "U64", true, &ImGuiDataType_U64);
    QUERY->doc_var(QUERY, "unsigned long long / unsigned __int64");
    static t_CKINT ImGuiDataType_Float = 8;
    QUERY->add_svar(QUERY, "int", "Float", true, &ImGuiDataType_Float);
    QUERY->doc_var(QUERY, "float");
    static t_CKINT ImGuiDataType_Double = 9;
    QUERY->add_svar(QUERY, "int", "Double", true, &ImGuiDataType_Double);
    QUERY->doc_var(QUERY, "double");
    static t_CKINT ImGuiDataType_COUNT = 10;
    QUERY->add_svar(QUERY, "int", "COUNT", true, &ImGuiDataType_COUNT);
    QUERY->end_class(QUERY);

    QUERY->begin_class(QUERY, "UI_Dir", "Object");
    QUERY->doc_class(QUERY, "A cardinal direction.\n");
    static t_CKINT ImGuiDir_None = -1;
    QUERY->add_svar(QUERY, "int", "None", true, &ImGuiDir_None);
    static t_CKINT ImGuiDir_Left = 0;
    QUERY->add_svar(QUERY, "int", "Left", true, &ImGuiDir_Left);
    static t_CKINT ImGuiDir_Right = 1;
    QUERY->add_svar(QUERY, "int", "Right", true, &ImGuiDir_Right);
    static t_CKINT ImGuiDir_Up = 2;
    QUERY->add_svar(QUERY, "int", "Up", true, &ImGuiDir_Up);
    static t_CKINT ImGuiDir_Down = 3;
    QUERY->add_svar(QUERY, "int", "Down", true, &ImGuiDir_Down);
    static t_CKINT ImGuiDir_COUNT = 4;
    QUERY->add_svar(QUERY, "int", "COUNT", true, &ImGuiDir_COUNT);
    QUERY->end_class(QUERY);

    QUERY->begin_class(QUERY, "UI_SortDirection", "Object");
    QUERY->doc_class(QUERY, "A sorting direction.\n");
    static t_CKINT ImGuiSortDirection_None = 0;
    QUERY->add_svar(QUERY, "int", "None", true, &ImGuiSortDirection_None);
    static t_CKINT ImGuiSortDirection_Ascending = 1;
    QUERY->add_svar(QUERY, "int", "Ascending", true,
                    &ImGuiSortDirection_Ascending);
    QUERY->doc_var(QUERY, "Ascending = 0->9, A->Z etc.");
    static t_CKINT ImGuiSortDirection_Descending = 2;
    QUERY->add_svar(QUERY, "int", "Descending", true,
                    &ImGuiSortDirection_Descending);
    QUERY->doc_var(QUERY, "Descending = 9->0, Z->A etc.");
    QUERY->end_class(QUERY);

    QUERY->begin_class(QUERY, "UI_Key", "Object");
    QUERY->doc_class(
      QUERY,
      "A key identifier (ImGuiKey_XXX or ImGuiMod_XXX value): can represent "
      "Keyboard, Mouse and Gamepad values..\nAll our named keys are >= 512. "
      "Keys value 0 to 511 are left unused as legacy native/opaque key values "
      "(< 1.87)..\nSince >= 1.89 we increased typing (went from int to enum), "
      "some legacy code may need a cast to ImGuiKey..\nRead details about the "
      "1.87 and 1.89 transition : "
      "https:github.com/ocornut/imgui/issues/4921.\nNote that \"Keys\" related "
      "to physical keys and are not the same concept as input \"Characters\", "
      "the later are submitted via io.AddInputCharacter()..\nThe keyboard key "
      "enum values are named after the keys on a standard US keyboard, and on "
      "other keyboard types the keys reported may not match the "
      "keycaps..\nForward declared enum type ImGuiKey");
    static t_CKINT ImGuiKey_None = 0;
    QUERY->add_svar(QUERY, "int", "None", true, &ImGuiKey_None);
    static t_CKINT ImGuiKey_Tab = 512;
    QUERY->add_svar(QUERY, "int", "Tab", true, &ImGuiKey_Tab);
    QUERY->doc_var(QUERY, "== ImGuiKey_NamedKey_BEGIN");
    static t_CKINT ImGuiKey_LeftArrow = 513;
    QUERY->add_svar(QUERY, "int", "LeftArrow", true, &ImGuiKey_LeftArrow);
    static t_CKINT ImGuiKey_RightArrow = 514;
    QUERY->add_svar(QUERY, "int", "RightArrow", true, &ImGuiKey_RightArrow);
    static t_CKINT ImGuiKey_UpArrow = 515;
    QUERY->add_svar(QUERY, "int", "UpArrow", true, &ImGuiKey_UpArrow);
    static t_CKINT ImGuiKey_DownArrow = 516;
    QUERY->add_svar(QUERY, "int", "DownArrow", true, &ImGuiKey_DownArrow);
    static t_CKINT ImGuiKey_PageUp = 517;
    QUERY->add_svar(QUERY, "int", "PageUp", true, &ImGuiKey_PageUp);
    static t_CKINT ImGuiKey_PageDown = 518;
    QUERY->add_svar(QUERY, "int", "PageDown", true, &ImGuiKey_PageDown);
    static t_CKINT ImGuiKey_Home = 519;
    QUERY->add_svar(QUERY, "int", "Home", true, &ImGuiKey_Home);
    static t_CKINT ImGuiKey_End = 520;
    QUERY->add_svar(QUERY, "int", "End", true, &ImGuiKey_End);
    static t_CKINT ImGuiKey_Insert = 521;
    QUERY->add_svar(QUERY, "int", "Insert", true, &ImGuiKey_Insert);
    static t_CKINT ImGuiKey_Delete = 522;
    QUERY->add_svar(QUERY, "int", "Delete", true, &ImGuiKey_Delete);
    static t_CKINT ImGuiKey_Backspace = 523;
    QUERY->add_svar(QUERY, "int", "Backspace", true, &ImGuiKey_Backspace);
    static t_CKINT ImGuiKey_Space = 524;
    QUERY->add_svar(QUERY, "int", "Space", true, &ImGuiKey_Space);
    static t_CKINT ImGuiKey_Enter = 525;
    QUERY->add_svar(QUERY, "int", "Enter", true, &ImGuiKey_Enter);
    static t_CKINT ImGuiKey_Escape = 526;
    QUERY->add_svar(QUERY, "int", "Escape", true, &ImGuiKey_Escape);
    static t_CKINT ImGuiKey_LeftCtrl = 527;
    QUERY->add_svar(QUERY, "int", "LeftCtrl", true, &ImGuiKey_LeftCtrl);
    static t_CKINT ImGuiKey_LeftShift = 528;
    QUERY->add_svar(QUERY, "int", "LeftShift", true, &ImGuiKey_LeftShift);
    static t_CKINT ImGuiKey_LeftAlt = 529;
    QUERY->add_svar(QUERY, "int", "LeftAlt", true, &ImGuiKey_LeftAlt);
    static t_CKINT ImGuiKey_LeftSuper = 530;
    QUERY->add_svar(QUERY, "int", "LeftSuper", true, &ImGuiKey_LeftSuper);
    static t_CKINT ImGuiKey_RightCtrl = 531;
    QUERY->add_svar(QUERY, "int", "RightCtrl", true, &ImGuiKey_RightCtrl);
    static t_CKINT ImGuiKey_RightShift = 532;
    QUERY->add_svar(QUERY, "int", "RightShift", true, &ImGuiKey_RightShift);
    static t_CKINT ImGuiKey_RightAlt = 533;
    QUERY->add_svar(QUERY, "int", "RightAlt", true, &ImGuiKey_RightAlt);
    static t_CKINT ImGuiKey_RightSuper = 534;
    QUERY->add_svar(QUERY, "int", "RightSuper", true, &ImGuiKey_RightSuper);
    static t_CKINT ImGuiKey_Menu = 535;
    QUERY->add_svar(QUERY, "int", "Menu", true, &ImGuiKey_Menu);
    static t_CKINT ImGuiKey_0 = 536;
    QUERY->add_svar(QUERY, "int", "0", true, &ImGuiKey_0);
    static t_CKINT ImGuiKey_1 = 537;
    QUERY->add_svar(QUERY, "int", "1", true, &ImGuiKey_1);
    static t_CKINT ImGuiKey_2 = 538;
    QUERY->add_svar(QUERY, "int", "2", true, &ImGuiKey_2);
    static t_CKINT ImGuiKey_3 = 539;
    QUERY->add_svar(QUERY, "int", "3", true, &ImGuiKey_3);
    static t_CKINT ImGuiKey_4 = 540;
    QUERY->add_svar(QUERY, "int", "4", true, &ImGuiKey_4);
    static t_CKINT ImGuiKey_5 = 541;
    QUERY->add_svar(QUERY, "int", "5", true, &ImGuiKey_5);
    static t_CKINT ImGuiKey_6 = 542;
    QUERY->add_svar(QUERY, "int", "6", true, &ImGuiKey_6);
    static t_CKINT ImGuiKey_7 = 543;
    QUERY->add_svar(QUERY, "int", "7", true, &ImGuiKey_7);
    static t_CKINT ImGuiKey_8 = 544;
    QUERY->add_svar(QUERY, "int", "8", true, &ImGuiKey_8);
    static t_CKINT ImGuiKey_9 = 545;
    QUERY->add_svar(QUERY, "int", "9", true, &ImGuiKey_9);
    static t_CKINT ImGuiKey_A = 546;
    QUERY->add_svar(QUERY, "int", "A", true, &ImGuiKey_A);
    static t_CKINT ImGuiKey_B = 547;
    QUERY->add_svar(QUERY, "int", "B", true, &ImGuiKey_B);
    static t_CKINT ImGuiKey_C = 548;
    QUERY->add_svar(QUERY, "int", "C", true, &ImGuiKey_C);
    static t_CKINT ImGuiKey_D = 549;
    QUERY->add_svar(QUERY, "int", "D", true, &ImGuiKey_D);
    static t_CKINT ImGuiKey_E = 550;
    QUERY->add_svar(QUERY, "int", "E", true, &ImGuiKey_E);
    static t_CKINT ImGuiKey_F = 551;
    QUERY->add_svar(QUERY, "int", "F", true, &ImGuiKey_F);
    static t_CKINT ImGuiKey_G = 552;
    QUERY->add_svar(QUERY, "int", "G", true, &ImGuiKey_G);
    static t_CKINT ImGuiKey_H = 553;
    QUERY->add_svar(QUERY, "int", "H", true, &ImGuiKey_H);
    static t_CKINT ImGuiKey_I = 554;
    QUERY->add_svar(QUERY, "int", "I", true, &ImGuiKey_I);
    static t_CKINT ImGuiKey_J = 555;
    QUERY->add_svar(QUERY, "int", "J", true, &ImGuiKey_J);
    static t_CKINT ImGuiKey_K = 556;
    QUERY->add_svar(QUERY, "int", "K", true, &ImGuiKey_K);
    static t_CKINT ImGuiKey_L = 557;
    QUERY->add_svar(QUERY, "int", "L", true, &ImGuiKey_L);
    static t_CKINT ImGuiKey_M = 558;
    QUERY->add_svar(QUERY, "int", "M", true, &ImGuiKey_M);
    static t_CKINT ImGuiKey_N = 559;
    QUERY->add_svar(QUERY, "int", "N", true, &ImGuiKey_N);
    static t_CKINT ImGuiKey_O = 560;
    QUERY->add_svar(QUERY, "int", "O", true, &ImGuiKey_O);
    static t_CKINT ImGuiKey_P = 561;
    QUERY->add_svar(QUERY, "int", "P", true, &ImGuiKey_P);
    static t_CKINT ImGuiKey_Q = 562;
    QUERY->add_svar(QUERY, "int", "Q", true, &ImGuiKey_Q);
    static t_CKINT ImGuiKey_R = 563;
    QUERY->add_svar(QUERY, "int", "R", true, &ImGuiKey_R);
    static t_CKINT ImGuiKey_S = 564;
    QUERY->add_svar(QUERY, "int", "S", true, &ImGuiKey_S);
    static t_CKINT ImGuiKey_T = 565;
    QUERY->add_svar(QUERY, "int", "T", true, &ImGuiKey_T);
    static t_CKINT ImGuiKey_U = 566;
    QUERY->add_svar(QUERY, "int", "U", true, &ImGuiKey_U);
    static t_CKINT ImGuiKey_V = 567;
    QUERY->add_svar(QUERY, "int", "V", true, &ImGuiKey_V);
    static t_CKINT ImGuiKey_W = 568;
    QUERY->add_svar(QUERY, "int", "W", true, &ImGuiKey_W);
    static t_CKINT ImGuiKey_X = 569;
    QUERY->add_svar(QUERY, "int", "X", true, &ImGuiKey_X);
    static t_CKINT ImGuiKey_Y = 570;
    QUERY->add_svar(QUERY, "int", "Y", true, &ImGuiKey_Y);
    static t_CKINT ImGuiKey_Z = 571;
    QUERY->add_svar(QUERY, "int", "Z", true, &ImGuiKey_Z);
    static t_CKINT ImGuiKey_F1 = 572;
    QUERY->add_svar(QUERY, "int", "F1", true, &ImGuiKey_F1);
    static t_CKINT ImGuiKey_F2 = 573;
    QUERY->add_svar(QUERY, "int", "F2", true, &ImGuiKey_F2);
    static t_CKINT ImGuiKey_F3 = 574;
    QUERY->add_svar(QUERY, "int", "F3", true, &ImGuiKey_F3);
    static t_CKINT ImGuiKey_F4 = 575;
    QUERY->add_svar(QUERY, "int", "F4", true, &ImGuiKey_F4);
    static t_CKINT ImGuiKey_F5 = 576;
    QUERY->add_svar(QUERY, "int", "F5", true, &ImGuiKey_F5);
    static t_CKINT ImGuiKey_F6 = 577;
    QUERY->add_svar(QUERY, "int", "F6", true, &ImGuiKey_F6);
    static t_CKINT ImGuiKey_F7 = 578;
    QUERY->add_svar(QUERY, "int", "F7", true, &ImGuiKey_F7);
    static t_CKINT ImGuiKey_F8 = 579;
    QUERY->add_svar(QUERY, "int", "F8", true, &ImGuiKey_F8);
    static t_CKINT ImGuiKey_F9 = 580;
    QUERY->add_svar(QUERY, "int", "F9", true, &ImGuiKey_F9);
    static t_CKINT ImGuiKey_F10 = 581;
    QUERY->add_svar(QUERY, "int", "F10", true, &ImGuiKey_F10);
    static t_CKINT ImGuiKey_F11 = 582;
    QUERY->add_svar(QUERY, "int", "F11", true, &ImGuiKey_F11);
    static t_CKINT ImGuiKey_F12 = 583;
    QUERY->add_svar(QUERY, "int", "F12", true, &ImGuiKey_F12);
    static t_CKINT ImGuiKey_F13 = 584;
    QUERY->add_svar(QUERY, "int", "F13", true, &ImGuiKey_F13);
    static t_CKINT ImGuiKey_F14 = 585;
    QUERY->add_svar(QUERY, "int", "F14", true, &ImGuiKey_F14);
    static t_CKINT ImGuiKey_F15 = 586;
    QUERY->add_svar(QUERY, "int", "F15", true, &ImGuiKey_F15);
    static t_CKINT ImGuiKey_F16 = 587;
    QUERY->add_svar(QUERY, "int", "F16", true, &ImGuiKey_F16);
    static t_CKINT ImGuiKey_F17 = 588;
    QUERY->add_svar(QUERY, "int", "F17", true, &ImGuiKey_F17);
    static t_CKINT ImGuiKey_F18 = 589;
    QUERY->add_svar(QUERY, "int", "F18", true, &ImGuiKey_F18);
    static t_CKINT ImGuiKey_F19 = 590;
    QUERY->add_svar(QUERY, "int", "F19", true, &ImGuiKey_F19);
    static t_CKINT ImGuiKey_F20 = 591;
    QUERY->add_svar(QUERY, "int", "F20", true, &ImGuiKey_F20);
    static t_CKINT ImGuiKey_F21 = 592;
    QUERY->add_svar(QUERY, "int", "F21", true, &ImGuiKey_F21);
    static t_CKINT ImGuiKey_F22 = 593;
    QUERY->add_svar(QUERY, "int", "F22", true, &ImGuiKey_F22);
    static t_CKINT ImGuiKey_F23 = 594;
    QUERY->add_svar(QUERY, "int", "F23", true, &ImGuiKey_F23);
    static t_CKINT ImGuiKey_F24 = 595;
    QUERY->add_svar(QUERY, "int", "F24", true, &ImGuiKey_F24);
    static t_CKINT ImGuiKey_Apostrophe = 596;
    QUERY->add_svar(QUERY, "int", "Apostrophe", true, &ImGuiKey_Apostrophe);
    QUERY->doc_var(QUERY, "'");
    static t_CKINT ImGuiKey_Comma = 597;
    QUERY->add_svar(QUERY, "int", "Comma", true, &ImGuiKey_Comma);
    QUERY->doc_var(QUERY, ",");
    static t_CKINT ImGuiKey_Minus = 598;
    QUERY->add_svar(QUERY, "int", "Minus", true, &ImGuiKey_Minus);
    QUERY->doc_var(QUERY, "-");
    static t_CKINT ImGuiKey_Period = 599;
    QUERY->add_svar(QUERY, "int", "Period", true, &ImGuiKey_Period);
    QUERY->doc_var(QUERY, ".");
    static t_CKINT ImGuiKey_Slash = 600;
    QUERY->add_svar(QUERY, "int", "Slash", true, &ImGuiKey_Slash);
    QUERY->doc_var(QUERY, "/");
    static t_CKINT ImGuiKey_Semicolon = 601;
    QUERY->add_svar(QUERY, "int", "Semicolon", true, &ImGuiKey_Semicolon);
    QUERY->doc_var(QUERY, ";");
    static t_CKINT ImGuiKey_Equal = 602;
    QUERY->add_svar(QUERY, "int", "Equal", true, &ImGuiKey_Equal);
    QUERY->doc_var(QUERY, "=");
    static t_CKINT ImGuiKey_LeftBracket = 603;
    QUERY->add_svar(QUERY, "int", "LeftBracket", true, &ImGuiKey_LeftBracket);
    QUERY->doc_var(QUERY, "[");
    static t_CKINT ImGuiKey_Backslash = 604;
    QUERY->add_svar(QUERY, "int", "Backslash", true, &ImGuiKey_Backslash);
    QUERY->doc_var(
      QUERY, "\\ (this text inhibit multiline comment caused by backslash)");
    static t_CKINT ImGuiKey_RightBracket = 605;
    QUERY->add_svar(QUERY, "int", "RightBracket", true, &ImGuiKey_RightBracket);
    QUERY->doc_var(QUERY, "]");
    static t_CKINT ImGuiKey_GraveAccent = 606;
    QUERY->add_svar(QUERY, "int", "GraveAccent", true, &ImGuiKey_GraveAccent);
    QUERY->doc_var(QUERY, "`");
    static t_CKINT ImGuiKey_CapsLock = 607;
    QUERY->add_svar(QUERY, "int", "CapsLock", true, &ImGuiKey_CapsLock);
    static t_CKINT ImGuiKey_ScrollLock = 608;
    QUERY->add_svar(QUERY, "int", "ScrollLock", true, &ImGuiKey_ScrollLock);
    static t_CKINT ImGuiKey_NumLock = 609;
    QUERY->add_svar(QUERY, "int", "NumLock", true, &ImGuiKey_NumLock);
    static t_CKINT ImGuiKey_PrintScreen = 610;
    QUERY->add_svar(QUERY, "int", "PrintScreen", true, &ImGuiKey_PrintScreen);
    static t_CKINT ImGuiKey_Pause = 611;
    QUERY->add_svar(QUERY, "int", "Pause", true, &ImGuiKey_Pause);
    static t_CKINT ImGuiKey_Keypad0 = 612;
    QUERY->add_svar(QUERY, "int", "Keypad0", true, &ImGuiKey_Keypad0);
    static t_CKINT ImGuiKey_Keypad1 = 613;
    QUERY->add_svar(QUERY, "int", "Keypad1", true, &ImGuiKey_Keypad1);
    static t_CKINT ImGuiKey_Keypad2 = 614;
    QUERY->add_svar(QUERY, "int", "Keypad2", true, &ImGuiKey_Keypad2);
    static t_CKINT ImGuiKey_Keypad3 = 615;
    QUERY->add_svar(QUERY, "int", "Keypad3", true, &ImGuiKey_Keypad3);
    static t_CKINT ImGuiKey_Keypad4 = 616;
    QUERY->add_svar(QUERY, "int", "Keypad4", true, &ImGuiKey_Keypad4);
    static t_CKINT ImGuiKey_Keypad5 = 617;
    QUERY->add_svar(QUERY, "int", "Keypad5", true, &ImGuiKey_Keypad5);
    static t_CKINT ImGuiKey_Keypad6 = 618;
    QUERY->add_svar(QUERY, "int", "Keypad6", true, &ImGuiKey_Keypad6);
    static t_CKINT ImGuiKey_Keypad7 = 619;
    QUERY->add_svar(QUERY, "int", "Keypad7", true, &ImGuiKey_Keypad7);
    static t_CKINT ImGuiKey_Keypad8 = 620;
    QUERY->add_svar(QUERY, "int", "Keypad8", true, &ImGuiKey_Keypad8);
    static t_CKINT ImGuiKey_Keypad9 = 621;
    QUERY->add_svar(QUERY, "int", "Keypad9", true, &ImGuiKey_Keypad9);
    static t_CKINT ImGuiKey_KeypadDecimal = 622;
    QUERY->add_svar(QUERY, "int", "KeypadDecimal", true,
                    &ImGuiKey_KeypadDecimal);
    static t_CKINT ImGuiKey_KeypadDivide = 623;
    QUERY->add_svar(QUERY, "int", "KeypadDivide", true, &ImGuiKey_KeypadDivide);
    static t_CKINT ImGuiKey_KeypadMultiply = 624;
    QUERY->add_svar(QUERY, "int", "KeypadMultiply", true,
                    &ImGuiKey_KeypadMultiply);
    static t_CKINT ImGuiKey_KeypadSubtract = 625;
    QUERY->add_svar(QUERY, "int", "KeypadSubtract", true,
                    &ImGuiKey_KeypadSubtract);
    static t_CKINT ImGuiKey_KeypadAdd = 626;
    QUERY->add_svar(QUERY, "int", "KeypadAdd", true, &ImGuiKey_KeypadAdd);
    static t_CKINT ImGuiKey_KeypadEnter = 627;
    QUERY->add_svar(QUERY, "int", "KeypadEnter", true, &ImGuiKey_KeypadEnter);
    static t_CKINT ImGuiKey_KeypadEqual = 628;
    QUERY->add_svar(QUERY, "int", "KeypadEqual", true, &ImGuiKey_KeypadEqual);
    static t_CKINT ImGuiKey_AppBack = 629;
    QUERY->add_svar(QUERY, "int", "AppBack", true, &ImGuiKey_AppBack);
    QUERY->doc_var(
      QUERY,
      "Available on some keyboard/mouses. Often referred as \"Browser Back\"");
    static t_CKINT ImGuiKey_AppForward = 630;
    QUERY->add_svar(QUERY, "int", "AppForward", true, &ImGuiKey_AppForward);
    static t_CKINT ImGuiKey_GamepadStart = 631;
    QUERY->add_svar(QUERY, "int", "GamepadStart", true, &ImGuiKey_GamepadStart);
    QUERY->doc_var(QUERY, "Menu (Xbox)      + (Switch)   Start/Options (PS)");
    static t_CKINT ImGuiKey_GamepadBack = 632;
    QUERY->add_svar(QUERY, "int", "GamepadBack", true, &ImGuiKey_GamepadBack);
    QUERY->doc_var(QUERY, "View (Xbox)      - (Switch)   Share (PS)");
    static t_CKINT ImGuiKey_GamepadFaceLeft = 633;
    QUERY->add_svar(QUERY, "int", "GamepadFaceLeft", true,
                    &ImGuiKey_GamepadFaceLeft);
    QUERY->doc_var(
      QUERY,
      "X (Xbox)         Y (Switch)   Square (PS)         Tap: Toggle Menu. "
      "Hold: Windowing mode (Focus/Move/Resize windows)");
    static t_CKINT ImGuiKey_GamepadFaceRight = 634;
    QUERY->add_svar(QUERY, "int", "GamepadFaceRight", true,
                    &ImGuiKey_GamepadFaceRight);
    QUERY->doc_var(QUERY,
                   "B (Xbox)         A (Switch)   Circle (PS)         Cancel / "
                   "Close / Exit");
    static t_CKINT ImGuiKey_GamepadFaceUp = 635;
    QUERY->add_svar(QUERY, "int", "GamepadFaceUp", true,
                    &ImGuiKey_GamepadFaceUp);
    QUERY->doc_var(QUERY,
                   "Y (Xbox)         X (Switch)   Triangle (PS)       Text "
                   "Input / On-screen Keyboard");
    static t_CKINT ImGuiKey_GamepadFaceDown = 636;
    QUERY->add_svar(QUERY, "int", "GamepadFaceDown", true,
                    &ImGuiKey_GamepadFaceDown);
    QUERY->doc_var(QUERY,
                   "A (Xbox)         B (Switch)   Cross (PS)          Activate "
                   "/ Open / Toggle / Tweak");
    static t_CKINT ImGuiKey_GamepadDpadLeft = 637;
    QUERY->add_svar(QUERY, "int", "GamepadDpadLeft", true,
                    &ImGuiKey_GamepadDpadLeft);
    QUERY->doc_var(QUERY,
                   "D-pad Left                                        Move / "
                   "Tweak / Resize Window (in Windowing mode)");
    static t_CKINT ImGuiKey_GamepadDpadRight = 638;
    QUERY->add_svar(QUERY, "int", "GamepadDpadRight", true,
                    &ImGuiKey_GamepadDpadRight);
    QUERY->doc_var(QUERY,
                   "D-pad Right                                       Move / "
                   "Tweak / Resize Window (in Windowing mode)");
    static t_CKINT ImGuiKey_GamepadDpadUp = 639;
    QUERY->add_svar(QUERY, "int", "GamepadDpadUp", true,
                    &ImGuiKey_GamepadDpadUp);
    QUERY->doc_var(QUERY,
                   "D-pad Up                                          Move / "
                   "Tweak / Resize Window (in Windowing mode)");
    static t_CKINT ImGuiKey_GamepadDpadDown = 640;
    QUERY->add_svar(QUERY, "int", "GamepadDpadDown", true,
                    &ImGuiKey_GamepadDpadDown);
    QUERY->doc_var(QUERY,
                   "D-pad Down                                        Move / "
                   "Tweak / Resize Window (in Windowing mode)");
    static t_CKINT ImGuiKey_GamepadL1 = 641;
    QUERY->add_svar(QUERY, "int", "GamepadL1", true, &ImGuiKey_GamepadL1);
    QUERY->doc_var(QUERY,
                   "L Bumper (Xbox)  L (Switch)   L1 (PS)             Tweak "
                   "Slower / Focus Previous (in Windowing mode)");
    static t_CKINT ImGuiKey_GamepadR1 = 642;
    QUERY->add_svar(QUERY, "int", "GamepadR1", true, &ImGuiKey_GamepadR1);
    QUERY->doc_var(QUERY,
                   "R Bumper (Xbox)  R (Switch)   R1 (PS)             Tweak "
                   "Faster / Focus Next (in Windowing mode)");
    static t_CKINT ImGuiKey_GamepadL2 = 643;
    QUERY->add_svar(QUERY, "int", "GamepadL2", true, &ImGuiKey_GamepadL2);
    QUERY->doc_var(QUERY, "L Trig. (Xbox)   ZL (Switch)  L2 (PS) [Analog]");
    static t_CKINT ImGuiKey_GamepadR2 = 644;
    QUERY->add_svar(QUERY, "int", "GamepadR2", true, &ImGuiKey_GamepadR2);
    QUERY->doc_var(QUERY, "R Trig. (Xbox)   ZR (Switch)  R2 (PS) [Analog]");
    static t_CKINT ImGuiKey_GamepadL3 = 645;
    QUERY->add_svar(QUERY, "int", "GamepadL3", true, &ImGuiKey_GamepadL3);
    QUERY->doc_var(QUERY, "L Stick (Xbox)   L3 (Switch)  L3 (PS)");
    static t_CKINT ImGuiKey_GamepadR3 = 646;
    QUERY->add_svar(QUERY, "int", "GamepadR3", true, &ImGuiKey_GamepadR3);
    QUERY->doc_var(QUERY, "R Stick (Xbox)   R3 (Switch)  R3 (PS)");
    static t_CKINT ImGuiKey_GamepadLStickLeft = 647;
    QUERY->add_svar(QUERY, "int", "GamepadLStickLeft", true,
                    &ImGuiKey_GamepadLStickLeft);
    QUERY->doc_var(QUERY,
                   "[Analog]                                          Move "
                   "Window (in Windowing mode)");
    static t_CKINT ImGuiKey_GamepadLStickRight = 648;
    QUERY->add_svar(QUERY, "int", "GamepadLStickRight", true,
                    &ImGuiKey_GamepadLStickRight);
    QUERY->doc_var(QUERY,
                   "[Analog]                                          Move "
                   "Window (in Windowing mode)");
    static t_CKINT ImGuiKey_GamepadLStickUp = 649;
    QUERY->add_svar(QUERY, "int", "GamepadLStickUp", true,
                    &ImGuiKey_GamepadLStickUp);
    QUERY->doc_var(QUERY,
                   "[Analog]                                          Move "
                   "Window (in Windowing mode)");
    static t_CKINT ImGuiKey_GamepadLStickDown = 650;
    QUERY->add_svar(QUERY, "int", "GamepadLStickDown", true,
                    &ImGuiKey_GamepadLStickDown);
    QUERY->doc_var(QUERY,
                   "[Analog]                                          Move "
                   "Window (in Windowing mode)");
    static t_CKINT ImGuiKey_GamepadRStickLeft = 651;
    QUERY->add_svar(QUERY, "int", "GamepadRStickLeft", true,
                    &ImGuiKey_GamepadRStickLeft);
    QUERY->doc_var(QUERY, "[Analog]");
    static t_CKINT ImGuiKey_GamepadRStickRight = 652;
    QUERY->add_svar(QUERY, "int", "GamepadRStickRight", true,
                    &ImGuiKey_GamepadRStickRight);
    QUERY->doc_var(QUERY, "[Analog]");
    static t_CKINT ImGuiKey_GamepadRStickUp = 653;
    QUERY->add_svar(QUERY, "int", "GamepadRStickUp", true,
                    &ImGuiKey_GamepadRStickUp);
    QUERY->doc_var(QUERY, "[Analog]");
    static t_CKINT ImGuiKey_GamepadRStickDown = 654;
    QUERY->add_svar(QUERY, "int", "GamepadRStickDown", true,
                    &ImGuiKey_GamepadRStickDown);
    QUERY->doc_var(QUERY, "[Analog]");
    static t_CKINT ImGuiKey_MouseLeft = 655;
    QUERY->add_svar(QUERY, "int", "MouseLeft", true, &ImGuiKey_MouseLeft);
    static t_CKINT ImGuiKey_MouseRight = 656;
    QUERY->add_svar(QUERY, "int", "MouseRight", true, &ImGuiKey_MouseRight);
    static t_CKINT ImGuiKey_MouseMiddle = 657;
    QUERY->add_svar(QUERY, "int", "MouseMiddle", true, &ImGuiKey_MouseMiddle);
    static t_CKINT ImGuiKey_MouseX1 = 658;
    QUERY->add_svar(QUERY, "int", "MouseX1", true, &ImGuiKey_MouseX1);
    static t_CKINT ImGuiKey_MouseX2 = 659;
    QUERY->add_svar(QUERY, "int", "MouseX2", true, &ImGuiKey_MouseX2);
    static t_CKINT ImGuiKey_MouseWheelX = 660;
    QUERY->add_svar(QUERY, "int", "MouseWheelX", true, &ImGuiKey_MouseWheelX);
    static t_CKINT ImGuiKey_MouseWheelY = 661;
    QUERY->add_svar(QUERY, "int", "MouseWheelY", true, &ImGuiKey_MouseWheelY);
    static t_CKINT ImGuiMod_None = 0;
    QUERY->add_svar(QUERY, "int", "Mod_None", true, &ImGuiMod_None);
    static t_CKINT ImGuiMod_Ctrl = 4096;
    QUERY->add_svar(QUERY, "int", "Mod_Ctrl", true, &ImGuiMod_Ctrl);
    QUERY->doc_var(QUERY, "Ctrl (non-macOS), Cmd (macOS)");
    static t_CKINT ImGuiMod_Shift = 8192;
    QUERY->add_svar(QUERY, "int", "Mod_Shift", true, &ImGuiMod_Shift);
    QUERY->doc_var(QUERY, "Shift");
    static t_CKINT ImGuiMod_Alt = 16384;
    QUERY->add_svar(QUERY, "int", "Mod_Alt", true, &ImGuiMod_Alt);
    QUERY->doc_var(QUERY, "Option/Menu");
    static t_CKINT ImGuiMod_Super = 32768;
    QUERY->add_svar(QUERY, "int", "Mod_Super", true, &ImGuiMod_Super);
    QUERY->doc_var(QUERY, "Windows/Super (non-macOS), Ctrl (macOS)");
    static t_CKINT ImGuiKey_KeysData_SIZE = 154;
    QUERY->add_svar(QUERY, "int", "KeysData_SIZE", true,
                    &ImGuiKey_KeysData_SIZE);
    QUERY->doc_var(QUERY, "Size of KeysData[]: only hold named keys");
    static t_CKINT ImGuiKey_KeysData_OFFSET = 512;
    QUERY->add_svar(QUERY, "int", "KeysData_OFFSET", true,
                    &ImGuiKey_KeysData_OFFSET);
    QUERY->doc_var(QUERY,
                   "Accesses to io.KeysData[] must use (key - "
                   "ImGuiKey_KeysData_OFFSET) index.");
    QUERY->end_class(QUERY);

    QUERY->begin_class(QUERY, "UI_InputFlags", "Object");
    QUERY->doc_class(
      QUERY,
      "Flags for Shortcut(), SetNextItemShortcut(),.\n(and for upcoming "
      "extended versions of IsKeyPressed(), IsMouseClicked(), Shortcut(), "
      "SetKeyOwner(), SetItemKeyOwner() that are still in "
      "imgui_internal.h).\nDon't mistake with ImGuiInputTextFlags! (which is "
      "for ImGui::InputText() function).\n");
    static t_CKINT ImGuiInputFlags_None = 0;
    QUERY->add_svar(QUERY, "int", "None", true, &ImGuiInputFlags_None);
    static t_CKINT ImGuiInputFlags_Repeat = 1;
    QUERY->add_svar(QUERY, "int", "Repeat", true, &ImGuiInputFlags_Repeat);
    QUERY->doc_var(
      QUERY,
      "Enable repeat. Return true on successive repeats. Default for legacy "
      "IsKeyPressed(). NOT Default for legacy IsMouseClicked(). MUST BE == 1.");
    static t_CKINT ImGuiInputFlags_RouteActive = 1024;
    QUERY->add_svar(QUERY, "int", "RouteActive", true,
                    &ImGuiInputFlags_RouteActive);
    QUERY->doc_var(QUERY, "Route to active item only.");
    static t_CKINT ImGuiInputFlags_RouteFocused = 2048;
    QUERY->add_svar(QUERY, "int", "RouteFocused", true,
                    &ImGuiInputFlags_RouteFocused);
    QUERY->doc_var(
      QUERY,
      "Route to windows in the focus stack (DEFAULT). Deep-most focused window "
      "takes inputs. Active item takes inputs over deep-most focused window.");
    static t_CKINT ImGuiInputFlags_RouteGlobal = 4096;
    QUERY->add_svar(QUERY, "int", "RouteGlobal", true,
                    &ImGuiInputFlags_RouteGlobal);
    QUERY->doc_var(QUERY,
                   "Global route (unless a focused window or active item "
                   "registered the route).");
    static t_CKINT ImGuiInputFlags_RouteAlways = 8192;
    QUERY->add_svar(QUERY, "int", "RouteAlways", true,
                    &ImGuiInputFlags_RouteAlways);
    QUERY->doc_var(QUERY, "Do not register route, poll keys directly.");
    static t_CKINT ImGuiInputFlags_RouteOverFocused = 16384;
    QUERY->add_svar(QUERY, "int", "RouteOverFocused", true,
                    &ImGuiInputFlags_RouteOverFocused);
    QUERY->doc_var(QUERY,
                   "Option: global route: higher priority than focused route "
                   "(unless active item in focused route).");
    static t_CKINT ImGuiInputFlags_RouteOverActive = 32768;
    QUERY->add_svar(QUERY, "int", "RouteOverActive", true,
                    &ImGuiInputFlags_RouteOverActive);
    QUERY->doc_var(
      QUERY,
      "Option: global route: higher priority than active item. Unlikely you "
      "need to use that: will interfere with every active items, e.g. CTRL+A "
      "registered by InputText will be overridden by this. May not be fully "
      "honored as user/internal code is likely to always assume they can "
      "access keys when active.");
    static t_CKINT ImGuiInputFlags_RouteUnlessBgFocused = 65536;
    QUERY->add_svar(QUERY, "int", "RouteUnlessBgFocused", true,
                    &ImGuiInputFlags_RouteUnlessBgFocused);
    QUERY->doc_var(QUERY,
                   "Option: global route: will not be applied if underlying "
                   "background/void is focused (== no Dear ImGui windows are "
                   "focused). Useful for overlay applications.");
    static t_CKINT ImGuiInputFlags_RouteFromRootWindow = 131072;
    QUERY->add_svar(QUERY, "int", "RouteFromRootWindow", true,
                    &ImGuiInputFlags_RouteFromRootWindow);
    QUERY->doc_var(QUERY,
                   "Option: route evaluated from the point of view of root "
                   "window rather than current window.");
    static t_CKINT ImGuiInputFlags_Tooltip = 262144;
    QUERY->add_svar(QUERY, "int", "Tooltip", true, &ImGuiInputFlags_Tooltip);
    QUERY->doc_var(QUERY,
                   "Automatically display a tooltip when hovering item [BETA] "
                   "Unsure of right api (opt-in/opt-out)");
    QUERY->end_class(QUERY);

    QUERY->begin_class(QUERY, "UI_ConfigFlags", "Object");
    QUERY->doc_class(QUERY,
                     "Configuration flags stored in io.ConfigFlags. Set by "
                     "user/application..\n");
    static t_CKINT ImGuiConfigFlags_None = 0;
    QUERY->add_svar(QUERY, "int", "None", true, &ImGuiConfigFlags_None);
    static t_CKINT ImGuiConfigFlags_NavEnableKeyboard = 1;
    QUERY->add_svar(QUERY, "int", "NavEnableKeyboard", true,
                    &ImGuiConfigFlags_NavEnableKeyboard);
    QUERY->doc_var(QUERY,
                   "Master keyboard navigation enable flag. Enable full "
                   "Tabbing + directional arrows + space/enter to activate.");
    static t_CKINT ImGuiConfigFlags_NavEnableGamepad = 2;
    QUERY->add_svar(QUERY, "int", "NavEnableGamepad", true,
                    &ImGuiConfigFlags_NavEnableGamepad);
    QUERY->doc_var(QUERY,
                   "Master gamepad navigation enable flag. Backend also needs "
                   "to set ImGuiBackendFlags_HasGamepad.");
    static t_CKINT ImGuiConfigFlags_NavEnableSetMousePos = 4;
    QUERY->add_svar(QUERY, "int", "NavEnableSetMousePos", true,
                    &ImGuiConfigFlags_NavEnableSetMousePos);
    QUERY->doc_var(
      QUERY,
      "Instruct navigation to move the mouse cursor. May be useful on "
      "TV/console systems where moving a virtual mouse is awkward. Will update "
      "io.MousePos and set io.WantSetMousePos=true. If enabled you MUST honor "
      "io.WantSetMousePos requests in your backend, otherwise ImGui will react "
      "as if the mouse is jumping around back and forth.");
    static t_CKINT ImGuiConfigFlags_NavNoCaptureKeyboard = 8;
    QUERY->add_svar(QUERY, "int", "NavNoCaptureKeyboard", true,
                    &ImGuiConfigFlags_NavNoCaptureKeyboard);
    QUERY->doc_var(QUERY,
                   "Instruct navigation to not set the io.WantCaptureKeyboard "
                   "flag when io.NavActive is set.");
    static t_CKINT ImGuiConfigFlags_NoMouse = 16;
    QUERY->add_svar(QUERY, "int", "NoMouse", true, &ImGuiConfigFlags_NoMouse);
    QUERY->doc_var(
      QUERY,
      "Instruct imgui to clear mouse position/buttons in NewFrame(). This "
      "allows ignoring the mouse information set by the backend.");
    static t_CKINT ImGuiConfigFlags_NoMouseCursorChange = 32;
    QUERY->add_svar(QUERY, "int", "NoMouseCursorChange", true,
                    &ImGuiConfigFlags_NoMouseCursorChange);
    QUERY->doc_var(
      QUERY,
      "Instruct backend to not alter mouse cursor shape and visibility. Use if "
      "the backend cursor changes are interfering with yours and you don't "
      "want to use SetMouseCursor() to change mouse cursor. You may want to "
      "honor requests from imgui by reading GetMouseCursor() yourself "
      "instead.");
    static t_CKINT ImGuiConfigFlags_DockingEnable = 64;
    QUERY->add_svar(QUERY, "int", "DockingEnable", true,
                    &ImGuiConfigFlags_DockingEnable);
    QUERY->doc_var(QUERY, "Docking enable flags.");
    static t_CKINT ImGuiConfigFlags_ViewportsEnable = 1024;
    QUERY->add_svar(QUERY, "int", "ViewportsEnable", true,
                    &ImGuiConfigFlags_ViewportsEnable);
    QUERY->doc_var(
      QUERY,
      "Viewport enable flags (require both "
      "ImGuiBackendFlags_PlatformHasViewports + "
      "ImGuiBackendFlags_RendererHasViewports set by the respective backends)");
    static t_CKINT ImGuiConfigFlags_DpiEnableScaleViewports = 16384;
    QUERY->add_svar(QUERY, "int", "DpiEnableScaleViewports", true,
                    &ImGuiConfigFlags_DpiEnableScaleViewports);
    QUERY->doc_var(
      QUERY,
      "[BETA: Don't use] FIXME-DPI: Reposition and resize imgui windows when "
      "the DpiScale of a viewport changed (mostly useful for the main viewport "
      "hosting other window). Note that resizing the main window itself is up "
      "to your application.");
    static t_CKINT ImGuiConfigFlags_DpiEnableScaleFonts = 32768;
    QUERY->add_svar(QUERY, "int", "DpiEnableScaleFonts", true,
                    &ImGuiConfigFlags_DpiEnableScaleFonts);
    QUERY->doc_var(QUERY,
                   "[BETA: Don't use] FIXME-DPI: Request bitmap-scaled fonts "
                   "to match DpiScale. This is a very low-quality workaround. "
                   "The correct way to handle DPI is _currently_ to replace "
                   "the atlas and/or fonts in the Platform_OnChangedViewport "
                   "callback, but this is all early work in progress.");
    static t_CKINT ImGuiConfigFlags_IsSRGB = 1048576;
    QUERY->add_svar(QUERY, "int", "IsSRGB", true, &ImGuiConfigFlags_IsSRGB);
    QUERY->doc_var(QUERY, "Application is SRGB-aware.");
    static t_CKINT ImGuiConfigFlags_IsTouchScreen = 2097152;
    QUERY->add_svar(QUERY, "int", "IsTouchScreen", true,
                    &ImGuiConfigFlags_IsTouchScreen);
    QUERY->doc_var(QUERY,
                   "Application is using a touch screen instead of a mouse.");
    QUERY->end_class(QUERY);

    QUERY->begin_class(QUERY, "UI_BackendFlags", "Object");
    QUERY->doc_class(QUERY,
                     "Backend capabilities flags stored in io.BackendFlags. "
                     "Set by imgui_impl_xxx or custom backend..\n");
    static t_CKINT ImGuiBackendFlags_None = 0;
    QUERY->add_svar(QUERY, "int", "None", true, &ImGuiBackendFlags_None);
    static t_CKINT ImGuiBackendFlags_HasGamepad = 1;
    QUERY->add_svar(QUERY, "int", "HasGamepad", true,
                    &ImGuiBackendFlags_HasGamepad);
    QUERY->doc_var(
      QUERY,
      "Backend Platform supports gamepad and currently has one connected.");
    static t_CKINT ImGuiBackendFlags_HasMouseCursors = 2;
    QUERY->add_svar(QUERY, "int", "HasMouseCursors", true,
                    &ImGuiBackendFlags_HasMouseCursors);
    QUERY->doc_var(QUERY,
                   "Backend Platform supports honoring GetMouseCursor() value "
                   "to change the OS cursor shape.");
    static t_CKINT ImGuiBackendFlags_HasSetMousePos = 4;
    QUERY->add_svar(QUERY, "int", "HasSetMousePos", true,
                    &ImGuiBackendFlags_HasSetMousePos);
    QUERY->doc_var(QUERY,
                   "Backend Platform supports io.WantSetMousePos requests to "
                   "reposition the OS mouse position (only used if "
                   "ImGuiConfigFlags_NavEnableSetMousePos is set).");
    static t_CKINT ImGuiBackendFlags_RendererHasVtxOffset = 8;
    QUERY->add_svar(QUERY, "int", "RendererHasVtxOffset", true,
                    &ImGuiBackendFlags_RendererHasVtxOffset);
    QUERY->doc_var(
      QUERY,
      "Backend Renderer supports ImDrawCmd::VtxOffset. This enables output of "
      "large meshes (64K+ vertices) while still using 16-bit indices.");
    static t_CKINT ImGuiBackendFlags_PlatformHasViewports = 1024;
    QUERY->add_svar(QUERY, "int", "PlatformHasViewports", true,
                    &ImGuiBackendFlags_PlatformHasViewports);
    QUERY->doc_var(QUERY, "Backend Platform supports multiple viewports.");
    static t_CKINT ImGuiBackendFlags_HasMouseHoveredViewport = 2048;
    QUERY->add_svar(QUERY, "int", "HasMouseHoveredViewport", true,
                    &ImGuiBackendFlags_HasMouseHoveredViewport);
    QUERY->doc_var(
      QUERY,
      "Backend Platform supports calling io.AddMouseViewportEvent() with the "
      "viewport under the mouse. IF POSSIBLE, ignore viewports with the "
      "ImGuiViewportFlags_NoInputs flag (Win32 backend, GLFW 3.30+ backend can "
      "do this, SDL backend cannot). If this cannot be done, Dear ImGui needs "
      "to use a flawed heuristic to find the viewport under.");
    static t_CKINT ImGuiBackendFlags_RendererHasViewports = 4096;
    QUERY->add_svar(QUERY, "int", "RendererHasViewports", true,
                    &ImGuiBackendFlags_RendererHasViewports);
    QUERY->doc_var(QUERY, "Backend Renderer supports multiple viewports.");
    QUERY->end_class(QUERY);

    QUERY->begin_class(QUERY, "UI_Col", "Object");
    QUERY->doc_class(QUERY,
                     "Enumeration for PushStyleColor() / PopStyleColor().\n");
    static t_CKINT ImGuiCol_Text = 0;
    QUERY->add_svar(QUERY, "int", "Text", true, &ImGuiCol_Text);
    static t_CKINT ImGuiCol_TextDisabled = 1;
    QUERY->add_svar(QUERY, "int", "TextDisabled", true, &ImGuiCol_TextDisabled);
    static t_CKINT ImGuiCol_WindowBg = 2;
    QUERY->add_svar(QUERY, "int", "WindowBg", true, &ImGuiCol_WindowBg);
    QUERY->doc_var(QUERY, "Background of normal windows");
    static t_CKINT ImGuiCol_ChildBg = 3;
    QUERY->add_svar(QUERY, "int", "ChildBg", true, &ImGuiCol_ChildBg);
    QUERY->doc_var(QUERY, "Background of child windows");
    static t_CKINT ImGuiCol_PopupBg = 4;
    QUERY->add_svar(QUERY, "int", "PopupBg", true, &ImGuiCol_PopupBg);
    QUERY->doc_var(QUERY, "Background of popups, menus, tooltips windows");
    static t_CKINT ImGuiCol_Border = 5;
    QUERY->add_svar(QUERY, "int", "Border", true, &ImGuiCol_Border);
    static t_CKINT ImGuiCol_BorderShadow = 6;
    QUERY->add_svar(QUERY, "int", "BorderShadow", true, &ImGuiCol_BorderShadow);
    static t_CKINT ImGuiCol_FrameBg = 7;
    QUERY->add_svar(QUERY, "int", "FrameBg", true, &ImGuiCol_FrameBg);
    QUERY->doc_var(
      QUERY, "Background of checkbox, radio button, plot, slider, text input");
    static t_CKINT ImGuiCol_FrameBgHovered = 8;
    QUERY->add_svar(QUERY, "int", "FrameBgHovered", true,
                    &ImGuiCol_FrameBgHovered);
    static t_CKINT ImGuiCol_FrameBgActive = 9;
    QUERY->add_svar(QUERY, "int", "FrameBgActive", true,
                    &ImGuiCol_FrameBgActive);
    static t_CKINT ImGuiCol_TitleBg = 10;
    QUERY->add_svar(QUERY, "int", "TitleBg", true, &ImGuiCol_TitleBg);
    QUERY->doc_var(QUERY, "Title bar");
    static t_CKINT ImGuiCol_TitleBgActive = 11;
    QUERY->add_svar(QUERY, "int", "TitleBgActive", true,
                    &ImGuiCol_TitleBgActive);
    QUERY->doc_var(QUERY, "Title bar when focused");
    static t_CKINT ImGuiCol_TitleBgCollapsed = 12;
    QUERY->add_svar(QUERY, "int", "TitleBgCollapsed", true,
                    &ImGuiCol_TitleBgCollapsed);
    QUERY->doc_var(QUERY, "Title bar when collapsed");
    static t_CKINT ImGuiCol_MenuBarBg = 13;
    QUERY->add_svar(QUERY, "int", "MenuBarBg", true, &ImGuiCol_MenuBarBg);
    static t_CKINT ImGuiCol_ScrollbarBg = 14;
    QUERY->add_svar(QUERY, "int", "ScrollbarBg", true, &ImGuiCol_ScrollbarBg);
    static t_CKINT ImGuiCol_ScrollbarGrab = 15;
    QUERY->add_svar(QUERY, "int", "ScrollbarGrab", true,
                    &ImGuiCol_ScrollbarGrab);
    static t_CKINT ImGuiCol_ScrollbarGrabHovered = 16;
    QUERY->add_svar(QUERY, "int", "ScrollbarGrabHovered", true,
                    &ImGuiCol_ScrollbarGrabHovered);
    static t_CKINT ImGuiCol_ScrollbarGrabActive = 17;
    QUERY->add_svar(QUERY, "int", "ScrollbarGrabActive", true,
                    &ImGuiCol_ScrollbarGrabActive);
    static t_CKINT ImGuiCol_CheckMark = 18;
    QUERY->add_svar(QUERY, "int", "CheckMark", true, &ImGuiCol_CheckMark);
    QUERY->doc_var(QUERY, "Checkbox tick and RadioButton circle");
    static t_CKINT ImGuiCol_SliderGrab = 19;
    QUERY->add_svar(QUERY, "int", "SliderGrab", true, &ImGuiCol_SliderGrab);
    static t_CKINT ImGuiCol_SliderGrabActive = 20;
    QUERY->add_svar(QUERY, "int", "SliderGrabActive", true,
                    &ImGuiCol_SliderGrabActive);
    static t_CKINT ImGuiCol_Button = 21;
    QUERY->add_svar(QUERY, "int", "Button", true, &ImGuiCol_Button);
    static t_CKINT ImGuiCol_ButtonHovered = 22;
    QUERY->add_svar(QUERY, "int", "ButtonHovered", true,
                    &ImGuiCol_ButtonHovered);
    static t_CKINT ImGuiCol_ButtonActive = 23;
    QUERY->add_svar(QUERY, "int", "ButtonActive", true, &ImGuiCol_ButtonActive);
    static t_CKINT ImGuiCol_Header = 24;
    QUERY->add_svar(QUERY, "int", "Header", true, &ImGuiCol_Header);
    QUERY->doc_var(QUERY,
                   "Header* colors are used for CollapsingHeader, TreeNode, "
                   "Selectable, MenuItem");
    static t_CKINT ImGuiCol_HeaderHovered = 25;
    QUERY->add_svar(QUERY, "int", "HeaderHovered", true,
                    &ImGuiCol_HeaderHovered);
    static t_CKINT ImGuiCol_HeaderActive = 26;
    QUERY->add_svar(QUERY, "int", "HeaderActive", true, &ImGuiCol_HeaderActive);
    static t_CKINT ImGuiCol_Separator = 27;
    QUERY->add_svar(QUERY, "int", "Separator", true, &ImGuiCol_Separator);
    static t_CKINT ImGuiCol_SeparatorHovered = 28;
    QUERY->add_svar(QUERY, "int", "SeparatorHovered", true,
                    &ImGuiCol_SeparatorHovered);
    static t_CKINT ImGuiCol_SeparatorActive = 29;
    QUERY->add_svar(QUERY, "int", "SeparatorActive", true,
                    &ImGuiCol_SeparatorActive);
    static t_CKINT ImGuiCol_ResizeGrip = 30;
    QUERY->add_svar(QUERY, "int", "ResizeGrip", true, &ImGuiCol_ResizeGrip);
    QUERY->doc_var(
      QUERY, "Resize grip in lower-right and lower-left corners of windows.");
    static t_CKINT ImGuiCol_ResizeGripHovered = 31;
    QUERY->add_svar(QUERY, "int", "ResizeGripHovered", true,
                    &ImGuiCol_ResizeGripHovered);
    static t_CKINT ImGuiCol_ResizeGripActive = 32;
    QUERY->add_svar(QUERY, "int", "ResizeGripActive", true,
                    &ImGuiCol_ResizeGripActive);
    static t_CKINT ImGuiCol_Tab = 33;
    QUERY->add_svar(QUERY, "int", "Tab", true, &ImGuiCol_Tab);
    QUERY->doc_var(QUERY, "TabItem in a TabBar");
    static t_CKINT ImGuiCol_TabHovered = 34;
    QUERY->add_svar(QUERY, "int", "TabHovered", true, &ImGuiCol_TabHovered);
    static t_CKINT ImGuiCol_TabActive = 35;
    QUERY->add_svar(QUERY, "int", "TabActive", true, &ImGuiCol_TabActive);
    static t_CKINT ImGuiCol_TabUnfocused = 36;
    QUERY->add_svar(QUERY, "int", "TabUnfocused", true, &ImGuiCol_TabUnfocused);
    static t_CKINT ImGuiCol_TabUnfocusedActive = 37;
    QUERY->add_svar(QUERY, "int", "TabUnfocusedActive", true,
                    &ImGuiCol_TabUnfocusedActive);
    static t_CKINT ImGuiCol_DockingPreview = 38;
    QUERY->add_svar(QUERY, "int", "DockingPreview", true,
                    &ImGuiCol_DockingPreview);
    QUERY->doc_var(QUERY,
                   "Preview overlay color when about to docking something");
    static t_CKINT ImGuiCol_DockingEmptyBg = 39;
    QUERY->add_svar(QUERY, "int", "DockingEmptyBg", true,
                    &ImGuiCol_DockingEmptyBg);
    QUERY->doc_var(QUERY,
                   "Background color for empty node (e.g. CentralNode with no "
                   "window docked into it)");
    static t_CKINT ImGuiCol_PlotLines = 40;
    QUERY->add_svar(QUERY, "int", "PlotLines", true, &ImGuiCol_PlotLines);
    static t_CKINT ImGuiCol_PlotLinesHovered = 41;
    QUERY->add_svar(QUERY, "int", "PlotLinesHovered", true,
                    &ImGuiCol_PlotLinesHovered);
    static t_CKINT ImGuiCol_PlotHistogram = 42;
    QUERY->add_svar(QUERY, "int", "PlotHistogram", true,
                    &ImGuiCol_PlotHistogram);
    static t_CKINT ImGuiCol_PlotHistogramHovered = 43;
    QUERY->add_svar(QUERY, "int", "PlotHistogramHovered", true,
                    &ImGuiCol_PlotHistogramHovered);
    static t_CKINT ImGuiCol_TableHeaderBg = 44;
    QUERY->add_svar(QUERY, "int", "TableHeaderBg", true,
                    &ImGuiCol_TableHeaderBg);
    QUERY->doc_var(QUERY, "Table header background");
    static t_CKINT ImGuiCol_TableBorderStrong = 45;
    QUERY->add_svar(QUERY, "int", "TableBorderStrong", true,
                    &ImGuiCol_TableBorderStrong);
    QUERY->doc_var(
      QUERY, "Table outer and header borders (prefer using Alpha=1.0 here)");
    static t_CKINT ImGuiCol_TableBorderLight = 46;
    QUERY->add_svar(QUERY, "int", "TableBorderLight", true,
                    &ImGuiCol_TableBorderLight);
    QUERY->doc_var(QUERY, "Table inner borders (prefer using Alpha=1.0 here)");
    static t_CKINT ImGuiCol_TableRowBg = 47;
    QUERY->add_svar(QUERY, "int", "TableRowBg", true, &ImGuiCol_TableRowBg);
    QUERY->doc_var(QUERY, "Table row background (even rows)");
    static t_CKINT ImGuiCol_TableRowBgAlt = 48;
    QUERY->add_svar(QUERY, "int", "TableRowBgAlt", true,
                    &ImGuiCol_TableRowBgAlt);
    QUERY->doc_var(QUERY, "Table row background (odd rows)");
    static t_CKINT ImGuiCol_TextSelectedBg = 49;
    QUERY->add_svar(QUERY, "int", "TextSelectedBg", true,
                    &ImGuiCol_TextSelectedBg);
    static t_CKINT ImGuiCol_DragDropTarget = 50;
    QUERY->add_svar(QUERY, "int", "DragDropTarget", true,
                    &ImGuiCol_DragDropTarget);
    QUERY->doc_var(QUERY, "Rectangle highlighting a drop target");
    static t_CKINT ImGuiCol_NavHighlight = 51;
    QUERY->add_svar(QUERY, "int", "NavHighlight", true, &ImGuiCol_NavHighlight);
    QUERY->doc_var(QUERY, "Gamepad/keyboard: current highlighted item");
    static t_CKINT ImGuiCol_NavWindowingHighlight = 52;
    QUERY->add_svar(QUERY, "int", "NavWindowingHighlight", true,
                    &ImGuiCol_NavWindowingHighlight);
    QUERY->doc_var(QUERY, "Highlight window when using CTRL+TAB");
    static t_CKINT ImGuiCol_NavWindowingDimBg = 53;
    QUERY->add_svar(QUERY, "int", "NavWindowingDimBg", true,
                    &ImGuiCol_NavWindowingDimBg);
    QUERY->doc_var(QUERY,
                   "Darken/colorize entire screen behind the CTRL+TAB window "
                   "list, when active");
    static t_CKINT ImGuiCol_ModalWindowDimBg = 54;
    QUERY->add_svar(QUERY, "int", "ModalWindowDimBg", true,
                    &ImGuiCol_ModalWindowDimBg);
    QUERY->doc_var(QUERY,
                   "Darken/colorize entire screen behind a modal window, when "
                   "one is active");
    static t_CKINT ImGuiCol_COUNT = 55;
    QUERY->add_svar(QUERY, "int", "COUNT", true, &ImGuiCol_COUNT);
    QUERY->end_class(QUERY);

    QUERY->begin_class(QUERY, "UI_StyleVar", "Object");
    QUERY->doc_class(
      QUERY,
      "Enumeration for PushStyleVar() / PopStyleVar() to temporarily modify "
      "the ImGuiStyle structure..\n- The enum only refers to fields of "
      "ImGuiStyle which makes sense to be pushed/popped inside UI "
      "code..\nDuring initialization or between frames, feel free to just poke "
      "into ImGuiStyle directly..\n- Tip: Use your programming IDE navigation "
      "facilities on the names in the _second column_ below to find the actual "
      "members and their description..\n- In Visual Studio: CTRL+comma "
      "(\"Edit.GoToAll\") can follow symbols inside comments, whereas CTRL+F12 "
      "(\"Edit.GoToImplementation\") cannot..\n- In Visual Studio w/ Visual "
      "Assist installed: ALT+G (\"VAssistX.GoToImplementation\") can also "
      "follow symbols inside comments..\n- In VS Code, CLion, etc.: CTRL+click "
      "can follow symbols inside comments..\n- When changing this enum, you "
      "need to update the associated internal table GStyleVarInfo[] "
      "accordingly. This is where we link enum values to members "
      "offset/type..\n");
    static t_CKINT ImGuiStyleVar_Alpha = 0;
    QUERY->add_svar(QUERY, "int", "Alpha", true, &ImGuiStyleVar_Alpha);
    QUERY->doc_var(QUERY, "float     Alpha");
    static t_CKINT ImGuiStyleVar_DisabledAlpha = 1;
    QUERY->add_svar(QUERY, "int", "DisabledAlpha", true,
                    &ImGuiStyleVar_DisabledAlpha);
    QUERY->doc_var(QUERY, "float     DisabledAlpha");
    static t_CKINT ImGuiStyleVar_WindowPadding = 2;
    QUERY->add_svar(QUERY, "int", "WindowPadding", true,
                    &ImGuiStyleVar_WindowPadding);
    QUERY->doc_var(QUERY, "ImVec2    WindowPadding");
    static t_CKINT ImGuiStyleVar_WindowRounding = 3;
    QUERY->add_svar(QUERY, "int", "WindowRounding", true,
                    &ImGuiStyleVar_WindowRounding);
    QUERY->doc_var(QUERY, "float     WindowRounding");
    static t_CKINT ImGuiStyleVar_WindowBorderSize = 4;
    QUERY->add_svar(QUERY, "int", "WindowBorderSize", true,
                    &ImGuiStyleVar_WindowBorderSize);
    QUERY->doc_var(QUERY, "float     WindowBorderSize");
    static t_CKINT ImGuiStyleVar_WindowMinSize = 5;
    QUERY->add_svar(QUERY, "int", "WindowMinSize", true,
                    &ImGuiStyleVar_WindowMinSize);
    QUERY->doc_var(QUERY, "ImVec2    WindowMinSize");
    static t_CKINT ImGuiStyleVar_WindowTitleAlign = 6;
    QUERY->add_svar(QUERY, "int", "WindowTitleAlign", true,
                    &ImGuiStyleVar_WindowTitleAlign);
    QUERY->doc_var(QUERY, "ImVec2    WindowTitleAlign");
    static t_CKINT ImGuiStyleVar_ChildRounding = 7;
    QUERY->add_svar(QUERY, "int", "ChildRounding", true,
                    &ImGuiStyleVar_ChildRounding);
    QUERY->doc_var(QUERY, "float     ChildRounding");
    static t_CKINT ImGuiStyleVar_ChildBorderSize = 8;
    QUERY->add_svar(QUERY, "int", "ChildBorderSize", true,
                    &ImGuiStyleVar_ChildBorderSize);
    QUERY->doc_var(QUERY, "float     ChildBorderSize");
    static t_CKINT ImGuiStyleVar_PopupRounding = 9;
    QUERY->add_svar(QUERY, "int", "PopupRounding", true,
                    &ImGuiStyleVar_PopupRounding);
    QUERY->doc_var(QUERY, "float     PopupRounding");
    static t_CKINT ImGuiStyleVar_PopupBorderSize = 10;
    QUERY->add_svar(QUERY, "int", "PopupBorderSize", true,
                    &ImGuiStyleVar_PopupBorderSize);
    QUERY->doc_var(QUERY, "float     PopupBorderSize");
    static t_CKINT ImGuiStyleVar_FramePadding = 11;
    QUERY->add_svar(QUERY, "int", "FramePadding", true,
                    &ImGuiStyleVar_FramePadding);
    QUERY->doc_var(QUERY, "ImVec2    FramePadding");
    static t_CKINT ImGuiStyleVar_FrameRounding = 12;
    QUERY->add_svar(QUERY, "int", "FrameRounding", true,
                    &ImGuiStyleVar_FrameRounding);
    QUERY->doc_var(QUERY, "float     FrameRounding");
    static t_CKINT ImGuiStyleVar_FrameBorderSize = 13;
    QUERY->add_svar(QUERY, "int", "FrameBorderSize", true,
                    &ImGuiStyleVar_FrameBorderSize);
    QUERY->doc_var(QUERY, "float     FrameBorderSize");
    static t_CKINT ImGuiStyleVar_ItemSpacing = 14;
    QUERY->add_svar(QUERY, "int", "ItemSpacing", true,
                    &ImGuiStyleVar_ItemSpacing);
    QUERY->doc_var(QUERY, "ImVec2    ItemSpacing");
    static t_CKINT ImGuiStyleVar_ItemInnerSpacing = 15;
    QUERY->add_svar(QUERY, "int", "ItemInnerSpacing", true,
                    &ImGuiStyleVar_ItemInnerSpacing);
    QUERY->doc_var(QUERY, "ImVec2    ItemInnerSpacing");
    static t_CKINT ImGuiStyleVar_IndentSpacing = 16;
    QUERY->add_svar(QUERY, "int", "IndentSpacing", true,
                    &ImGuiStyleVar_IndentSpacing);
    QUERY->doc_var(QUERY, "float     IndentSpacing");
    static t_CKINT ImGuiStyleVar_CellPadding = 17;
    QUERY->add_svar(QUERY, "int", "CellPadding", true,
                    &ImGuiStyleVar_CellPadding);
    QUERY->doc_var(QUERY, "ImVec2    CellPadding");
    static t_CKINT ImGuiStyleVar_ScrollbarSize = 18;
    QUERY->add_svar(QUERY, "int", "ScrollbarSize", true,
                    &ImGuiStyleVar_ScrollbarSize);
    QUERY->doc_var(QUERY, "float     ScrollbarSize");
    static t_CKINT ImGuiStyleVar_ScrollbarRounding = 19;
    QUERY->add_svar(QUERY, "int", "ScrollbarRounding", true,
                    &ImGuiStyleVar_ScrollbarRounding);
    QUERY->doc_var(QUERY, "float     ScrollbarRounding");
    static t_CKINT ImGuiStyleVar_GrabMinSize = 20;
    QUERY->add_svar(QUERY, "int", "GrabMinSize", true,
                    &ImGuiStyleVar_GrabMinSize);
    QUERY->doc_var(QUERY, "float     GrabMinSize");
    static t_CKINT ImGuiStyleVar_GrabRounding = 21;
    QUERY->add_svar(QUERY, "int", "GrabRounding", true,
                    &ImGuiStyleVar_GrabRounding);
    QUERY->doc_var(QUERY, "float     GrabRounding");
    static t_CKINT ImGuiStyleVar_TabRounding = 22;
    QUERY->add_svar(QUERY, "int", "TabRounding", true,
                    &ImGuiStyleVar_TabRounding);
    QUERY->doc_var(QUERY, "float     TabRounding");
    static t_CKINT ImGuiStyleVar_TabBorderSize = 23;
    QUERY->add_svar(QUERY, "int", "TabBorderSize", true,
                    &ImGuiStyleVar_TabBorderSize);
    QUERY->doc_var(QUERY, "float     TabBorderSize");
    static t_CKINT ImGuiStyleVar_TabBarBorderSize = 24;
    QUERY->add_svar(QUERY, "int", "TabBarBorderSize", true,
                    &ImGuiStyleVar_TabBarBorderSize);
    QUERY->doc_var(QUERY, "float     TabBarBorderSize");
    static t_CKINT ImGuiStyleVar_TableAngledHeadersAngle = 25;
    QUERY->add_svar(QUERY, "int", "TableAngledHeadersAngle", true,
                    &ImGuiStyleVar_TableAngledHeadersAngle);
    QUERY->doc_var(QUERY, "float     TableAngledHeadersAngle");
    static t_CKINT ImGuiStyleVar_TableAngledHeadersTextAlign = 26;
    QUERY->add_svar(QUERY, "int", "TableAngledHeadersTextAlign", true,
                    &ImGuiStyleVar_TableAngledHeadersTextAlign);
    QUERY->doc_var(QUERY, "ImVec2  TableAngledHeadersTextAlign");
    static t_CKINT ImGuiStyleVar_ButtonTextAlign = 27;
    QUERY->add_svar(QUERY, "int", "ButtonTextAlign", true,
                    &ImGuiStyleVar_ButtonTextAlign);
    QUERY->doc_var(QUERY, "ImVec2    ButtonTextAlign");
    static t_CKINT ImGuiStyleVar_SelectableTextAlign = 28;
    QUERY->add_svar(QUERY, "int", "SelectableTextAlign", true,
                    &ImGuiStyleVar_SelectableTextAlign);
    QUERY->doc_var(QUERY, "ImVec2    SelectableTextAlign");
    static t_CKINT ImGuiStyleVar_SeparatorTextBorderSize = 29;
    QUERY->add_svar(QUERY, "int", "SeparatorTextBorderSize", true,
                    &ImGuiStyleVar_SeparatorTextBorderSize);
    QUERY->doc_var(QUERY, "float     SeparatorTextBorderSize");
    static t_CKINT ImGuiStyleVar_SeparatorTextAlign = 30;
    QUERY->add_svar(QUERY, "int", "SeparatorTextAlign", true,
                    &ImGuiStyleVar_SeparatorTextAlign);
    QUERY->doc_var(QUERY, "ImVec2    SeparatorTextAlign");
    static t_CKINT ImGuiStyleVar_SeparatorTextPadding = 31;
    QUERY->add_svar(QUERY, "int", "SeparatorTextPadding", true,
                    &ImGuiStyleVar_SeparatorTextPadding);
    QUERY->doc_var(QUERY, "ImVec2    SeparatorTextPadding");
    static t_CKINT ImGuiStyleVar_DockingSeparatorSize = 32;
    QUERY->add_svar(QUERY, "int", "DockingSeparatorSize", true,
                    &ImGuiStyleVar_DockingSeparatorSize);
    QUERY->doc_var(QUERY, "float     DockingSeparatorSize");
    static t_CKINT ImGuiStyleVar_COUNT = 33;
    QUERY->add_svar(QUERY, "int", "COUNT", true, &ImGuiStyleVar_COUNT);
    QUERY->end_class(QUERY);

    QUERY->begin_class(QUERY, "UI_ButtonFlags", "Object");
    QUERY->doc_class(
      QUERY, "Flags for InvisibleButton() [extended in imgui_internal.h].\n");
    static t_CKINT ImGuiButtonFlags_None = 0;
    QUERY->add_svar(QUERY, "int", "None", true, &ImGuiButtonFlags_None);
    static t_CKINT ImGuiButtonFlags_MouseButtonLeft = 1;
    QUERY->add_svar(QUERY, "int", "MouseButtonLeft", true,
                    &ImGuiButtonFlags_MouseButtonLeft);
    QUERY->doc_var(QUERY, "React on left mouse button (default)");
    static t_CKINT ImGuiButtonFlags_MouseButtonRight = 2;
    QUERY->add_svar(QUERY, "int", "MouseButtonRight", true,
                    &ImGuiButtonFlags_MouseButtonRight);
    QUERY->doc_var(QUERY, "React on right mouse button");
    static t_CKINT ImGuiButtonFlags_MouseButtonMiddle = 4;
    QUERY->add_svar(QUERY, "int", "MouseButtonMiddle", true,
                    &ImGuiButtonFlags_MouseButtonMiddle);
    QUERY->doc_var(QUERY, "React on center mouse button");
    QUERY->end_class(QUERY);

    QUERY->begin_class(QUERY, "UI_ColorEditFlags", "Object");
    QUERY->doc_class(QUERY,
                     "Flags for ColorEdit3() / ColorEdit4() / ColorPicker3() / "
                     "ColorPicker4() / ColorButton().\n");
    static t_CKINT ImGuiColorEditFlags_None = 0;
    QUERY->add_svar(QUERY, "int", "None", true, &ImGuiColorEditFlags_None);
    static t_CKINT ImGuiColorEditFlags_NoAlpha = 2;
    QUERY->add_svar(QUERY, "int", "NoAlpha", true,
                    &ImGuiColorEditFlags_NoAlpha);
    QUERY->doc_var(
      QUERY,
      "ColorEdit, ColorPicker, ColorButton: ignore Alpha component (will only "
      "read 3 components from the input pointer).");
    static t_CKINT ImGuiColorEditFlags_NoPicker = 4;
    QUERY->add_svar(QUERY, "int", "NoPicker", true,
                    &ImGuiColorEditFlags_NoPicker);
    QUERY->doc_var(QUERY,
                   "ColorEdit: disable picker when clicking on color square.");
    static t_CKINT ImGuiColorEditFlags_NoOptions = 8;
    QUERY->add_svar(QUERY, "int", "NoOptions", true,
                    &ImGuiColorEditFlags_NoOptions);
    QUERY->doc_var(QUERY,
                   "ColorEdit: disable toggling options menu when "
                   "right-clicking on inputs/small preview.");
    static t_CKINT ImGuiColorEditFlags_NoSmallPreview = 16;
    QUERY->add_svar(QUERY, "int", "NoSmallPreview", true,
                    &ImGuiColorEditFlags_NoSmallPreview);
    QUERY->doc_var(QUERY,
                   "ColorEdit, ColorPicker: disable color square preview next "
                   "to the inputs. (e.g. to show only the inputs)");
    static t_CKINT ImGuiColorEditFlags_NoInputs = 32;
    QUERY->add_svar(QUERY, "int", "NoInputs", true,
                    &ImGuiColorEditFlags_NoInputs);
    QUERY->doc_var(
      QUERY,
      "ColorEdit, ColorPicker: disable inputs sliders/text widgets (e.g. to "
      "show only the small preview color square).");
    static t_CKINT ImGuiColorEditFlags_NoTooltip = 64;
    QUERY->add_svar(QUERY, "int", "NoTooltip", true,
                    &ImGuiColorEditFlags_NoTooltip);
    QUERY->doc_var(QUERY,
                   "ColorEdit, ColorPicker, ColorButton: disable tooltip when "
                   "hovering the preview.");
    static t_CKINT ImGuiColorEditFlags_NoLabel = 128;
    QUERY->add_svar(QUERY, "int", "NoLabel", true,
                    &ImGuiColorEditFlags_NoLabel);
    QUERY->doc_var(
      QUERY,
      "ColorEdit, ColorPicker: disable display of inline text label (the label "
      "is still forwarded to the tooltip and picker).");
    static t_CKINT ImGuiColorEditFlags_NoSidePreview = 256;
    QUERY->add_svar(QUERY, "int", "NoSidePreview", true,
                    &ImGuiColorEditFlags_NoSidePreview);
    QUERY->doc_var(QUERY,
                   "ColorPicker: disable bigger color preview on right side of "
                   "the picker, use small color square preview instead.");
    static t_CKINT ImGuiColorEditFlags_NoDragDrop = 512;
    QUERY->add_svar(QUERY, "int", "NoDragDrop", true,
                    &ImGuiColorEditFlags_NoDragDrop);
    QUERY->doc_var(QUERY,
                   "ColorEdit: disable drag and drop target. ColorButton: "
                   "disable drag and drop source.");
    static t_CKINT ImGuiColorEditFlags_NoBorder = 1024;
    QUERY->add_svar(QUERY, "int", "NoBorder", true,
                    &ImGuiColorEditFlags_NoBorder);
    QUERY->doc_var(
      QUERY, "ColorButton: disable border (which is enforced by default)");
    static t_CKINT ImGuiColorEditFlags_AlphaBar = 65536;
    QUERY->add_svar(QUERY, "int", "AlphaBar", true,
                    &ImGuiColorEditFlags_AlphaBar);
    QUERY->doc_var(
      QUERY,
      "ColorEdit, ColorPicker: show vertical alpha bar/gradient in picker.");
    static t_CKINT ImGuiColorEditFlags_AlphaPreview = 131072;
    QUERY->add_svar(QUERY, "int", "AlphaPreview", true,
                    &ImGuiColorEditFlags_AlphaPreview);
    QUERY->doc_var(QUERY,
                   "ColorEdit, ColorPicker, ColorButton: display preview as a "
                   "transparent color over a checkerboard, instead of opaque.");
    static t_CKINT ImGuiColorEditFlags_AlphaPreviewHalf = 262144;
    QUERY->add_svar(QUERY, "int", "AlphaPreviewHalf", true,
                    &ImGuiColorEditFlags_AlphaPreviewHalf);
    QUERY->doc_var(QUERY,
                   "ColorEdit, ColorPicker, ColorButton: display half opaque / "
                   "half checkerboard, instead of opaque.");
    static t_CKINT ImGuiColorEditFlags_HDR = 524288;
    QUERY->add_svar(QUERY, "int", "HDR", true, &ImGuiColorEditFlags_HDR);
    QUERY->doc_var(QUERY,
                   "(WIP) ColorEdit: Currently only disable 0.0f..1.0f limits "
                   "in RGBA edition (note: you probably want to use "
                   "ImGuiColorEditFlags_Float flag as well).");
    static t_CKINT ImGuiColorEditFlags_DisplayRGB = 1048576;
    QUERY->add_svar(QUERY, "int", "DisplayRGB", true,
                    &ImGuiColorEditFlags_DisplayRGB);
    QUERY->doc_var(
      QUERY,
      "[Display]     ColorEdit: override _display_ type among RGB/HSV/Hex. "
      "ColorPicker: select any combination using one or more of RGB/HSV/Hex.");
    static t_CKINT ImGuiColorEditFlags_DisplayHSV = 2097152;
    QUERY->add_svar(QUERY, "int", "DisplayHSV", true,
                    &ImGuiColorEditFlags_DisplayHSV);
    QUERY->doc_var(QUERY, "[Display]     \"");
    static t_CKINT ImGuiColorEditFlags_DisplayHex = 4194304;
    QUERY->add_svar(QUERY, "int", "DisplayHex", true,
                    &ImGuiColorEditFlags_DisplayHex);
    QUERY->doc_var(QUERY, "[Display]     \"");
    static t_CKINT ImGuiColorEditFlags_Uint8 = 8388608;
    QUERY->add_svar(QUERY, "int", "Uint8", true, &ImGuiColorEditFlags_Uint8);
    QUERY->doc_var(QUERY,
                   "[DataType]    ColorEdit, ColorPicker, ColorButton: "
                   "_display_ values formatted as 0..255.");
    static t_CKINT ImGuiColorEditFlags_Float = 16777216;
    QUERY->add_svar(QUERY, "int", "Float", true, &ImGuiColorEditFlags_Float);
    QUERY->doc_var(QUERY,
                   "[DataType]    ColorEdit, ColorPicker, ColorButton: "
                   "_display_ values formatted as 0.0f..1.0f floats instead of "
                   "0..255 integers. No round-trip of value via integers.");
    static t_CKINT ImGuiColorEditFlags_PickerHueBar = 33554432;
    QUERY->add_svar(QUERY, "int", "PickerHueBar", true,
                    &ImGuiColorEditFlags_PickerHueBar);
    QUERY->doc_var(
      QUERY,
      "[Picker]      ColorPicker: bar for Hue, rectangle for Sat/Value.");
    static t_CKINT ImGuiColorEditFlags_PickerHueWheel = 67108864;
    QUERY->add_svar(QUERY, "int", "PickerHueWheel", true,
                    &ImGuiColorEditFlags_PickerHueWheel);
    QUERY->doc_var(
      QUERY,
      "[Picker]      ColorPicker: wheel for Hue, triangle for Sat/Value.");
    static t_CKINT ImGuiColorEditFlags_InputRGB = 134217728;
    QUERY->add_svar(QUERY, "int", "InputRGB", true,
                    &ImGuiColorEditFlags_InputRGB);
    QUERY->doc_var(QUERY,
                   "[Input]       ColorEdit, ColorPicker: input and output "
                   "data in RGB format.");
    static t_CKINT ImGuiColorEditFlags_InputHSV = 268435456;
    QUERY->add_svar(QUERY, "int", "InputHSV", true,
                    &ImGuiColorEditFlags_InputHSV);
    QUERY->doc_var(QUERY,
                   "[Input]       ColorEdit, ColorPicker: input and output "
                   "data in HSV format.");
    QUERY->end_class(QUERY);

    QUERY->begin_class(QUERY, "UI_SliderFlags", "Object");
    QUERY->doc_class(
      QUERY,
      "Flags for DragFloat(), DragInt(), SliderFloat(), SliderInt() etc..\nWe "
      "use the same sets of flags for DragXXX() and SliderXXX() functions as "
      "the features are the same and it makes it easier to swap them..\n(Those "
      "are per-item flags. There are shared flags in ImGuiIO: "
      "io.ConfigDragClickToInputText).\n");
    static t_CKINT ImGuiSliderFlags_None = 0;
    QUERY->add_svar(QUERY, "int", "None", true, &ImGuiSliderFlags_None);
    static t_CKINT ImGuiSliderFlags_AlwaysClamp = 16;
    QUERY->add_svar(QUERY, "int", "AlwaysClamp", true,
                    &ImGuiSliderFlags_AlwaysClamp);
    QUERY->doc_var(
      QUERY,
      "Clamp value to min/max bounds when input manually with CTRL+Click. By "
      "default CTRL+Click allows going out of bounds.");
    static t_CKINT ImGuiSliderFlags_Logarithmic = 32;
    QUERY->add_svar(QUERY, "int", "Logarithmic", true,
                    &ImGuiSliderFlags_Logarithmic);
    QUERY->doc_var(QUERY,
                   "Make the widget logarithmic (linear otherwise). Consider "
                   "using ImGuiSliderFlags_NoRoundToFormat with this if using "
                   "a format-string with small amount of digits.");
    static t_CKINT ImGuiSliderFlags_NoRoundToFormat = 64;
    QUERY->add_svar(QUERY, "int", "NoRoundToFormat", true,
                    &ImGuiSliderFlags_NoRoundToFormat);
    QUERY->doc_var(
      QUERY,
      "Disable rounding underlying value to match precision of the display "
      "format string (e.g. %.3f values are rounded to those 3 digits)");
    static t_CKINT ImGuiSliderFlags_NoInput = 128;
    QUERY->add_svar(QUERY, "int", "NoInput", true, &ImGuiSliderFlags_NoInput);
    QUERY->doc_var(QUERY,
                   "Disable CTRL+Click or Enter key allowing to input text "
                   "directly into the widget");
    QUERY->end_class(QUERY);

    QUERY->begin_class(QUERY, "UI_MouseButton", "Object");
    QUERY->doc_class(QUERY,
                     "Identify a mouse button..\nThose values are guaranteed "
                     "to be stable and we frequently use 0/1 directly. Named "
                     "enums provided for convenience..\n");
    static t_CKINT ImGuiMouseButton_Left = 0;
    QUERY->add_svar(QUERY, "int", "Left", true, &ImGuiMouseButton_Left);
    static t_CKINT ImGuiMouseButton_Right = 1;
    QUERY->add_svar(QUERY, "int", "Right", true, &ImGuiMouseButton_Right);
    static t_CKINT ImGuiMouseButton_Middle = 2;
    QUERY->add_svar(QUERY, "int", "Middle", true, &ImGuiMouseButton_Middle);
    static t_CKINT ImGuiMouseButton_COUNT = 5;
    QUERY->add_svar(QUERY, "int", "COUNT", true, &ImGuiMouseButton_COUNT);
    QUERY->end_class(QUERY);

    QUERY->begin_class(QUERY, "UI_MouseCursor", "Object");
    QUERY->doc_class(
      QUERY,
      "Enumeration for GetMouseCursor().\nUser code may request backend to "
      "display given cursor by calling SetMouseCursor(), which is why we have "
      "some cursors that are marked unused here.\n");
    static t_CKINT ImGuiMouseCursor_None = -1;
    QUERY->add_svar(QUERY, "int", "None", true, &ImGuiMouseCursor_None);
    static t_CKINT ImGuiMouseCursor_Arrow = 0;
    QUERY->add_svar(QUERY, "int", "Arrow", true, &ImGuiMouseCursor_Arrow);
    static t_CKINT ImGuiMouseCursor_TextInput = 1;
    QUERY->add_svar(QUERY, "int", "TextInput", true,
                    &ImGuiMouseCursor_TextInput);
    QUERY->doc_var(QUERY, "When hovering over InputText, etc.");
    static t_CKINT ImGuiMouseCursor_ResizeAll = 2;
    QUERY->add_svar(QUERY, "int", "ResizeAll", true,
                    &ImGuiMouseCursor_ResizeAll);
    QUERY->doc_var(QUERY, "(Unused by Dear ImGui functions)");
    static t_CKINT ImGuiMouseCursor_ResizeNS = 3;
    QUERY->add_svar(QUERY, "int", "ResizeNS", true, &ImGuiMouseCursor_ResizeNS);
    QUERY->doc_var(QUERY, "When hovering over a horizontal border");
    static t_CKINT ImGuiMouseCursor_ResizeEW = 4;
    QUERY->add_svar(QUERY, "int", "ResizeEW", true, &ImGuiMouseCursor_ResizeEW);
    QUERY->doc_var(QUERY, "When hovering over a vertical border or a column");
    static t_CKINT ImGuiMouseCursor_ResizeNESW = 5;
    QUERY->add_svar(QUERY, "int", "ResizeNESW", true,
                    &ImGuiMouseCursor_ResizeNESW);
    QUERY->doc_var(QUERY,
                   "When hovering over the bottom-left corner of a window");
    static t_CKINT ImGuiMouseCursor_ResizeNWSE = 6;
    QUERY->add_svar(QUERY, "int", "ResizeNWSE", true,
                    &ImGuiMouseCursor_ResizeNWSE);
    QUERY->doc_var(QUERY,
                   "When hovering over the bottom-right corner of a window");
    static t_CKINT ImGuiMouseCursor_Hand = 7;
    QUERY->add_svar(QUERY, "int", "Hand", true, &ImGuiMouseCursor_Hand);
    QUERY->doc_var(QUERY,
                   "(Unused by Dear ImGui functions. Use for e.g. hyperlinks)");
    static t_CKINT ImGuiMouseCursor_NotAllowed = 8;
    QUERY->add_svar(QUERY, "int", "NotAllowed", true,
                    &ImGuiMouseCursor_NotAllowed);
    QUERY->doc_var(QUERY,
                   "When hovering something with disallowed interaction. "
                   "Usually a crossed circle.");
    static t_CKINT ImGuiMouseCursor_COUNT = 9;
    QUERY->add_svar(QUERY, "int", "COUNT", true, &ImGuiMouseCursor_COUNT);
    QUERY->end_class(QUERY);

    QUERY->begin_class(QUERY, "UI_MouseSource", "Object");
    QUERY->doc_class(
      QUERY,
      "Enumeration for AddMouseSourceEvent() actual source of Mouse Input "
      "data..\nHistorically we use \"Mouse\" terminology everywhere to "
      "indicate pointer data, e.g. MousePos, IsMousePressed(), "
      "io.AddMousePosEvent().\nBut that \"Mouse\" data can come from different "
      "source which occasionally may be useful for application to know "
      "about..\nYou can submit a change of pointer type using "
      "io.AddMouseSourceEvent()..\nForward declared enum type "
      "ImGuiMouseSource");
    static t_CKINT ImGuiMouseSource_Mouse = 0;
    QUERY->add_svar(QUERY, "int", "Mouse", true, &ImGuiMouseSource_Mouse);
    QUERY->doc_var(QUERY, "Input is coming from an actual mouse.");
    static t_CKINT ImGuiMouseSource_TouchScreen = 1;
    QUERY->add_svar(QUERY, "int", "TouchScreen", true,
                    &ImGuiMouseSource_TouchScreen);
    QUERY->doc_var(QUERY,
                   "Input is coming from a touch screen (no hovering prior to "
                   "initial press, less precise initial press aiming, "
                   "dual-axis wheeling possible).");
    static t_CKINT ImGuiMouseSource_Pen = 2;
    QUERY->add_svar(QUERY, "int", "Pen", true, &ImGuiMouseSource_Pen);
    QUERY->doc_var(QUERY,
                   "Input is coming from a pressure/magnetic pen (often used "
                   "in conjunction with high-sampling rates).");
    static t_CKINT ImGuiMouseSource_COUNT = 3;
    QUERY->add_svar(QUERY, "int", "COUNT", true, &ImGuiMouseSource_COUNT);
    QUERY->end_class(QUERY);

    QUERY->begin_class(QUERY, "UI_Cond", "Object");
    QUERY->doc_class(
      QUERY,
      "Enumeration for ImGui::SetNextWindow***(), SetWindow***(), "
      "SetNextItem***() functions.\nRepresent a condition..\nImportant: Treat "
      "as a regular enum! Do NOT combine multiple values using binary "
      "operators! All the functions above treat 0 as a shortcut to "
      "ImGuiCond_Always..\n");
    static t_CKINT ImGuiCond_None = 0;
    QUERY->add_svar(QUERY, "int", "None", true, &ImGuiCond_None);
    QUERY->doc_var(QUERY,
                   "No condition (always set the variable), same as _Always");
    static t_CKINT ImGuiCond_Always = 1;
    QUERY->add_svar(QUERY, "int", "Always", true, &ImGuiCond_Always);
    QUERY->doc_var(QUERY,
                   "No condition (always set the variable), same as _None");
    static t_CKINT ImGuiCond_Once = 2;
    QUERY->add_svar(QUERY, "int", "Once", true, &ImGuiCond_Once);
    QUERY->doc_var(QUERY,
                   "Set the variable once per runtime session (only the first "
                   "call will succeed)");
    static t_CKINT ImGuiCond_FirstUseEver = 4;
    QUERY->add_svar(QUERY, "int", "FirstUseEver", true,
                    &ImGuiCond_FirstUseEver);
    QUERY->doc_var(QUERY,
                   "Set the variable if the object/window has no persistently "
                   "saved data (no entry in .ini file)");
    static t_CKINT ImGuiCond_Appearing = 8;
    QUERY->add_svar(QUERY, "int", "Appearing", true, &ImGuiCond_Appearing);
    QUERY->doc_var(QUERY,
                   "Set the variable if the object/window is appearing after "
                   "being hidden/inactive (or the first time)");
    QUERY->end_class(QUERY);

    QUERY->begin_class(QUERY, "UI_TableFlags", "Object");
    QUERY->doc_class(
      QUERY,
      "Flags for ImGui::BeginTable().\n- Important! Sizing policies have "
      "complex and subtle side effects, much more so than you would "
      "expect..\nRead comments/demos carefully + experiment with live demos to "
      "get acquainted with them..\n- The DEFAULT sizing policies are:.\n- "
      "Default to ImGuiTableFlags_SizingFixedFit    if ScrollX is on, or if "
      "host window has ImGuiWindowFlags_AlwaysAutoResize..\n- Default to "
      "ImGuiTableFlags_SizingStretchSame if ScrollX is off..\n- When ScrollX "
      "is off:.\n- Table defaults to ImGuiTableFlags_SizingStretchSame -> all "
      "Columns defaults to ImGuiTableColumnFlags_WidthStretch with same "
      "weight..\n- Columns sizing policy allowed: Stretch (default), "
      "Fixed/Auto..\n- Fixed Columns (if any) will generally obtain their "
      "requested width (unless the table cannot fit them all)..\n- Stretch "
      "Columns will share the remaining width according to their respective "
      "weight..\n- Mixed Fixed/Stretch columns is possible but has various "
      "side-effects on resizing behaviors..\nThe typical use of mixing sizing "
      "policies is: any number of LEADING Fixed columns, followed by one or "
      "two TRAILING Stretch columns..\n(this is because the visible order of "
      "columns have subtle but necessary effects on how they react to manual "
      "resizing)..\n- When ScrollX is on:.\n- Table defaults to "
      "ImGuiTableFlags_SizingFixedFit -> all Columns defaults to "
      "ImGuiTableColumnFlags_WidthFixed.\n- Columns sizing policy allowed: "
      "Fixed/Auto mostly..\n- Fixed Columns can be enlarged as needed. Table "
      "will show a horizontal scrollbar if needed..\n- When using "
      "auto-resizing (non-resizable) fixed columns, querying the content width "
      "to use item right-alignment e.g. SetNextItemWidth(-FLT_MIN) doesn't "
      "make sense, would create a feedback loop..\n- Using Stretch columns "
      "OFTEN DOES NOT MAKE SENSE if ScrollX is on, UNLESS you have specified a "
      "value for 'inner_width' in BeginTable()..\nIf you specify a value for "
      "'inner_width' then effectively the scrolling space is known and Stretch "
      "or mixed Fixed/Stretch columns become meaningful again..\n- Read on "
      "documentation at the top of imgui_tables.cpp for details..\n");
    static t_CKINT ImGuiTableFlags_None = 0;
    QUERY->add_svar(QUERY, "int", "None", true, &ImGuiTableFlags_None);
    static t_CKINT ImGuiTableFlags_Resizable = 1;
    QUERY->add_svar(QUERY, "int", "Resizable", true,
                    &ImGuiTableFlags_Resizable);
    QUERY->doc_var(QUERY, "Enable resizing columns.");
    static t_CKINT ImGuiTableFlags_Reorderable = 2;
    QUERY->add_svar(QUERY, "int", "Reorderable", true,
                    &ImGuiTableFlags_Reorderable);
    QUERY->doc_var(
      QUERY,
      "Enable reordering columns in header row (need calling "
      "TableSetupColumn() + TableHeadersRow() to display headers)");
    static t_CKINT ImGuiTableFlags_Hideable = 4;
    QUERY->add_svar(QUERY, "int", "Hideable", true, &ImGuiTableFlags_Hideable);
    QUERY->doc_var(QUERY, "Enable hiding/disabling columns in context menu.");
    static t_CKINT ImGuiTableFlags_Sortable = 8;
    QUERY->add_svar(QUERY, "int", "Sortable", true, &ImGuiTableFlags_Sortable);
    QUERY->doc_var(
      QUERY,
      "Enable sorting. Call TableGetSortSpecs() to obtain sort specs. Also see "
      "ImGuiTableFlags_SortMulti and ImGuiTableFlags_SortTristate.");
    static t_CKINT ImGuiTableFlags_NoSavedSettings = 16;
    QUERY->add_svar(QUERY, "int", "NoSavedSettings", true,
                    &ImGuiTableFlags_NoSavedSettings);
    QUERY->doc_var(QUERY,
                   "Disable persisting columns order, width and sort settings "
                   "in the .ini file.");
    static t_CKINT ImGuiTableFlags_ContextMenuInBody = 32;
    QUERY->add_svar(QUERY, "int", "ContextMenuInBody", true,
                    &ImGuiTableFlags_ContextMenuInBody);
    QUERY->doc_var(
      QUERY,
      "Right-click on columns body/contents will display table context menu. "
      "By default it is available in TableHeadersRow().");
    static t_CKINT ImGuiTableFlags_RowBg = 64;
    QUERY->add_svar(QUERY, "int", "RowBg", true, &ImGuiTableFlags_RowBg);
    QUERY->doc_var(
      QUERY,
      "Set each RowBg color with ImGuiCol_TableRowBg or ImGuiCol_TableRowBgAlt "
      "(equivalent of calling TableSetBgColor with ImGuiTableBgFlags_RowBg0 on "
      "each row manually)");
    static t_CKINT ImGuiTableFlags_BordersInnerH = 128;
    QUERY->add_svar(QUERY, "int", "BordersInnerH", true,
                    &ImGuiTableFlags_BordersInnerH);
    QUERY->doc_var(QUERY, "Draw horizontal borders between rows.");
    static t_CKINT ImGuiTableFlags_BordersOuterH = 256;
    QUERY->add_svar(QUERY, "int", "BordersOuterH", true,
                    &ImGuiTableFlags_BordersOuterH);
    QUERY->doc_var(QUERY, "Draw horizontal borders at the top and bottom.");
    static t_CKINT ImGuiTableFlags_BordersInnerV = 512;
    QUERY->add_svar(QUERY, "int", "BordersInnerV", true,
                    &ImGuiTableFlags_BordersInnerV);
    QUERY->doc_var(QUERY, "Draw vertical borders between columns.");
    static t_CKINT ImGuiTableFlags_BordersOuterV = 1024;
    QUERY->add_svar(QUERY, "int", "BordersOuterV", true,
                    &ImGuiTableFlags_BordersOuterV);
    QUERY->doc_var(QUERY, "Draw vertical borders on the left and right sides.");
    static t_CKINT ImGuiTableFlags_BordersH = 384;
    QUERY->add_svar(QUERY, "int", "BordersH", true, &ImGuiTableFlags_BordersH);
    QUERY->doc_var(QUERY, "Draw horizontal borders.");
    static t_CKINT ImGuiTableFlags_BordersV = 1536;
    QUERY->add_svar(QUERY, "int", "BordersV", true, &ImGuiTableFlags_BordersV);
    QUERY->doc_var(QUERY, "Draw vertical borders.");
    static t_CKINT ImGuiTableFlags_BordersInner = 640;
    QUERY->add_svar(QUERY, "int", "BordersInner", true,
                    &ImGuiTableFlags_BordersInner);
    QUERY->doc_var(QUERY, "Draw inner borders.");
    static t_CKINT ImGuiTableFlags_BordersOuter = 1280;
    QUERY->add_svar(QUERY, "int", "BordersOuter", true,
                    &ImGuiTableFlags_BordersOuter);
    QUERY->doc_var(QUERY, "Draw outer borders.");
    static t_CKINT ImGuiTableFlags_Borders = 1920;
    QUERY->add_svar(QUERY, "int", "Borders", true, &ImGuiTableFlags_Borders);
    QUERY->doc_var(QUERY, "Draw all borders.");
    static t_CKINT ImGuiTableFlags_NoBordersInBody = 2048;
    QUERY->add_svar(QUERY, "int", "NoBordersInBody", true,
                    &ImGuiTableFlags_NoBordersInBody);
    QUERY->doc_var(QUERY,
                   "[ALPHA] Disable vertical borders in columns Body (borders "
                   "will always appear in Headers). -> May move to style");
    static t_CKINT ImGuiTableFlags_NoBordersInBodyUntilResize = 4096;
    QUERY->add_svar(QUERY, "int", "NoBordersInBodyUntilResize", true,
                    &ImGuiTableFlags_NoBordersInBodyUntilResize);
    QUERY->doc_var(
      QUERY,
      "[ALPHA] Disable vertical borders in columns Body until hovered for "
      "resize (borders will always appear in Headers). -> May move to style");
    static t_CKINT ImGuiTableFlags_SizingFixedFit = 8192;
    QUERY->add_svar(QUERY, "int", "SizingFixedFit", true,
                    &ImGuiTableFlags_SizingFixedFit);
    QUERY->doc_var(QUERY,
                   "Columns default to _WidthFixed or _WidthAuto (if resizable "
                   "or not resizable), matching contents width.");
    static t_CKINT ImGuiTableFlags_SizingFixedSame = 16384;
    QUERY->add_svar(QUERY, "int", "SizingFixedSame", true,
                    &ImGuiTableFlags_SizingFixedSame);
    QUERY->doc_var(
      QUERY,
      "Columns default to _WidthFixed or _WidthAuto (if resizable or not "
      "resizable), matching the maximum contents width of all columns. "
      "Implicitly enable ImGuiTableFlags_NoKeepColumnsVisible.");
    static t_CKINT ImGuiTableFlags_SizingStretchProp = 24576;
    QUERY->add_svar(QUERY, "int", "SizingStretchProp", true,
                    &ImGuiTableFlags_SizingStretchProp);
    QUERY->doc_var(QUERY,
                   "Columns default to _WidthStretch with default weights "
                   "proportional to each columns contents widths.");
    static t_CKINT ImGuiTableFlags_SizingStretchSame = 32768;
    QUERY->add_svar(QUERY, "int", "SizingStretchSame", true,
                    &ImGuiTableFlags_SizingStretchSame);
    QUERY->doc_var(QUERY,
                   "Columns default to _WidthStretch with default weights all "
                   "equal, unless overridden by TableSetupColumn().");
    static t_CKINT ImGuiTableFlags_NoHostExtendX = 65536;
    QUERY->add_svar(QUERY, "int", "NoHostExtendX", true,
                    &ImGuiTableFlags_NoHostExtendX);
    QUERY->doc_var(QUERY,
                   "Make outer width auto-fit to columns, overriding "
                   "outer_size.x value. Only available when ScrollX/ScrollY "
                   "are disabled and Stretch columns are not used.");
    static t_CKINT ImGuiTableFlags_NoHostExtendY = 131072;
    QUERY->add_svar(QUERY, "int", "NoHostExtendY", true,
                    &ImGuiTableFlags_NoHostExtendY);
    QUERY->doc_var(
      QUERY,
      "Make outer height stop exactly at outer_size.y (prevent auto-extending "
      "table past the limit). Only available when ScrollX/ScrollY are "
      "disabled. Data below the limit will be clipped and not visible.");
    static t_CKINT ImGuiTableFlags_NoKeepColumnsVisible = 262144;
    QUERY->add_svar(QUERY, "int", "NoKeepColumnsVisible", true,
                    &ImGuiTableFlags_NoKeepColumnsVisible);
    QUERY->doc_var(
      QUERY,
      "Disable keeping column always minimally visible when ScrollX is off and "
      "table gets too small. Not recommended if columns are resizable.");
    static t_CKINT ImGuiTableFlags_PreciseWidths = 524288;
    QUERY->add_svar(QUERY, "int", "PreciseWidths", true,
                    &ImGuiTableFlags_PreciseWidths);
    QUERY->doc_var(
      QUERY,
      "Disable distributing remainder width to stretched columns (width "
      "allocation on a 100-wide table with 3 columns: Without this flag: "
      "33,33,34. With this flag: 33,33,33). With larger number of columns, "
      "resizing will appear to be less smooth.");
    static t_CKINT ImGuiTableFlags_NoClip = 1048576;
    QUERY->add_svar(QUERY, "int", "NoClip", true, &ImGuiTableFlags_NoClip);
    QUERY->doc_var(
      QUERY,
      "Disable clipping rectangle for every individual columns (reduce draw "
      "command count, items will be able to overflow into other columns). "
      "Generally incompatible with TableSetupScrollFreeze().");
    static t_CKINT ImGuiTableFlags_PadOuterX = 2097152;
    QUERY->add_svar(QUERY, "int", "PadOuterX", true,
                    &ImGuiTableFlags_PadOuterX);
    QUERY->doc_var(QUERY,
                   "Default if BordersOuterV is on. Enable outermost padding. "
                   "Generally desirable if you have headers.");
    static t_CKINT ImGuiTableFlags_NoPadOuterX = 4194304;
    QUERY->add_svar(QUERY, "int", "NoPadOuterX", true,
                    &ImGuiTableFlags_NoPadOuterX);
    QUERY->doc_var(
      QUERY, "Default if BordersOuterV is off. Disable outermost padding.");
    static t_CKINT ImGuiTableFlags_NoPadInnerX = 8388608;
    QUERY->add_svar(QUERY, "int", "NoPadInnerX", true,
                    &ImGuiTableFlags_NoPadInnerX);
    QUERY->doc_var(
      QUERY,
      "Disable inner padding between columns (double inner padding if "
      "BordersOuterV is on, single inner padding if BordersOuterV is off).");
    static t_CKINT ImGuiTableFlags_ScrollX = 16777216;
    QUERY->add_svar(QUERY, "int", "ScrollX", true, &ImGuiTableFlags_ScrollX);
    QUERY->doc_var(
      QUERY,
      "Enable horizontal scrolling. Require 'outer_size' parameter of "
      "BeginTable() to specify the container size. Changes default sizing "
      "policy. Because this creates a child window, ScrollY is currently "
      "generally recommended when using ScrollX.");
    static t_CKINT ImGuiTableFlags_ScrollY = 33554432;
    QUERY->add_svar(QUERY, "int", "ScrollY", true, &ImGuiTableFlags_ScrollY);
    QUERY->doc_var(QUERY,
                   "Enable vertical scrolling. Require 'outer_size' parameter "
                   "of BeginTable() to specify the container size.");
    static t_CKINT ImGuiTableFlags_SortMulti = 67108864;
    QUERY->add_svar(QUERY, "int", "SortMulti", true,
                    &ImGuiTableFlags_SortMulti);
    QUERY->doc_var(
      QUERY,
      "Hold shift when clicking headers to sort on multiple column. "
      "TableGetSortSpecs() may return specs where (SpecsCount > 1).");
    static t_CKINT ImGuiTableFlags_SortTristate = 134217728;
    QUERY->add_svar(QUERY, "int", "SortTristate", true,
                    &ImGuiTableFlags_SortTristate);
    QUERY->doc_var(
      QUERY,
      "Allow no sorting, disable default sorting. TableGetSortSpecs() may "
      "return specs where (SpecsCount == 0).");
    static t_CKINT ImGuiTableFlags_HighlightHoveredColumn = 268435456;
    QUERY->add_svar(QUERY, "int", "HighlightHoveredColumn", true,
                    &ImGuiTableFlags_HighlightHoveredColumn);
    QUERY->doc_var(QUERY,
                   "Highlight column headers when hovered (may evolve into a "
                   "fuller highlight)");
    QUERY->end_class(QUERY);

    QUERY->begin_class(QUERY, "UI_TableColumnFlags", "Object");
    QUERY->doc_class(QUERY, "Flags for ImGui::TableSetupColumn().\n");
    static t_CKINT ImGuiTableColumnFlags_None = 0;
    QUERY->add_svar(QUERY, "int", "None", true, &ImGuiTableColumnFlags_None);
    static t_CKINT ImGuiTableColumnFlags_Disabled = 1;
    QUERY->add_svar(QUERY, "int", "Disabled", true,
                    &ImGuiTableColumnFlags_Disabled);
    QUERY->doc_var(QUERY,
                   "Overriding/master disable flag: hide column, won't show in "
                   "context menu (unlike calling TableSetColumnEnabled() which "
                   "manipulates the user accessible state)");
    static t_CKINT ImGuiTableColumnFlags_DefaultHide = 2;
    QUERY->add_svar(QUERY, "int", "DefaultHide", true,
                    &ImGuiTableColumnFlags_DefaultHide);
    QUERY->doc_var(QUERY, "Default as a hidden/disabled column.");
    static t_CKINT ImGuiTableColumnFlags_DefaultSort = 4;
    QUERY->add_svar(QUERY, "int", "DefaultSort", true,
                    &ImGuiTableColumnFlags_DefaultSort);
    QUERY->doc_var(QUERY, "Default as a sorting column.");
    static t_CKINT ImGuiTableColumnFlags_WidthStretch = 8;
    QUERY->add_svar(QUERY, "int", "WidthStretch", true,
                    &ImGuiTableColumnFlags_WidthStretch);
    QUERY->doc_var(QUERY,
                   "Column will stretch. Preferable with horizontal scrolling "
                   "disabled (default if table sizing policy is "
                   "_SizingStretchSame or _SizingStretchProp).");
    static t_CKINT ImGuiTableColumnFlags_WidthFixed = 16;
    QUERY->add_svar(QUERY, "int", "WidthFixed", true,
                    &ImGuiTableColumnFlags_WidthFixed);
    QUERY->doc_var(QUERY,
                   "Column will not stretch. Preferable with horizontal "
                   "scrolling enabled (default if table sizing policy is "
                   "_SizingFixedFit and table is resizable).");
    static t_CKINT ImGuiTableColumnFlags_NoResize = 32;
    QUERY->add_svar(QUERY, "int", "NoResize", true,
                    &ImGuiTableColumnFlags_NoResize);
    QUERY->doc_var(QUERY, "Disable manual resizing.");
    static t_CKINT ImGuiTableColumnFlags_NoReorder = 64;
    QUERY->add_svar(QUERY, "int", "NoReorder", true,
                    &ImGuiTableColumnFlags_NoReorder);
    QUERY->doc_var(QUERY,
                   "Disable manual reordering this column, this will also "
                   "prevent other columns from crossing over this column.");
    static t_CKINT ImGuiTableColumnFlags_NoHide = 128;
    QUERY->add_svar(QUERY, "int", "NoHide", true,
                    &ImGuiTableColumnFlags_NoHide);
    QUERY->doc_var(QUERY, "Disable ability to hide/disable this column.");
    static t_CKINT ImGuiTableColumnFlags_NoClip = 256;
    QUERY->add_svar(QUERY, "int", "NoClip", true,
                    &ImGuiTableColumnFlags_NoClip);
    QUERY->doc_var(QUERY,
                   "Disable clipping for this column (all NoClip columns will "
                   "render in a same draw command).");
    static t_CKINT ImGuiTableColumnFlags_NoSort = 512;
    QUERY->add_svar(QUERY, "int", "NoSort", true,
                    &ImGuiTableColumnFlags_NoSort);
    QUERY->doc_var(QUERY,
                   "Disable ability to sort on this field (even if "
                   "ImGuiTableFlags_Sortable is set on the table).");
    static t_CKINT ImGuiTableColumnFlags_NoSortAscending = 1024;
    QUERY->add_svar(QUERY, "int", "NoSortAscending", true,
                    &ImGuiTableColumnFlags_NoSortAscending);
    QUERY->doc_var(QUERY,
                   "Disable ability to sort in the ascending direction.");
    static t_CKINT ImGuiTableColumnFlags_NoSortDescending = 2048;
    QUERY->add_svar(QUERY, "int", "NoSortDescending", true,
                    &ImGuiTableColumnFlags_NoSortDescending);
    QUERY->doc_var(QUERY,
                   "Disable ability to sort in the descending direction.");
    static t_CKINT ImGuiTableColumnFlags_NoHeaderLabel = 4096;
    QUERY->add_svar(QUERY, "int", "NoHeaderLabel", true,
                    &ImGuiTableColumnFlags_NoHeaderLabel);
    QUERY->doc_var(QUERY,
                   "TableHeadersRow() will not submit horizontal label for "
                   "this column. Convenient for some small columns. Name will "
                   "still appear in context menu or in angled headers.");
    static t_CKINT ImGuiTableColumnFlags_NoHeaderWidth = 8192;
    QUERY->add_svar(QUERY, "int", "NoHeaderWidth", true,
                    &ImGuiTableColumnFlags_NoHeaderWidth);
    QUERY->doc_var(
      QUERY,
      "Disable header text width contribution to automatic column width.");
    static t_CKINT ImGuiTableColumnFlags_PreferSortAscending = 16384;
    QUERY->add_svar(QUERY, "int", "PreferSortAscending", true,
                    &ImGuiTableColumnFlags_PreferSortAscending);
    QUERY->doc_var(QUERY,
                   "Make the initial sort direction Ascending when first "
                   "sorting on this column (default).");
    static t_CKINT ImGuiTableColumnFlags_PreferSortDescending = 32768;
    QUERY->add_svar(QUERY, "int", "PreferSortDescending", true,
                    &ImGuiTableColumnFlags_PreferSortDescending);
    QUERY->doc_var(QUERY,
                   "Make the initial sort direction Descending when first "
                   "sorting on this column.");
    static t_CKINT ImGuiTableColumnFlags_IndentEnable = 65536;
    QUERY->add_svar(QUERY, "int", "IndentEnable", true,
                    &ImGuiTableColumnFlags_IndentEnable);
    QUERY->doc_var(
      QUERY,
      "Use current Indent value when entering cell (default for column 0).");
    static t_CKINT ImGuiTableColumnFlags_IndentDisable = 131072;
    QUERY->add_svar(QUERY, "int", "IndentDisable", true,
                    &ImGuiTableColumnFlags_IndentDisable);
    QUERY->doc_var(
      QUERY,
      "Ignore current Indent value when entering cell (default for columns > "
      "0). Indentation changes _within_ the cell will still be honored.");
    static t_CKINT ImGuiTableColumnFlags_AngledHeader = 262144;
    QUERY->add_svar(QUERY, "int", "AngledHeader", true,
                    &ImGuiTableColumnFlags_AngledHeader);
    QUERY->doc_var(QUERY,
                   "TableHeadersRow() will submit an angled header row for "
                   "this column. Note this will add an extra row.");
    static t_CKINT ImGuiTableColumnFlags_IsEnabled = 16777216;
    QUERY->add_svar(QUERY, "int", "IsEnabled", true,
                    &ImGuiTableColumnFlags_IsEnabled);
    QUERY->doc_var(QUERY,
                   "Status: is enabled == not hidden by user/api (referred to "
                   "as \"Hide\" in _DefaultHide and _NoHide) flags.");
    static t_CKINT ImGuiTableColumnFlags_IsVisible = 33554432;
    QUERY->add_svar(QUERY, "int", "IsVisible", true,
                    &ImGuiTableColumnFlags_IsVisible);
    QUERY->doc_var(
      QUERY, "Status: is visible == is enabled AND not clipped by scrolling.");
    static t_CKINT ImGuiTableColumnFlags_IsSorted = 67108864;
    QUERY->add_svar(QUERY, "int", "IsSorted", true,
                    &ImGuiTableColumnFlags_IsSorted);
    QUERY->doc_var(QUERY, "Status: is currently part of the sort specs");
    static t_CKINT ImGuiTableColumnFlags_IsHovered = 134217728;
    QUERY->add_svar(QUERY, "int", "IsHovered", true,
                    &ImGuiTableColumnFlags_IsHovered);
    QUERY->doc_var(QUERY, "Status: is hovered by mouse");
    QUERY->end_class(QUERY);

    QUERY->begin_class(QUERY, "UI_TableRowFlags", "Object");
    QUERY->doc_class(QUERY, "Flags for ImGui::TableNextRow().\n");
    static t_CKINT ImGuiTableRowFlags_None = 0;
    QUERY->add_svar(QUERY, "int", "None", true, &ImGuiTableRowFlags_None);
    static t_CKINT ImGuiTableRowFlags_Headers = 1;
    QUERY->add_svar(QUERY, "int", "Headers", true, &ImGuiTableRowFlags_Headers);
    QUERY->doc_var(
      QUERY,
      "Identify header row (set default background color + width of its "
      "contents accounted differently for auto column width)");
    QUERY->end_class(QUERY);

    QUERY->begin_class(QUERY, "UI_TableBgTarget", "Object");
    QUERY->doc_class(
      QUERY,
      "Enum for ImGui::TableSetBgColor().\nBackground colors are rendering in "
      "3 layers:.\n- Layer 0: draw with RowBg0 color if set, otherwise draw "
      "with ColumnBg0 if set..\n- Layer 1: draw with RowBg1 color if set, "
      "otherwise draw with ColumnBg1 if set..\n- Layer 2: draw with CellBg "
      "color if set..\nThe purpose of the two row/columns layers is to let you "
      "decide if a background color change should override or blend with the "
      "existing color..\nWhen using ImGuiTableFlags_RowBg on the table, each "
      "row has the RowBg0 color automatically set for odd/even rows..\nIf you "
      "set the color of RowBg0 target, your color will override the existing "
      "RowBg0 color..\nIf you set the color of RowBg1 or ColumnBg1 target, "
      "your color will blend over the RowBg0 color..\n");
    static t_CKINT ImGuiTableBgTarget_None = 0;
    QUERY->add_svar(QUERY, "int", "None", true, &ImGuiTableBgTarget_None);
    static t_CKINT ImGuiTableBgTarget_RowBg0 = 1;
    QUERY->add_svar(QUERY, "int", "RowBg0", true, &ImGuiTableBgTarget_RowBg0);
    QUERY->doc_var(QUERY,
                   "Set row background color 0 (generally used for background, "
                   "automatically set when ImGuiTableFlags_RowBg is used)");
    static t_CKINT ImGuiTableBgTarget_RowBg1 = 2;
    QUERY->add_svar(QUERY, "int", "RowBg1", true, &ImGuiTableBgTarget_RowBg1);
    QUERY->doc_var(
      QUERY,
      "Set row background color 1 (generally used for selection marking)");
    static t_CKINT ImGuiTableBgTarget_CellBg = 3;
    QUERY->add_svar(QUERY, "int", "CellBg", true, &ImGuiTableBgTarget_CellBg);
    QUERY->doc_var(QUERY, "Set cell background color (top-most color)");
    QUERY->end_class(QUERY);

    QUERY->begin_class(QUERY, "UI_DrawFlags", "Object");
    QUERY->doc_class(
      QUERY,
      "Flags for ImDrawList functions.\n(Legacy: bit 0 must always correspond "
      "to ImDrawFlags_Closed to be backward compatible with old API using a "
      "bool. Bits 1..3 must be unused).\n");
    static t_CKINT ImDrawFlags_None = 0;
    QUERY->add_svar(QUERY, "int", "None", true, &ImDrawFlags_None);
    static t_CKINT ImDrawFlags_Closed = 1;
    QUERY->add_svar(QUERY, "int", "Closed", true, &ImDrawFlags_Closed);
    QUERY->doc_var(QUERY,
                   "PathStroke(), AddPolyline(): specify that shape should be "
                   "closed (Important: this is always == 1 for legacy reason)");
    static t_CKINT ImDrawFlags_RoundCornersTopLeft = 16;
    QUERY->add_svar(QUERY, "int", "RoundCornersTopLeft", true,
                    &ImDrawFlags_RoundCornersTopLeft);
    QUERY->doc_var(
      QUERY,
      "AddRect(), AddRectFilled(), PathRect(): enable rounding top-left corner "
      "only (when rounding > 0.0f, we default to all corners). Was 0x01.");
    static t_CKINT ImDrawFlags_RoundCornersTopRight = 32;
    QUERY->add_svar(QUERY, "int", "RoundCornersTopRight", true,
                    &ImDrawFlags_RoundCornersTopRight);
    QUERY->doc_var(QUERY,
                   "AddRect(), AddRectFilled(), PathRect(): enable rounding "
                   "top-right corner only (when rounding > 0.0f, we default to "
                   "all corners). Was 0x02.");
    static t_CKINT ImDrawFlags_RoundCornersBottomLeft = 64;
    QUERY->add_svar(QUERY, "int", "RoundCornersBottomLeft", true,
                    &ImDrawFlags_RoundCornersBottomLeft);
    QUERY->doc_var(QUERY,
                   "AddRect(), AddRectFilled(), PathRect(): enable rounding "
                   "bottom-left corner only (when rounding > 0.0f, we default "
                   "to all corners). Was 0x04.");
    static t_CKINT ImDrawFlags_RoundCornersBottomRight = 128;
    QUERY->add_svar(QUERY, "int", "RoundCornersBottomRight", true,
                    &ImDrawFlags_RoundCornersBottomRight);
    QUERY->doc_var(QUERY,
                   "AddRect(), AddRectFilled(), PathRect(): enable rounding "
                   "bottom-right corner only (when rounding > 0.0f, we default "
                   "to all corners). Wax 0x08.");
    static t_CKINT ImDrawFlags_RoundCornersNone = 256;
    QUERY->add_svar(QUERY, "int", "RoundCornersNone", true,
                    &ImDrawFlags_RoundCornersNone);
    QUERY->doc_var(
      QUERY,
      "AddRect(), AddRectFilled(), PathRect(): disable rounding on all corners "
      "(when rounding > 0.0f). This is NOT zero, NOT an implicit flag!");
    static t_CKINT ImDrawFlags_RoundCornersTop = 48;
    QUERY->add_svar(QUERY, "int", "RoundCornersTop", true,
                    &ImDrawFlags_RoundCornersTop);
    static t_CKINT ImDrawFlags_RoundCornersBottom = 192;
    QUERY->add_svar(QUERY, "int", "RoundCornersBottom", true,
                    &ImDrawFlags_RoundCornersBottom);
    static t_CKINT ImDrawFlags_RoundCornersLeft = 80;
    QUERY->add_svar(QUERY, "int", "RoundCornersLeft", true,
                    &ImDrawFlags_RoundCornersLeft);
    static t_CKINT ImDrawFlags_RoundCornersRight = 160;
    QUERY->add_svar(QUERY, "int", "RoundCornersRight", true,
                    &ImDrawFlags_RoundCornersRight);
    static t_CKINT ImDrawFlags_RoundCornersAll = 240;
    QUERY->add_svar(QUERY, "int", "RoundCornersAll", true,
                    &ImDrawFlags_RoundCornersAll);
    QUERY->end_class(QUERY);

    QUERY->begin_class(QUERY, "UI_DrawListFlags", "Object");
    QUERY->doc_class(
      QUERY,
      "Flags for ImDrawList instance. Those are set automatically by ImGui:: "
      "functions from ImGuiIO settings, and generally not manipulated "
      "directly..\nIt is however possible to temporarily alter flags between "
      "calls to ImDrawList:: functions..\n");
    static t_CKINT ImDrawListFlags_None = 0;
    QUERY->add_svar(QUERY, "int", "None", true, &ImDrawListFlags_None);
    static t_CKINT ImDrawListFlags_AntiAliasedLines = 1;
    QUERY->add_svar(QUERY, "int", "AntiAliasedLines", true,
                    &ImDrawListFlags_AntiAliasedLines);
    QUERY->doc_var(
      QUERY,
      "Enable anti-aliased lines/borders (*2 the number of triangles for 1.0f "
      "wide line or lines thin enough to be drawn using textures, otherwise *3 "
      "the number of triangles)");
    static t_CKINT ImDrawListFlags_AntiAliasedLinesUseTex = 2;
    QUERY->add_svar(QUERY, "int", "AntiAliasedLinesUseTex", true,
                    &ImDrawListFlags_AntiAliasedLinesUseTex);
    QUERY->doc_var(QUERY,
                   "Enable anti-aliased lines/borders using textures when "
                   "possible. Require backend to render with bilinear "
                   "filtering (NOT point/nearest filtering).");
    static t_CKINT ImDrawListFlags_AntiAliasedFill = 4;
    QUERY->add_svar(QUERY, "int", "AntiAliasedFill", true,
                    &ImDrawListFlags_AntiAliasedFill);
    QUERY->doc_var(QUERY,
                   "Enable anti-aliased edge around filled shapes (rounded "
                   "rectangles, circles).");
    static t_CKINT ImDrawListFlags_AllowVtxOffset = 8;
    QUERY->add_svar(QUERY, "int", "AllowVtxOffset", true,
                    &ImDrawListFlags_AllowVtxOffset);
    QUERY->doc_var(QUERY,
                   "Can emit 'VtxOffset > 0' to allow large meshes. Set when "
                   "'ImGuiBackendFlags_RendererHasVtxOffset' is enabled.");
    QUERY->end_class(QUERY);

    QUERY->begin_class(QUERY, "UI_FontAtlasFlags", "Object");
    QUERY->doc_class(QUERY, "Flags for ImFontAtlas build.\n");
    static t_CKINT ImFontAtlasFlags_None = 0;
    QUERY->add_svar(QUERY, "int", "None", true, &ImFontAtlasFlags_None);
    static t_CKINT ImFontAtlasFlags_NoPowerOfTwoHeight = 1;
    QUERY->add_svar(QUERY, "int", "NoPowerOfTwoHeight", true,
                    &ImFontAtlasFlags_NoPowerOfTwoHeight);
    QUERY->doc_var(QUERY, "Don't round the height to next power of two");
    static t_CKINT ImFontAtlasFlags_NoMouseCursors = 2;
    QUERY->add_svar(QUERY, "int", "NoMouseCursors", true,
                    &ImFontAtlasFlags_NoMouseCursors);
    QUERY->doc_var(QUERY,
                   "Don't build software mouse cursors into the atlas (save a "
                   "little texture memory)");
    static t_CKINT ImFontAtlasFlags_NoBakedLines = 4;
    QUERY->add_svar(QUERY, "int", "NoBakedLines", true,
                    &ImFontAtlasFlags_NoBakedLines);
    QUERY->doc_var(
      QUERY,
      "Don't build thick line textures into the atlas (save a little texture "
      "memory, allow support for point/nearest filtering). The "
      "AntiAliasedLinesUseTex features uses them, otherwise they will be "
      "rendered using polygons (more expensive for CPU/GPU).");
    QUERY->end_class(QUERY);

    QUERY->begin_class(QUERY, "UI_ViewportFlags", "Object");
    QUERY->doc_class(QUERY,
                     "Flags stored in ImGuiViewport::Flags, giving indications "
                     "to the platform backends..\n");
    static t_CKINT ImGuiViewportFlags_None = 0;
    QUERY->add_svar(QUERY, "int", "None", true, &ImGuiViewportFlags_None);
    static t_CKINT ImGuiViewportFlags_IsPlatformWindow = 1;
    QUERY->add_svar(QUERY, "int", "IsPlatformWindow", true,
                    &ImGuiViewportFlags_IsPlatformWindow);
    QUERY->doc_var(QUERY, "Represent a Platform Window");
    static t_CKINT ImGuiViewportFlags_IsPlatformMonitor = 2;
    QUERY->add_svar(QUERY, "int", "IsPlatformMonitor", true,
                    &ImGuiViewportFlags_IsPlatformMonitor);
    QUERY->doc_var(QUERY, "Represent a Platform Monitor (unused yet)");
    static t_CKINT ImGuiViewportFlags_OwnedByApp = 4;
    QUERY->add_svar(QUERY, "int", "OwnedByApp", true,
                    &ImGuiViewportFlags_OwnedByApp);
    QUERY->doc_var(QUERY,
                   "Platform Window: Was created/managed by the user "
                   "application? (rather than our backend)");
    static t_CKINT ImGuiViewportFlags_NoDecoration = 8;
    QUERY->add_svar(QUERY, "int", "NoDecoration", true,
                    &ImGuiViewportFlags_NoDecoration);
    QUERY->doc_var(
      QUERY,
      "Platform Window: Disable platform decorations: title bar, borders, etc. "
      "(generally set all windows, but if ImGuiConfigFlags_ViewportsDecoration "
      "is set we only set this on popups/tooltips)");
    static t_CKINT ImGuiViewportFlags_NoTaskBarIcon = 16;
    QUERY->add_svar(QUERY, "int", "NoTaskBarIcon", true,
                    &ImGuiViewportFlags_NoTaskBarIcon);
    QUERY->doc_var(QUERY,
                   "Platform Window: Disable platform task bar icon (generally "
                   "set on popups/tooltips, or all windows if "
                   "ImGuiConfigFlags_ViewportsNoTaskBarIcon is set)");
    static t_CKINT ImGuiViewportFlags_NoFocusOnAppearing = 32;
    QUERY->add_svar(QUERY, "int", "NoFocusOnAppearing", true,
                    &ImGuiViewportFlags_NoFocusOnAppearing);
    QUERY->doc_var(QUERY, "Platform Window: Don't take focus when created.");
    static t_CKINT ImGuiViewportFlags_NoFocusOnClick = 64;
    QUERY->add_svar(QUERY, "int", "NoFocusOnClick", true,
                    &ImGuiViewportFlags_NoFocusOnClick);
    QUERY->doc_var(QUERY, "Platform Window: Don't take focus when clicked on.");
    static t_CKINT ImGuiViewportFlags_NoInputs = 128;
    QUERY->add_svar(QUERY, "int", "NoInputs", true,
                    &ImGuiViewportFlags_NoInputs);
    QUERY->doc_var(QUERY,
                   "Platform Window: Make mouse pass through so we can drag "
                   "this window while peaking behind it.");
    static t_CKINT ImGuiViewportFlags_NoRendererClear = 256;
    QUERY->add_svar(QUERY, "int", "NoRendererClear", true,
                    &ImGuiViewportFlags_NoRendererClear);
    QUERY->doc_var(QUERY,
                   "Platform Window: Renderer doesn't need to clear the "
                   "framebuffer ahead (because we will fill it entirely).");
    static t_CKINT ImGuiViewportFlags_NoAutoMerge = 512;
    QUERY->add_svar(QUERY, "int", "NoAutoMerge", true,
                    &ImGuiViewportFlags_NoAutoMerge);
    QUERY->doc_var(QUERY,
                   "Platform Window: Avoid merging this window into another "
                   "host window. This can only be set via ImGuiWindowClass "
                   "viewport flags override (because we need to now ahead if "
                   "we are going to create a viewport in the first place!).");
    static t_CKINT ImGuiViewportFlags_TopMost = 1024;
    QUERY->add_svar(QUERY, "int", "TopMost", true, &ImGuiViewportFlags_TopMost);
    QUERY->doc_var(QUERY,
                   "Platform Window: Display on top (for tooltips only).");
    static t_CKINT ImGuiViewportFlags_CanHostOtherWindows = 2048;
    QUERY->add_svar(QUERY, "int", "CanHostOtherWindows", true,
                    &ImGuiViewportFlags_CanHostOtherWindows);
    QUERY->doc_var(
      QUERY,
      "Viewport can host multiple imgui windows (secondary viewports are "
      "associated to a single window).  FIXME: In practice there's still "
      "probably code making the assumption that this is always and only on the "
      "MainViewport. Will fix once we add support for \"no main viewport\".");
    static t_CKINT ImGuiViewportFlags_IsMinimized = 4096;
    QUERY->add_svar(QUERY, "int", "IsMinimized", true,
                    &ImGuiViewportFlags_IsMinimized);
    QUERY->doc_var(
      QUERY,
      "Platform Window: Window is minimized, can skip render. When minimized "
      "we tend to avoid using the viewport pos/size for clipping window or "
      "testing if they are contained in the viewport.");
    static t_CKINT ImGuiViewportFlags_IsFocused = 8192;
    QUERY->add_svar(QUERY, "int", "IsFocused", true,
                    &ImGuiViewportFlags_IsFocused);
    QUERY->doc_var(QUERY,
                   "Platform Window: Window is focused (last call to "
                   "Platform_GetWindowFocus() returned true)");
    QUERY->end_class(QUERY);
}
