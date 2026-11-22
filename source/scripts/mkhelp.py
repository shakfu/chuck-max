"""Generate help/ck_code.maxhelp, help/ck_shreds.maxhelp and a 'shreds' tab
for help/chuck~.maxhelp. Usage: python3 mkhelp.py <repo root>"""

import json
import sys

ROOT = sys.argv[1]
APP = {"major": 9, "minor": 2, "revision": 0, "architecture": "x64", "modernui": 1}


class Patch:
    def __init__(self):
        self.boxes, self.lines = [], []

    def add(self, id, maxclass, rect, **k):
        b = {"id": id, "maxclass": maxclass, "patching_rect": [float(v) for v in rect]}
        b.update(k)
        self.boxes.append({"box": b})
        return id

    def comment(self, id, rect, text, lines=1, bold=False, size=None):
        k = {"numinlets": 1, "numoutlets": 0, "text": text}
        if lines > 1:
            k["linecount"] = lines
        if bold:
            k["fontface"] = 1
        if size:
            k["fontsize"] = float(size)
        return self.add(id, "comment", rect, **k)

    def msg(self, id, rect, text):
        return self.add(id, "message", rect, numinlets=2, numoutlets=1, outlettype=[""], text=text)

    def obj(self, id, rect, text, ins, outs):
        k = {"numinlets": ins, "numoutlets": len(outs), "text": text}
        if outs:
            k["outlettype"] = outs
        return self.add(id, "newobj", rect, **k)

    def chuck(self, id, rect):
        return self.obj(id, rect, "chuck~ 2", 2, ["signal", "signal", ""])

    def dac(self, id, rect):
        return self.add(id, "ezdac~", rect, numinlets=2, numoutlets=0)

    def ck_code(self, id, rect, text, **attrs):
        return self.add(id, "ck_code", rect, numinlets=1, numoutlets=1, outlettype=[""], ck_code_text=text, **attrs)

    def ck_shreds(self, id, rect, **attrs):
        return self.add(id, "ck_shreds", rect, numinlets=1, numoutlets=1, outlettype=[""], **attrs)

    def cord(self, src, dst, outlet=0, inlet=0):
        self.lines.append({"patchline": {"source": [src, outlet], "destination": [dst, inlet]}})

    def patcher(self, rect, **k):
        p = {"fileversion": 1, "appversion": APP, "classnamespace": "box", "rect": rect}
        p.update(k)
        p["boxes"], p["lines"] = self.boxes, self.lines
        return p


def ser(o, ind=0):
    """JSON as Max 9 writes it: 4-space indent, scalar arrays on one line."""
    pad, pad1 = " " * (4 * ind), " " * (4 * (ind + 1))
    if isinstance(o, dict):
        if not o:
            return "{}"
        items = ",\n".join('%s%s: %s' % (pad1, json.dumps(k), ser(v, ind + 1)) for k, v in o.items())
        return "{\n" + items + "\n" + pad + "}"
    if isinstance(o, list):
        if not o:
            return "[]"
        if all(not isinstance(x, (dict, list)) for x in o):
            return "[ " + ", ".join(json.dumps(x) for x in o) + " ]"
        return "[\n" + ",\n".join(pad1 + ser(x, ind + 1) for x in o) + "\n" + pad + "]"
    return json.dumps(o)


def tab(name, patch, x):
    """A `p name` box shown as a tab of the help file."""
    return {"box": {"id": "tab-" + name, "maxclass": "newobj", "numinlets": 0, "numoutlets": 0,
                    "patching_rect": [float(x), 20.0, 100.0, 22.0], "text": "p " + name,
                    "patcher": patch.patcher([0.0, 26.0, 900.0, 640.0], showontab=1)}}


def helpfile(path, tabs):
    root = {"fileversion": 1, "appversion": APP, "classnamespace": "box",
            "rect": [100.0, 100.0, 900.0, 666.0], "showrootpatcherontab": 0, "showontab": 0,
            "autosave": 0, "boxes": [tab(n, p, 20 + 110 * i) for i, (n, p) in enumerate(tabs)], "lines": []}
    with open(path, "w") as f:
        f.write(ser({"patcher": root}) + "\n")


