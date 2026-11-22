{
    "patcher": {
        "fileversion": 1,
        "appversion": {
            "major": 9,
            "minor": 2,
            "revision": 0,
            "architecture": "x64",
            "modernui": 1
        },
        "classnamespace": "box",
        "rect": [ 100.0, 100.0, 900.0, 666.0 ],
        "showrootpatcherontab": 0,
        "showontab": 0,
        "autosave": 0,
        "boxes": [
            {
                "box": {
                    "id": "tab-basic",
                    "maxclass": "newobj",
                    "numinlets": 0,
                    "numoutlets": 0,
                    "patching_rect": [ 20.0, 20.0, 100.0, 22.0 ],
                    "text": "p basic",
                    "patcher": {
                        "fileversion": 1,
                        "appversion": {
                            "major": 9,
                            "minor": 2,
                            "revision": 0,
                            "architecture": "x64",
                            "modernui": 1
                        },
                        "classnamespace": "box",
                        "rect": [ 0.0, 26.0, 900.0, 640.0 ],
                        "showontab": 1,
                        "boxes": [
                            {
                                "box": {
                                    "id": "title",
                                    "maxclass": "comment",
                                    "patching_rect": [ 20.0, 15.0, 700.0, 24.0 ],
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "text": "ck_code : a ChucK editor box for chuck~",
                                    "fontface": 1,
                                    "fontsize": 16.0
                                }
                            },
                            {
                                "box": {
                                    "id": "desc",
                                    "maxclass": "comment",
                                    "patching_rect": [ 20.0, 45.0, 860.0, 47.0 ],
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "text": "Turn on audio, click into the editor and press Cmd-Return. ck_code sends 'addcode <name> <code>', 'replacecode <id> <name> <code>' and 'remove <id>' to chuck~. It learns which shreds it owns from chuck~'s reply outlet, which must be connected back into its inlet. The title bar counts them.",
                                    "linecount": 3
                                }
                            },
                            {
                                "box": {
                                    "id": "ed",
                                    "maxclass": "ck_code",
                                    "patching_rect": [ 20.0, 100.0, 520.0, 300.0 ],
                                    "numinlets": 1,
                                    "numoutlets": 1,
                                    "outlettype": [ "" ],
                                    "ck_code_text": "// cmd-ret: add as a new shred\nSinOsc s => ADSR e => dac;\ne.set(5::ms, 80::ms, 0.2, 200::ms);\n0.3 => s.gain;\n[60, 63, 67, 70, 72] @=> int notes[];\nwhile (true) {\n    Std.mtof(notes[Math.random2(0, 4)]) => s.freq;\n    e.keyOn();\n    150::ms => now;\n    e.keyOff();\n    100::ms => now;\n}\n"
                                }
                            },
                            {
                                "box": {
                                    "id": "keys-h",
                                    "maxclass": "comment",
                                    "patching_rect": [ 560.0, 100.0, 320.0, 20.0 ],
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "text": "Keys (click into the editor first)",
                                    "fontface": 1
                                }
                            },
                            {
                                "box": {
                                    "id": "keys",
                                    "maxclass": "comment",
                                    "patching_rect": [ 560.0, 122.0, 320.0, 155.0 ],
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "text": "Cmd-Return: add the buffer, or the selection, as a new shred\nShift-Return: add the top-level statements on the caret's line\nCmd-\\ or Cmd-Shift-Return: replace this box's latest shred\nCmd-Backspace: remove this box's latest shred\nTab: complete a word; Shift-Tab: outdent\nCmd-Z / Cmd-Shift-Z: undo / redo\nRight-click: save to or load from a .ck file",
                                    "linecount": 10
                                }
                            },
                            {
                                "box": {
                                    "id": "msgs-h",
                                    "maxclass": "comment",
                                    "patching_rect": [ 560.0, 290.0, 320.0, 20.0 ],
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "text": "The same actions as messages",
                                    "fontface": 1
                                }
                            },
                            {
                                "box": {
                                    "id": "m-add",
                                    "maxclass": "message",
                                    "patching_rect": [ 560.0, 312.0, 40.0, 22.0 ],
                                    "numinlets": 2,
                                    "numoutlets": 1,
                                    "outlettype": [ "" ],
                                    "text": "add"
                                }
                            },
                            {
                                "box": {
                                    "id": "m-rep",
                                    "maxclass": "message",
                                    "patching_rect": [ 608.0, 312.0, 56.0, 22.0 ],
                                    "numinlets": 2,
                                    "numoutlets": 1,
                                    "outlettype": [ "" ],
                                    "text": "replace"
                                }
                            },
                            {
                                "box": {
                                    "id": "m-rm",
                                    "maxclass": "message",
                                    "patching_rect": [ 672.0, 312.0, 56.0, 22.0 ],
                                    "numinlets": 2,
                                    "numoutlets": 1,
                                    "outlettype": [ "" ],
                                    "text": "remove"
                                }
                            },
                            {
                                "box": {
                                    "id": "ck",
                                    "maxclass": "newobj",
                                    "patching_rect": [ 20.0, 430.0, 160.0, 22.0 ],
                                    "numinlets": 2,
                                    "numoutlets": 3,
                                    "text": "chuck~ 2",
                                    "outlettype": [ "signal", "signal", "" ]
                                }
                            },
                            {
                                "box": {
                                    "id": "dac",
                                    "maxclass": "ezdac~",
                                    "patching_rect": [ 20.0, 470.0, 45.0, 45.0 ],
                                    "numinlets": 2,
                                    "numoutlets": 0
                                }
                            },
                            {
                                "box": {
                                    "id": "dac-c",
                                    "maxclass": "comment",
                                    "patching_rect": [ 72.0, 485.0, 100.0, 20.0 ],
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "text": "audio on/off"
                                }
                            },
                            {
                                "box": {
                                    "id": "route",
                                    "maxclass": "newobj",
                                    "patching_rect": [ 260.0, 470.0, 80.0, 22.0 ],
                                    "numinlets": 2,
                                    "numoutlets": 2,
                                    "text": "route shred",
                                    "outlettype": [ "", "" ]
                                }
                            },
                            {
                                "box": {
                                    "id": "print",
                                    "maxclass": "newobj",
                                    "patching_rect": [ 260.0, 504.0, 90.0, 22.0 ],
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "text": "print SHRED"
                                }
                            },
                            {
                                "box": {
                                    "id": "cord-c",
                                    "maxclass": "comment",
                                    "patching_rect": [ 260.0, 430.0, 600.0, 33.0 ],
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "text": "The cord from chuck~'s rightmost outlet back into ck_code carries 'shred add|remove|dump'. Without it ck_code cannot replace or remove: it never learns the ids.",
                                    "linecount": 2
                                }
                            },
                            {
                                "box": {
                                    "id": "name-c",
                                    "maxclass": "comment",
                                    "patching_rect": [ 260.0, 540.0, 600.0, 47.0 ],
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "text": "Shreds are named after @shredname, else the @file name, else untitled-<n>.ck. The [print SHRED] shows 'add <id> <name>' as each shred starts. Compile errors appear in the Max window.",
                                    "linecount": 3
                                }
                            }
                        ],
                        "lines": [
                            {
                                "patchline": {
                                    "source": [ "m-add", 0 ],
                                    "destination": [ "ed", 0 ]
                                }
                            },
                            {
                                "patchline": {
                                    "source": [ "m-rep", 0 ],
                                    "destination": [ "ed", 0 ]
                                }
                            },
                            {
                                "patchline": {
                                    "source": [ "m-rm", 0 ],
                                    "destination": [ "ed", 0 ]
                                }
                            },
                            {
                                "patchline": {
                                    "source": [ "ed", 0 ],
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
                                    "destination": [ "ed", 0 ]
                                }
                            },
                            {
                                "patchline": {
                                    "source": [ "ck", 2 ],
                                    "destination": [ "route", 0 ]
                                }
                            },
                            {
                                "patchline": {
                                    "source": [ "route", 0 ],
                                    "destination": [ "print", 0 ]
                                }
                            }
                        ]
                    }
                }
            },
            {
                "box": {
                    "id": "tab-shared",
                    "maxclass": "newobj",
                    "numinlets": 0,
                    "numoutlets": 0,
                    "patching_rect": [ 130.0, 20.0, 100.0, 22.0 ],
                    "text": "p shared",
                    "patcher": {
                        "fileversion": 1,
                        "appversion": {
                            "major": 9,
                            "minor": 2,
                            "revision": 0,
                            "architecture": "x64",
                            "modernui": 1
                        },
                        "classnamespace": "box",
                        "rect": [ 0.0, 26.0, 900.0, 640.0 ],
                        "showontab": 1,
                        "boxes": [
                            {
                                "box": {
                                    "id": "desc",
                                    "maxclass": "comment",
                                    "patching_rect": [ 20.0, 15.0, 860.0, 47.0 ],
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "text": "Boxes with the same name share shreds, like one file in two windows. Both editors below have @shredname drone.ck: add from one, then replace from the other with Cmd-\\. Ownership follows the name, so a replace from either takes over the latest drone.ck shred.",
                                    "linecount": 3
                                }
                            },
                            {
                                "box": {
                                    "id": "ed1",
                                    "maxclass": "ck_code",
                                    "patching_rect": [ 20.0, 75.0, 420.0, 220.0 ],
                                    "numinlets": 1,
                                    "numoutlets": 1,
                                    "outlettype": [ "" ],
                                    "ck_code_text": "// edit the frequency, then cmd-\\ to replace the running shred\nTriOsc t => LPF f => dac;\n110 => t.freq;\n600 => f.freq;\n0.15 => t.gain;\n1::week => now;\n",
                                    "shredname": "drone.ck"
                                }
                            },
                            {
                                "box": {
                                    "id": "ed2",
                                    "maxclass": "ck_code",
                                    "patching_rect": [ 460.0, 75.0, 420.0, 220.0 ],
                                    "numinlets": 1,
                                    "numoutlets": 1,
                                    "outlettype": [ "" ],
                                    "ck_code_text": "// edit the frequency, then cmd-\\ to replace the running shred\nTriOsc t => LPF f => dac;\n165 => t.freq;\n600 => f.freq;\n0.15 => t.gain;\n1::week => now;\n",
                                    "shredname": "drone.ck"
                                }
                            },
                            {
                                "box": {
                                    "id": "ck",
                                    "maxclass": "newobj",
                                    "patching_rect": [ 20.0, 330.0, 160.0, 22.0 ],
                                    "numinlets": 2,
                                    "numoutlets": 3,
                                    "text": "chuck~ 2",
                                    "outlettype": [ "signal", "signal", "" ]
                                }
                            },
                            {
                                "box": {
                                    "id": "dac",
                                    "maxclass": "ezdac~",
                                    "patching_rect": [ 20.0, 370.0, 45.0, 45.0 ],
                                    "numinlets": 2,
                                    "numoutlets": 0
                                }
                            },
                            {
                                "box": {
                                    "id": "mon",
                                    "maxclass": "ck_shreds",
                                    "patching_rect": [ 460.0, 330.0, 420.0, 180.0 ],
                                    "numinlets": 1,
                                    "numoutlets": 1,
                                    "outlettype": [ "" ]
                                }
                            },
                            {
                                "box": {
                                    "id": "mon-c",
                                    "maxclass": "comment",
                                    "patching_rect": [ 460.0, 515.0, 420.0, 33.0 ],
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "text": "ck_shreds lists every shred in chuck~; see its own help file."
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
                                    "source": [ "ck", 2 ],
                                    "destination": [ "ed1", 0 ]
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
                                    "source": [ "ck", 2 ],
                                    "destination": [ "ed2", 0 ]
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
                            }
                        ]
                    }
                }
            },
            {
                "box": {
                    "id": "tab-attributes",
                    "maxclass": "newobj",
                    "numinlets": 0,
                    "numoutlets": 0,
                    "patching_rect": [ 240.0, 20.0, 100.0, 22.0 ],
                    "text": "p attributes",
                    "patcher": {
                        "fileversion": 1,
                        "appversion": {
                            "major": 9,
                            "minor": 2,
                            "revision": 0,
                            "architecture": "x64",
                            "modernui": 1
                        },
                        "classnamespace": "box",
                        "rect": [ 0.0, 26.0, 900.0, 640.0 ],
                        "showontab": 1,
                        "boxes": [
                            {
                                "box": {
                                    "id": "desc",
                                    "maxclass": "comment",
                                    "patching_rect": [ 20.0, 15.0, 860.0, 33.0 ],
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "text": "Attributes are set from the inspector or with messages. Colors follow @theme unless set one by one (@bgcolor, @keywordcolor, ...). 'themes' prints the theme names.",
                                    "linecount": 2
                                }
                            },
                            {
                                "box": {
                                    "id": "am0",
                                    "maxclass": "message",
                                    "patching_rect": [ 20.0, 60.0, 110.0, 22.0 ],
                                    "numinlets": 2,
                                    "numoutlets": 1,
                                    "outlettype": [ "" ],
                                    "text": "themes"
                                }
                            },
                            {
                                "box": {
                                    "id": "am1",
                                    "maxclass": "message",
                                    "patching_rect": [ 140.0, 60.0, 110.0, 22.0 ],
                                    "numinlets": 2,
                                    "numoutlets": 1,
                                    "outlettype": [ "" ],
                                    "text": "theme dracula"
                                }
                            },
                            {
                                "box": {
                                    "id": "am2",
                                    "maxclass": "message",
                                    "patching_rect": [ 20.0, 90.0, 110.0, 22.0 ],
                                    "numinlets": 2,
                                    "numoutlets": 1,
                                    "outlettype": [ "" ],
                                    "text": "theme onedark"
                                }
                            },
                            {
                                "box": {
                                    "id": "am3",
                                    "maxclass": "message",
                                    "patching_rect": [ 140.0, 90.0, 110.0, 22.0 ],
                                    "numinlets": 2,
                                    "numoutlets": 1,
                                    "outlettype": [ "" ],
                                    "text": "fontsize 15"
                                }
                            },
                            {
                                "box": {
                                    "id": "am4",
                                    "maxclass": "message",
                                    "patching_rect": [ 20.0, 120.0, 110.0, 22.0 ],
                                    "numinlets": 2,
                                    "numoutlets": 1,
                                    "outlettype": [ "" ],
                                    "text": "fontsize 12"
                                }
                            },
                            {
                                "box": {
                                    "id": "am5",
                                    "maxclass": "message",
                                    "patching_rect": [ 140.0, 120.0, 110.0, 22.0 ],
                                    "numinlets": 2,
                                    "numoutlets": 1,
                                    "outlettype": [ "" ],
                                    "text": "linenumbers 0"
                                }
                            },
                            {
                                "box": {
                                    "id": "am6",
                                    "maxclass": "message",
                                    "patching_rect": [ 20.0, 150.0, 110.0, 22.0 ],
                                    "numinlets": 2,
                                    "numoutlets": 1,
                                    "outlettype": [ "" ],
                                    "text": "linenumbers 1"
                                }
                            },
                            {
                                "box": {
                                    "id": "am7",
                                    "maxclass": "message",
                                    "patching_rect": [ 140.0, 150.0, 110.0, 22.0 ],
                                    "numinlets": 2,
                                    "numoutlets": 1,
                                    "outlettype": [ "" ],
                                    "text": "wrap 0"
                                }
                            },
                            {
                                "box": {
                                    "id": "am8",
                                    "maxclass": "message",
                                    "patching_rect": [ 20.0, 180.0, 110.0, 22.0 ],
                                    "numinlets": 2,
                                    "numoutlets": 1,
                                    "outlettype": [ "" ],
                                    "text": "wrap 1"
                                }
                            },
                            {
                                "box": {
                                    "id": "am9",
                                    "maxclass": "message",
                                    "patching_rect": [ 140.0, 180.0, 110.0, 22.0 ],
                                    "numinlets": 2,
                                    "numoutlets": 1,
                                    "outlettype": [ "" ],
                                    "text": "readonly 1"
                                }
                            },
                            {
                                "box": {
                                    "id": "am10",
                                    "maxclass": "message",
                                    "patching_rect": [ 20.0, 210.0, 110.0, 22.0 ],
                                    "numinlets": 2,
                                    "numoutlets": 1,
                                    "outlettype": [ "" ],
                                    "text": "readonly 0"
                                }
                            },
                            {
                                "box": {
                                    "id": "am11",
                                    "maxclass": "message",
                                    "patching_rect": [ 140.0, 210.0, 110.0, 22.0 ],
                                    "numinlets": 2,
                                    "numoutlets": 1,
                                    "outlettype": [ "" ],
                                    "text": "shredname mine.ck"
                                }
                            },
                            {
                                "box": {
                                    "id": "ed",
                                    "maxclass": "ck_code",
                                    "patching_rect": [ 270.0, 60.0, 610.0, 240.0 ],
                                    "numinlets": 1,
                                    "numoutlets": 1,
                                    "outlettype": [ "" ],
                                    "ck_code_text": "// a box for trying attributes\nSinOsc s => dac;\n1::second => now;\n"
                                }
                            },
                            {
                                "box": {
                                    "id": "list",
                                    "maxclass": "comment",
                                    "patching_rect": [ 20.0, 320.0, 860.0, 260.0 ],
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "text": "@file  path of a .ck file; read on load when @autoload is on\n@autoload (1)  read @file when it is set\n@autowrite (0)  write @file 400 ms after each edit\n@shredname  name for this box's shreds; default: the @file name, else untitled-<n>.ck\n@theme  onedark catppuccin dracula nord gruvbox monokai solarized\n@fontsize (12)  @tabwidth (4)  @scrollspeed (3)\n@linenumbers (1)  @titlebar (1)  @wrap (1)  @matchbrackets (1)\n@blink (1)  @blinkduration (500)  flash the code that was sent, in ms\n@completion (1)  Tab completes words, keywords and UGen names\n@autoclose (1)  type the closing bracket or quote\n@readonly (0)  no edits; sending still works\nThe text is saved in the patcher.",
                                    "linecount": 13
                                }
                            }
                        ],
                        "lines": [
                            {
                                "patchline": {
                                    "source": [ "am0", 0 ],
                                    "destination": [ "ed", 0 ]
                                }
                            },
                            {
                                "patchline": {
                                    "source": [ "am1", 0 ],
                                    "destination": [ "ed", 0 ]
                                }
                            },
                            {
                                "patchline": {
                                    "source": [ "am2", 0 ],
                                    "destination": [ "ed", 0 ]
                                }
                            },
                            {
                                "patchline": {
                                    "source": [ "am3", 0 ],
                                    "destination": [ "ed", 0 ]
                                }
                            },
                            {
                                "patchline": {
                                    "source": [ "am4", 0 ],
                                    "destination": [ "ed", 0 ]
                                }
                            },
                            {
                                "patchline": {
                                    "source": [ "am5", 0 ],
                                    "destination": [ "ed", 0 ]
                                }
                            },
                            {
                                "patchline": {
                                    "source": [ "am6", 0 ],
                                    "destination": [ "ed", 0 ]
                                }
                            },
                            {
                                "patchline": {
                                    "source": [ "am7", 0 ],
                                    "destination": [ "ed", 0 ]
                                }
                            },
                            {
                                "patchline": {
                                    "source": [ "am8", 0 ],
                                    "destination": [ "ed", 0 ]
                                }
                            },
                            {
                                "patchline": {
                                    "source": [ "am9", 0 ],
                                    "destination": [ "ed", 0 ]
                                }
                            },
                            {
                                "patchline": {
                                    "source": [ "am10", 0 ],
                                    "destination": [ "ed", 0 ]
                                }
                            },
                            {
                                "patchline": {
                                    "source": [ "am11", 0 ],
                                    "destination": [ "ed", 0 ]
                                }
                            }
                        ]
                    }
                }
            }
        ],
        "lines": []
    }
}
