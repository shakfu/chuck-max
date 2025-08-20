UI_Bool ui_bool;
T.assert(
    ui_bool.val() == false,
    "UI_Bool default value"
);

UI_String ui_string;
T.assert( ui_string.val() == "", "UI_String default value was " + ui_string.val());
ui_string.val("hello");
T.assert( ui_string.val() == "hello", "UI_String set value");