def reply_print(p, ck, x, y):
    """chuck~'s reply outlet into [route shred] -> [print SHRED]."""
    p.obj("route", (x, y, 80, 22), "route shred", 2, ["", ""])
    p.obj("print", (x, y + 34, 90, 22), "print SHRED", 1, [])
    p.cord(ck, "route", outlet=2)
    p.cord("route", "print")


ARP = """// cmd-ret: add as a new shred
SinOsc s => ADSR e => dac;
e.set(5::ms, 80::ms, 0.2, 200::ms);
0.3 => s.gain;
[60, 63, 67, 70, 72] @=> int notes[];
while (true) {
    Std.mtof(notes[Math.random2(0, 4)]) => s.freq;
    e.keyOn();
    150::ms => now;
    e.keyOff();
    100::ms => now;
}
"""

DRONE = """// edit the frequency, then cmd-\\ to replace the running shred
TriOsc t => LPF f => dac;
110 => t.freq;
600 => f.freq;
0.15 => t.gain;
1::week => now;
"""

# ---- ck_code: basic ----
b = Patch()
b.comment("title", (20, 15, 700, 24), "ck_code : a ChucK editor box for chuck~", bold=True, size=16)
b.comment("desc", (20, 45, 860, 47),
          "Turn on audio, click into the editor and press Cmd-Return. ck_code sends 'addcode <name> <code>', "
          "'replacecode <id> <name> <code>' and 'remove <id>' to chuck~. It learns which shreds it owns from "
          "chuck~'s reply outlet, which must be connected back into its inlet. The title bar counts them.", lines=3)
b.ck_code("ed", (20, 100, 520, 300), ARP)
b.comment("keys-h", (560, 100, 320, 20), "Keys (click into the editor first)", bold=True)
b.comment("keys", (560, 122, 320, 155),
          "Cmd-Return: add the buffer, or the selection, as a new shred\n"
          "Shift-Return: add the top-level statements on the caret's line\n"
          "Cmd-\\ or Cmd-Shift-Return: replace this box's latest shred\n"
          "Cmd-Backspace: remove this box's latest shred\n"
          "Tab: complete a word; Shift-Tab: outdent\n"
          "Cmd-Z / Cmd-Shift-Z: undo / redo\n"
          "Right-click: save to or load from a .ck file", lines=10)
b.comment("msgs-h", (560, 290, 320, 20), "The same actions as messages", bold=True)
b.msg("m-add", (560, 312, 40, 22), "add")
b.msg("m-rep", (608, 312, 56, 22), "replace")
b.msg("m-rm", (672, 312, 56, 22), "remove")
for m in ("m-add", "m-rep", "m-rm"):
    b.cord(m, "ed")
b.chuck("ck", (20, 430, 160, 22))
b.dac("dac", (20, 470, 45, 45))
b.comment("dac-c", (72, 485, 100, 20), "audio on/off")
b.cord("ed", "ck")
b.cord("ck", "dac", 0, 0)
b.cord("ck", "dac", 1, 1)
b.cord("ck", "ed", outlet=2)
reply_print(b, "ck", 260, 470)
b.comment("cord-c", (260, 430, 600, 33),
          "The cord from chuck~'s rightmost outlet back into ck_code carries 'shred add|remove|dump'. "
          "Without it ck_code cannot replace or remove: it never learns the ids.", lines=2)
b.comment("name-c", (260, 540, 600, 47),
          "Shreds are named after @shredname, else the @file name, else untitled-<n>.ck. The [print SHRED] "
          "shows 'add <id> <name>' as each shred starts. Compile errors appear in the Max window.", lines=3)

# ---- ck_code: shared ----
s = Patch()
s.comment("desc", (20, 15, 860, 47),
          "Boxes with the same name share shreds, like one file in two windows. Both editors below have "
          "@shredname drone.ck: add from one, then replace from the other with Cmd-\\. Ownership follows "
          "the name, so a replace from either takes over the latest drone.ck shred.", lines=3)
