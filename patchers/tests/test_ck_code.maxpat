{
    "patcher": {
        "fileversion": 1,
        "appversion": {
            "major": 9,
            "minor": 1,
            "revision": 2,
            "architecture": "x64",
            "modernui": 1
        },
        "classnamespace": "box",
        "rect": [ 80.0, 80.0, 940.0, 780.0 ],
        "boxes": [
            {
                "box": {
                    "id": "title",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 20.0, 15.0, 620.0, 22.0 ],
                    "text": "ck_code : ChucK editor box for chuck~",
                    "fontsize": 14.0,
                    "fontface": 1
                }
            },
            {
                "box": {
                    "id": "note",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 20.0, 40.0, 620.0, 50.0 ],
                    "linecount": 3,
                    "text": "ck_code sends addcode / replacecode / remove to chuck~. chuck~'s reply outlet goes back into ck_code, which claims the shreds named after it (title bar: shred count). Two boxes with the same @shredname share shreds; try @shredname otf.ck on both."
                }
            },
            {
                "box": {
                    "id": "ed1",
                    "maxclass": "ck_code",
                    "numinlets": 1,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 20.0, 95.0, 470.0, 300.0 ],
                    "shredname": "arp.ck",
                    "ck_code_text": "// cmd-ret: add as a new shred    shift-ret: add the block under the caret\n// cmd-\\ : replace this box's latest shred    cmd-bksp: remove it\nSinOsc s => ADSR e => dac;\ne.set(5::ms, 80::ms, 0.2, 200::ms);\n0.3 => s.gain;\n\n[60, 63, 67, 70, 72] @=> int notes[];\nwhile (true) {\n    Std.mtof(notes[Math.random2(0, notes.size() - 1)]) => s.freq;\n    e.keyOn();\n    150::ms => now;\n    e.keyOff();\n    100::ms => now;\n}\n"
                }
            },
            {
                "box": {
                    "id": "ed2",
                    "maxclass": "ck_code",
                    "numinlets": 1,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 500.0, 95.0, 400.0, 300.0 ],
                    "ck_code_text": "Noise n => LPF f => dac;\n800 => f.freq;\n0.05 => n.gain;\n1::week => now;\n"
                }
            },
            {
                "box": {
                    "id": "dump",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 20.0, 410.0, 80.0, 22.0 ],
                    "text": "shreds dump"
                }
            },
            {
                "box": {
                    "id": "rmall",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 110.0, 410.0, 70.0, 22.0 ],
                    "text": "removeall"
                }
            },
            {
                "box": {
                    "id": "ck",
                    "maxclass": "newobj",
                    "numinlets": 2,
                    "numoutlets": 3,
                    "outlettype": [ "signal", "signal", "" ],
                    "patching_rect": [ 20.0, 450.0, 160.0, 22.0 ],
                    "text": "chuck~ 2"
                }
            },
            {
                "box": {
                    "id": "dac",
                    "maxclass": "ezdac~",
                    "numinlets": 2,
                    "numoutlets": 0,
                    "patching_rect": [ 20.0, 500.0, 45.0, 45.0 ]
                }
            },
            {
                "box": {
                    "id": "print",
                    "maxclass": "newobj",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 250.0, 500.0, 80.0, 22.0 ],
                    "text": "print reply"
                }
            },
            {
                "box": {
                    "id": "mon",
                    "maxclass": "ck_shreds",
                    "numinlets": 1,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 500.0, 450.0, 400.0, 200.0 ]
                }
            },
            {
                "box": {
                    "id": "mon-note",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 500.0, 655.0, 400.0, 50.0 ],
                    "linecount": 3,
                    "text": "ck_shreds polls chuck~ with 'shreds dump' every @interval ms and lists every shred, including ones the editors did not start. The red button sends 'remove <id>'."
                }
            }
        ],
        "lines": [
            {
                "patchline": {
                    "source": [ "ed1", 0 ],
                    "destination": [ "ck", 0 ]
                }
            },
            {
                "patchline": {
                    "source": [ "ed2", 0 ],
                    "destination": [ "ck", 0 ]
                }
            },
            {
                "patchline": {
                    "source": [ "dump", 0 ],
                    "destination": [ "ck", 0 ]
                }
            },
            {
                "patchline": {
                    "source": [ "rmall", 0 ],
                    "destination": [ "ck", 0 ]
                }
            },
            {
                "patchline": {
                    "source": [ "ck", 0 ],
                    "destination": [ "dac", 0 ]
                }
            },
            {
                "patchline": {
                    "source": [ "ck", 1 ],
                    "destination": [ "dac", 1 ]
                }
            },
            {
                "patchline": {
                    "source": [ "ck", 2 ],
                    "destination": [ "ed1", 0 ]
                }
            },
            {
                "patchline": {
                    "source": [ "ck", 2 ],
                    "destination": [ "ed2", 0 ]
                }
            },
            {
                "patchline": {
                    "source": [ "ck", 2 ],
                    "destination": [ "print", 0 ]
                }
            },
            {
                "patchline": {
                    "source": [ "mon", 0 ],
                    "destination": [ "ck", 0 ]
                }
            },
            {
                "patchline": {
                    "source": [ "ck", 2 ],
                    "destination": [ "mon", 0 ]
                }
            }
        ]
    }
}
