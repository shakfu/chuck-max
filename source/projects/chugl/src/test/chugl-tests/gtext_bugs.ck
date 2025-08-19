// empty string no crash
GText empty_string --> GG.scene();
"" => empty_string.text;

GG.nextFrame() => now;
GG.nextFrame() => now;

// nonempty string --> empty string no crash
GText change_to_empty --> GG.scene();
GG.nextFrame() => now;
GG.nextFrame() => now;

"    " => change_to_empty.text;

GG.nextFrame() => now;
GG.nextFrame() => now;

// ungrucking no crash
change_to_empty --< GG.scene();

GG.nextFrame() => now;
GText ungrucked_text --> GG.scene();
repeat(2) GG.nextFrame() => now;
ungrucked_text.detach();
repeat(2) GG.nextFrame() => now;