s.ck_code("ed1", (20, 75, 420, 220), DRONE, shredname="drone.ck")
s.ck_code("ed2", (460, 75, 420, 220), DRONE.replace("110 => t.freq", "165 => t.freq"), shredname="drone.ck")
s.chuck("ck", (20, 330, 160, 22))
s.dac("dac", (20, 370, 45, 45))
s.ck_shreds("mon", (460, 330, 420, 180))
s.comment("mon-c", (460, 515, 420, 33),
          "ck_shreds lists every shred in chuck~; see its own help file.", lines=1)
for e in ("ed1", "ed2", "mon"):
    s.cord(e, "ck")
    s.cord("ck", e, outlet=2)
s.cord("ck", "dac", 0, 0)
s.cord("ck", "dac", 1, 1)

# ---- ck_code: attributes ----
a = Patch()
a.comment("desc", (20, 15, 860, 33),
          "Attributes are set from the inspector or with messages. Colors follow @theme unless set one by one "
          "(@bgcolor, @keywordcolor, ...). 'themes' prints the theme names.", lines=2)
attr_msgs = ["themes", "theme dracula", "theme onedark", "fontsize 15", "fontsize 12", "linenumbers 0",
             "linenumbers 1", "wrap 0", "wrap 1", "readonly 1", "readonly 0", "shredname mine.ck"]
