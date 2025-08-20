GText text;

T.assert(T.veq(text.color(), @(1, 1, 1, 1)), "Default color is white");
text.color(@(0, 0, 0, 1));
T.assert(T.veq(text.color(), @(0, 0, 0, 1)), "Color set to black");

T.assert(text.text() == "hello ChuGL", "default text");
text.text("hello world");
T.assert(text.text() == "hello world", "set text");

T.assert(text.font() == "", "default font");
text.font("chugl:font");
T.assert(text.font() == "chugl:font", "set font");

T.assert(T.feq(text.spacing(), 1.0), "default spacing");
text.spacing(2.0);
T.assert(T.feq(text.spacing(), 2.0), "set spacing");

Texture tex;
text.texture(tex);
T.assert(text.texture() == tex, "set texture");
