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
        "rect": [ 352.0, 167.0, 298.0, 480.0 ],
        "openinpresentation": 1,
        "boxes": [
            {
                "box": {
                    "id": "obj-10",
                    "maxclass": "newobj",
                    "numinlets": 1,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 132.0, 314.0, 96.0, 22.0 ],
                    "text": "string.tosymbol"
                }
            },
            {
                "box": {
                    "id": "obj-5",
                    "maxclass": "newobj",
                    "numinlets": 1,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 132.0, 352.0, 78.0, 22.0 ],
                    "text": "prepend eval"
                }
            },
            {
                "box": {
                    "code": "global float length;\n\nNoise imp => OneZero lowpass => dac;\nlowpass => Delay delay => lowpass;\n.99999 => float R;\nlength => float L;\nL::samp => delay.delay;\nMath.pow( R, L ) => delay.gain;\n-1 => lowpass.zero;\n1 => imp.gain;\nL::samp => now;\n0 => imp.gain;\n(Math.log(.0001) / Math.log(R))::samp => now;",
                    "editlocked": 1,
                    "fontface": 0,
                    "fontname": "<Monospaced>",
                    "fontsize": 12.0,
                    "id": "obj-2",
                    "maxclass": "text.codebox",
                    "numinlets": 2,
                    "numoutlets": 3,
                    "outlettype": [ "", "", "" ],
                    "patching_rect": [ 132.0, 99.0, 340.0, 200.0 ],
                    "presentation": 1,
                    "presentation_rect": [ 2.0, 30.0, 391.0, 236.0 ],
                    "saved_object_attributes": {
                        "parameter_enable": 0,
                        "parameter_mappable": 0
                    },
                    "syntax": ".lua"
                }
            },
            {
                "box": {
                    "id": "obj-8",
                    "maxclass": "newobj",
                    "numinlets": 1,
                    "numoutlets": 1,
                    "outlettype": [ "bang" ],
                    "patching_rect": [ 495.0, 43.0, 58.0, 22.0 ],
                    "text": "loadbang"
                }
            },
            {
                "box": {
                    "id": "obj-7",
                    "linecount": 13,
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 495.0, 89.0, 260.0, 183.0 ],
                    "text": "set \"global float length;\n\nNoise imp => OneZero lowpass => dac;\nlowpass => Delay delay => lowpass;\n.99999 => float R;\nlength => float L;\nL::samp => delay.delay;\nMath.pow( R, L ) => delay.gain;\n-1 => lowpass.zero;\n1 => imp.gain;\nL::samp => now;\n0 => imp.gain;\n(Math.log(.0001) / Math.log(R))::samp => now;\""
                }
            },
            {
                "box": {
                    "fontsize": 10.0,
                    "id": "obj-4",
                    "maxclass": "textbutton",
                    "numinlets": 1,
                    "numoutlets": 3,
                    "outlettype": [ "", "", "int" ],
                    "parameter_enable": 0,
                    "patching_rect": [ 132.0, 56.0, 48.0, 23.0 ],
                    "presentation": 1,
                    "presentation_rect": [ 2.0, 3.0, 44.0, 19.0 ],
                    "text": "run"
                }
            },
            {
                "box": {
                    "fontsize": 10.0,
                    "id": "obj-3",
                    "maxclass": "textbutton",
                    "numinlets": 1,
                    "numoutlets": 3,
                    "outlettype": [ "", "", "int" ],
                    "parameter_enable": 0,
                    "patching_rect": [ 185.0, 56.0, 47.0, 23.0 ],
                    "presentation": 1,
                    "presentation_rect": [ 48.0, 3.0, 44.0, 19.0 ],
                    "text": "clear"
                }
            },
            {
                "box": {
                    "comment": "",
                    "id": "obj-1",
                    "index": 0,
                    "maxclass": "outlet",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 132.0, 410.0, 30.0, 30.0 ]
                }
            },
            {
                "box": {
                    "id": "obj-9",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 234.0, 57.0, 36.0, 22.0 ],
                    "text": "clear"
                }
            },
            {
                "box": {
                    "fontsize": 10.0,
                    "id": "obj-28",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 274.0, 58.0, 173.0, 18.0 ],
                    "presentation": 1,
                    "presentation_rect": [ 96.0, 4.0, 297.0, 18.0 ],
                    "text": "Enter ChucK code here",
                    "textjustification": 1
                }
            }
        ],
        "lines": [
            {
                "patchline": {
                    "destination": [ "obj-5", 0 ],
                    "source": [ "obj-10", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-10", 0 ],
                    "source": [ "obj-2", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-9", 0 ],
                    "source": [ "obj-3", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-2", 0 ],
                    "source": [ "obj-4", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-1", 0 ],
                    "source": [ "obj-5", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-7", 0 ],
                    "source": [ "obj-8", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-2", 0 ],
                    "source": [ "obj-9", 0 ]
                }
            }
        ],
        "autosave": 0
    }
}