for i, t in enumerate(attr_msgs):
    a.msg("am%d" % i, (20 + (i % 2) * 120, 60 + (i // 2) * 30, 110, 22), t)
    a.cord("am%d" % i, "ed")
a.ck_code("ed", (270, 60, 610, 240), "// a box for trying attributes\nSinOsc s => dac;\n1::second => now;\n")
a.comment("list", (20, 320, 860, 260),
          "@file  path of a .ck file; read on load when @autoload is on\n"
          "@autoload (1)  read @file when it is set\n"
          "@autowrite (0)  write @file 400 ms after each edit\n"
          "@shredname  name for this box's shreds; default: the @file name, else untitled-<n>.ck\n"
          "@theme  onedark catppuccin dracula nord gruvbox monokai solarized\n"
          "@fontsize (12)  @tabwidth (4)  @scrollspeed (3)\n"
          "@linenumbers (1)  @titlebar (1)  @wrap (1)  @matchbrackets (1)\n"
          "@blink (1)  @blinkduration (500)  flash the code that was sent, in ms\n"
          "@completion (1)  Tab completes words, keywords and UGen names\n"
          "@autoclose (1)  type the closing bracket or quote\n"
          "@readonly (0)  no edits; sending still works\n"
          "The text is saved in the patcher.", lines=13)

helpfile(ROOT + "/help/ck_code.maxhelp", [("basic", b), ("shared", s), ("attributes", a)])

# ---- ck_shreds ----
m = Patch()
m.comment("title", (20, 15, 700, 24), "ck_shreds : the shreds running in a chuck~", bold=True, size=16)
m.comment("desc", (20, 45, 860, 47),
          "Turn on audio and add some shreds. ck_shreds sends 'shreds dump' to chuck~ every @interval ms and "
          "fills the table from the reply outlet; 'shred add' and 'shred remove' update it in between. The red "
          "button sends 'remove <id>'.", lines=3)
codes = [
    ("addcode beep.ck \"SinOsc s => dac; 0.05 => s.gain; 1::hour => now;\"", "a sine"),
    ("addcode noise.ck \"Noise n => LPF f => dac; 500 => f.freq; 0.03 => n.gain; 1::hour => now;\"", "filtered noise"),
    ("add test/sine.ck", "a file: the table shows its name"),
    ("eval \"1::hour => now;\"", "eval: named <compiled.code>"),
    ("removeall", "remove every shred"),
]
for i, (t, c) in enumerate(codes):
    y = 100 + i * 30
    m.msg("c%d" % i, (20, y, 420, 22), t)
    m.comment("cc%d" % i, (450, y, 200, 20), c)
    m.cord("c%d" % i, "ck")
m.ck_shreds("mon", (20, 260, 420, 200))
m.chuck("ck", (20, 500, 160, 22))
m.dac("dac", (20, 540, 45, 45))
m.cord("mon", "ck")
m.cord("ck", "mon", outlet=2)
m.cord("ck", "dac", 0, 0)
m.cord("ck", "dac", 1, 1)
mm = [("bang", "dump now"), ("clear", "empty the table until the next dump"),
      ("interval 250", "poll 4 times a second"), ("interval 1000", "the default"),
      ("interval 0", "no polling: add/remove events only")]
for i, (t, c) in enumerate(mm):
    y = 260 + i * 30
    m.msg("mm%d" % i, (470, y, 100, 22), t)
    m.comment("mc%d" % i, (580, y, 300, 20), c)
    m.cord("mm%d" % i, "mon")
m.comment("time-c", (470, 420, 410, 75),
          "Time is VM time since the shred started. Between dumps it advances only while audio is on, as VM "
          "time does. Colors: @bgcolor @textcolor @headercolor @gridcolor @buttoncolor; the font comes from "
          "the inspector.", lines=4)
m.comment("wire-c", (200, 500, 680, 33),
          "Wiring: ck_shreds outlet -> chuck~ inlet; chuck~ rightmost outlet -> ck_shreds inlet. It ignores "
          "the rest of the reply vocabulary (val, event, global, msg).", lines=2)
helpfile(ROOT + "/help/ck_shreds.maxhelp", [("basic", m)])

# ---- chuck~: shreds tab ----
t = Patch()
t.comment("t", (20, 15, 860, 47),
          "Shreds by name, and listing them. Turn on audio at the bottom. 'addcode' and 'replacecode' are "
          "'add' and 'replace' for code: the shred is named <name> instead of <compiled.code>. The data outlet "
          "reports 'shred add <id> <name>' and 'shred remove <id>'; 'shreds dump' answers with every shred.", lines=3)
t.comment("h1", (20, 70, 400, 20), "1. Named code shreds", bold=True)
t.msg("a1", (20, 92, 520, 22), "addcode drone.ck \"TriOsc t => dac; 110 => t.freq; 0.1 => t.gain; 1::hour => now;\"")
t.msg("a2", (20, 122, 520, 22), "replacecode 1 drone.ck \"TriOsc t => dac; 165 => t.freq; 0.1 => t.gain; 1::hour => now;\"")
t.comment("n2", (550, 122, 330, 33),
          "replaces shred 1 (use the id from 'shred add'); the id stays", lines=2)
t.msg("a3", (20, 152, 120, 22), "remove 1")
t.comment("h2", (20, 190, 400, 20), "2. List shreds", bold=True)
t.msg("d1", (20, 212, 100, 22), "shreds dump")
t.comment("n3", (130, 212, 750, 33),
          "replies 'shred dump <id> <name> <elapsed> ...', one triple per shred, elapsed in seconds of VM "
          "time. ck_shreds polls with it.", lines=2)
t.msg("d2", (20, 250, 80, 22), "removeall")
for i in ("a1", "a2", "a3", "d1", "d2"):
    t.cord(i, "ck")
t.chuck("ck", (20, 300, 160, 22))
t.dac("dac", (20, 340, 45, 45))
t.cord("ck", "dac", 0, 0)
t.cord("ck", "dac", 1, 1)
reply_print(t, "ck", 260, 340)
t.comment("f", (20, 420, 860, 33),
          "ck_code (an editor box) and ck_shreds (a shred table) are built on these messages; see their help files.",
          lines=1)

path = ROOT + "/help/chuck~.maxhelp"
with open(path) as f:
    text = f.read()
assert '"text": "p shreds"' not in text, "shreds tab already present"
nb = tab("shreds", t, 226)
nb["box"]["id"] = "obj-shreds"
nb["box"]["patching_rect"] = [226.0, 234.0, 70.0, 22.0]
nb["box"]["patcher"]["rect"] = [0.0, 26.0, 983.0, 718.0]
# splice after the last root box, leaving the rest of the file as Max wrote it
anchor = '\n        ],\n        "lines": [],\n        "parameters"'
assert text.count(anchor) == 1
i = text.index(anchor)
text = text[:i] + ",\n            " + ser(nb, 3) + text[i:]
json.loads(text)  # still valid
with open(path, "w") as f:
    f.write(text)
