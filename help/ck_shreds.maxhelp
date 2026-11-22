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
                                    "text": "ck_shreds : the shreds running in a chuck~",
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
                                    "text": "Turn on audio and add some shreds. ck_shreds sends 'shreds dump' to chuck~ every @interval ms and fills the table from the reply outlet; 'shred add' and 'shred remove' update it in between. The red button sends 'remove <id>'.",
                                    "linecount": 3
                                }
                            },
                            {
                                "box": {
                                    "id": "c0",
                                    "maxclass": "message",
                                    "patching_rect": [ 20.0, 100.0, 420.0, 22.0 ],
                                    "numinlets": 2,
                                    "numoutlets": 1,
                                    "outlettype": [ "" ],
                                    "text": "addcode beep.ck \"SinOsc s => dac; 0.05 => s.gain; 1::hour => now;\""
                                }
                            },
                            {
                                "box": {
                                    "id": "cc0",
                                    "maxclass": "comment",
                                    "patching_rect": [ 450.0, 100.0, 200.0, 20.0 ],
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "text": "a sine"
                                }
                            },
                            {
                                "box": {
                                    "id": "c1",
                                    "maxclass": "message",
                                    "patching_rect": [ 20.0, 130.0, 420.0, 22.0 ],
                                    "numinlets": 2,
                                    "numoutlets": 1,
                                    "outlettype": [ "" ],
                                    "text": "addcode noise.ck \"Noise n => LPF f => dac; 500 => f.freq; 0.03 => n.gain; 1::hour => now;\""
                                }
                            },
                            {
                                "box": {
                                    "id": "cc1",
                                    "maxclass": "comment",
                                    "patching_rect": [ 450.0, 130.0, 200.0, 20.0 ],
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "text": "filtered noise"
                                }
                            },
                            {
                                "box": {
                                    "id": "c2",
                                    "maxclass": "message",
                                    "patching_rect": [ 20.0, 160.0, 420.0, 22.0 ],
                                    "numinlets": 2,
                                    "numoutlets": 1,
                                    "outlettype": [ "" ],
                                    "text": "add test/sine.ck"
                                }
                            },
                            {
                                "box": {
                                    "id": "cc2",
                                    "maxclass": "comment",
                                    "patching_rect": [ 450.0, 160.0, 200.0, 20.0 ],
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "text": "a file: the table shows its name"
                                }
                            },
                            {
                                "box": {
                                    "id": "c3",
                                    "maxclass": "message",
                                    "patching_rect": [ 20.0, 190.0, 420.0, 22.0 ],
                                    "numinlets": 2,
                                    "numoutlets": 1,
                                    "outlettype": [ "" ],
                                    "text": "eval \"1::hour => now;\""
                                }
                            },
                            {
                                "box": {
                                    "id": "cc3",
                                    "maxclass": "comment",
                                    "patching_rect": [ 450.0, 190.0, 200.0, 20.0 ],
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "text": "eval: named <compiled.code>"
                                }
                            },
                            {
                                "box": {
                                    "id": "c4",
                                    "maxclass": "message",
                                    "patching_rect": [ 20.0, 220.0, 420.0, 22.0 ],
                                    "numinlets": 2,
                                    "numoutlets": 1,
                                    "outlettype": [ "" ],
                                    "text": "removeall"
                                }
                            },
                            {
                                "box": {
                                    "id": "cc4",
                                    "maxclass": "comment",
                                    "patching_rect": [ 450.0, 220.0, 200.0, 20.0 ],
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "text": "remove every shred"
                                }
                            },
                            {
                                "box": {
                                    "id": "mon",
                                    "maxclass": "ck_shreds",
                                    "patching_rect": [ 20.0, 260.0, 420.0, 200.0 ],
                                    "numinlets": 1,
                                    "numoutlets": 1,
                                    "outlettype": [ "" ]
                                }
                            },
                            {
                                "box": {
                                    "id": "ck",
                                    "maxclass": "newobj",
                                    "patching_rect": [ 20.0, 500.0, 160.0, 22.0 ],
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
                                    "patching_rect": [ 20.0, 540.0, 45.0, 45.0 ],
                                    "numinlets": 2,
                                    "numoutlets": 0
                                }
                            },
                            {
                                "box": {
                                    "id": "mm0",
                                    "maxclass": "message",
                                    "patching_rect": [ 470.0, 260.0, 100.0, 22.0 ],
                                    "numinlets": 2,
                                    "numoutlets": 1,
                                    "outlettype": [ "" ],
                                    "text": "bang"
                                }
                            },
                            {
                                "box": {
                                    "id": "mc0",
                                    "maxclass": "comment",
                                    "patching_rect": [ 580.0, 260.0, 300.0, 20.0 ],
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "text": "dump now"
                                }
                            },
                            {
                                "box": {
                                    "id": "mm1",
                                    "maxclass": "message",
                                    "patching_rect": [ 470.0, 290.0, 100.0, 22.0 ],
                                    "numinlets": 2,
                                    "numoutlets": 1,
                                    "outlettype": [ "" ],
                                    "text": "clear"
                                }
                            },
                            {
                                "box": {
                                    "id": "mc1",
                                    "maxclass": "comment",
                                    "patching_rect": [ 580.0, 290.0, 300.0, 20.0 ],
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "text": "empty the table until the next dump"
                                }
                            },
                            {
                                "box": {
                                    "id": "mm2",
                                    "maxclass": "message",
                                    "patching_rect": [ 470.0, 320.0, 100.0, 22.0 ],
                                    "numinlets": 2,
                                    "numoutlets": 1,
                                    "outlettype": [ "" ],
                                    "text": "interval 250"
                                }
                            },
                            {
                                "box": {
                                    "id": "mc2",
                                    "maxclass": "comment",
                                    "patching_rect": [ 580.0, 320.0, 300.0, 20.0 ],
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "text": "poll 4 times a second"
                                }
                            },
                            {
                                "box": {
                                    "id": "mm3",
                                    "maxclass": "message",
                                    "patching_rect": [ 470.0, 350.0, 100.0, 22.0 ],
                                    "numinlets": 2,
                                    "numoutlets": 1,
                                    "outlettype": [ "" ],
                                    "text": "interval 1000"
                                }
                            },
                            {
                                "box": {
                                    "id": "mc3",
                                    "maxclass": "comment",
                                    "patching_rect": [ 580.0, 350.0, 300.0, 20.0 ],
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "text": "the default"
                                }
                            },
                            {
                                "box": {
                                    "id": "mm4",
                                    "maxclass": "message",
                                    "patching_rect": [ 470.0, 380.0, 100.0, 22.0 ],
                                    "numinlets": 2,
                                    "numoutlets": 1,
                                    "outlettype": [ "" ],
                                    "text": "interval 0"
                                }
                            },
                            {
                                "box": {
                                    "id": "mc4",
                                    "maxclass": "comment",
                                    "patching_rect": [ 580.0, 380.0, 300.0, 20.0 ],
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "text": "no polling: add/remove events only"
                                }
                            },
                            {
                                "box": {
                                    "id": "time-c",
                                    "maxclass": "comment",
                                    "patching_rect": [ 470.0, 420.0, 410.0, 75.0 ],
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "text": "Time is VM time since the shred started. Between dumps it advances only while audio is on, as VM time does. Colors: @bgcolor @textcolor @headercolor @gridcolor @buttoncolor; the font comes from the inspector.",
                                    "linecount": 4
                                }
                            },
                            {
                                "box": {
                                    "id": "wire-c",
                                    "maxclass": "comment",
                                    "patching_rect": [ 200.0, 500.0, 680.0, 33.0 ],
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "text": "Wiring: ck_shreds outlet -> chuck~ inlet; chuck~ rightmost outlet -> ck_shreds inlet. It ignores the rest of the reply vocabulary (val, event, global, msg).",
                                    "linecount": 2
                                }
                            }
                        ],
                        "lines": [
                            {
                                "patchline": {
                                    "source": [ "c0", 0 ],
                                    "destination": [ "ck", 0 ]
                                }
                            },
                            {
                                "patchline": {
                                    "source": [ "c1", 0 ],
                                    "destination": [ "ck", 0 ]
                                }
                            },
                            {
                                "patchline": {
                                    "source": [ "c2", 0 ],
                                    "destination": [ "ck", 0 ]
                                }
                            },
                            {
                                "patchline": {
                                    "source": [ "c3", 0 ],
                                    "destination": [ "ck", 0 ]
                                }
                            },
                            {
                                "patchline": {
                                    "source": [ "c4", 0 ],
                                    "destination": [ "ck", 0 ]
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
                            },
                            {
                                "patchline": {
                                    "source": [ "mm0", 0 ],
                                    "destination": [ "mon", 0 ]
                                }
                            },
                            {
                                "patchline": {
                                    "source": [ "mm1", 0 ],
                                    "destination": [ "mon", 0 ]
                                }
                            },
                            {
                                "patchline": {
                                    "source": [ "mm2", 0 ],
                                    "destination": [ "mon", 0 ]
                                }
                            },
                            {
                                "patchline": {
                                    "source": [ "mm3", 0 ],
                                    "destination": [ "mon", 0 ]
                                }
                            },
                            {
                                "patchline": {
                                    "source": [ "mm4", 0 ],
                                    "destination": [ "mon", 0 ]
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
