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
        "rect": [ 100.0, 100.0, 900.0, 700.0 ],
        "default_fontsize": 10.0,
        "default_fontname": "Verdana",
        "gridsize": [ 5.0, 5.0 ],
        "description": "Test the tap feature with two tap outlets",
        "boxes": [
            {
                "box": {
                    "fontface": 1,
                    "fontsize": 14.0,
                    "id": "obj-title",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 20.0, 20.0, 600.0, 24.0 ],
                    "text": "Testing chuck~ tap feature with two tap outlets"
                }
            },
            {
                "box": {
                    "id": "obj-desc1",
                    "linecount": 2,
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 20.0, 48.0, 600.0, 31.0 ],
                    "text": "The @ntap 2 attribute creates two independent tap outlets.\nUse 'tap <outlet> <name>' to assign each outlet: tap 1 osc1, tap 2 osc2"
                }
            },
            {
                "box": {
                    "id": "obj-run",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 150.0, 120.0, 140.0, 21.0 ],
                    "text": "run test/tap_test2.ck"
                }
            },
            {
                "box": {
                    "id": "obj-tap-osc1",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 310.0, 120.0, 65.0, 21.0 ],
                    "text": "tap 1 osc1"
                }
            },
            {
                "box": {
                    "id": "obj-tap-osc2",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 385.0, 120.0, 65.0, 21.0 ],
                    "text": "tap 2 osc2"
                }
            },
            {
                "box": {
                    "id": "obj-tap-clear",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 470.0, 120.0, 30.0, 21.0 ],
                    "text": "tap"
                }
            },
            {
                "box": {
                    "id": "obj-status",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 520.0, 120.0, 39.0, 21.0 ],
                    "text": "status"
                }
            },
            {
                "box": {
                    "id": "obj-reset",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 575.0, 120.0, 34.0, 21.0 ],
                    "text": "reset"
                }
            },
            {
                "box": {
                    "id": "obj-button",
                    "maxclass": "button",
                    "numinlets": 1,
                    "numoutlets": 1,
                    "outlettype": [ "bang" ],
                    "parameter_enable": 0,
                    "patching_rect": [ 150.0, 160.0, 24.0, 24.0 ]
                }
            },
            {
                "box": {
                    "id": "obj-label-run",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 178.0, 164.0, 30.0, 19.0 ],
                    "text": "run"
                }
            },
            {
                "box": {
                    "fontname": "Verdana",
                    "fontsize": 10.0,
                    "id": "obj-chuck",
                    "maxclass": "newobj",
                    "numinlets": 1,
                    "numoutlets": 3,
                    "outlettype": [ "signal", "signal", "signal" ],
                    "patching_rect": [ 150.0, 210.0, 220.0, 21.0 ],
                    "text": "chuck~ 1 @ntap 2 test/tap_test2.ck",
                    "varname": "chuck-0"
                }
            },
            {
                "box": {
                    "id": "obj-label-main",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 50.0, 280.0, 120.0, 19.0 ],
                    "text": "Main audio output:"
                }
            },
            {
                "box": {
                    "id": "obj-scope-main",
                    "maxclass": "scope~",
                    "numinlets": 2,
                    "numoutlets": 0,
                    "patching_rect": [ 50.0, 300.0, 180.0, 130.0 ]
                }
            },
            {
                "box": {
                    "id": "obj-label-tap1",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 260.0, 280.0, 120.0, 19.0 ],
                    "text": "Tap outlet 1:"
                }
            },
            {
                "box": {
                    "id": "obj-scope-tap1",
                    "maxclass": "scope~",
                    "numinlets": 2,
                    "numoutlets": 0,
                    "patching_rect": [ 260.0, 300.0, 180.0, 130.0 ]
                }
            },
            {
                "box": {
                    "id": "obj-label-tap2",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 470.0, 280.0, 120.0, 19.0 ],
                    "text": "Tap outlet 2:"
                }
            },
            {
                "box": {
                    "id": "obj-scope-tap2",
                    "maxclass": "scope~",
                    "numinlets": 2,
                    "numoutlets": 0,
                    "patching_rect": [ 470.0, 300.0, 180.0, 130.0 ]
                }
            },
            {
                "box": {
                    "id": "obj-meter-tap1",
                    "maxclass": "meter~",
                    "numinlets": 1,
                    "numoutlets": 1,
                    "outlettype": [ "float" ],
                    "patching_rect": [ 680.0, 300.0, 20.0, 130.0 ]
                }
            },
            {
                "box": {
                    "id": "obj-meter-tap2",
                    "maxclass": "meter~",
                    "numinlets": 1,
                    "numoutlets": 1,
                    "outlettype": [ "float" ],
                    "patching_rect": [ 710.0, 300.0, 20.0, 130.0 ]
                }
            },
            {
                "box": {
                    "id": "obj-gain",
                    "lastchannelcount": 0,
                    "maxclass": "live.gain~",
                    "numinlets": 2,
                    "numoutlets": 5,
                    "orientation": 1,
                    "outlettype": [ "signal", "signal", "", "float", "list" ],
                    "parameter_enable": 1,
                    "patching_rect": [ 50.0, 470.0, 140.0, 47.0 ],
                    "saved_attribute_attributes": {
                        "valueof": {
                            "parameter_longname": "live.gain~",
                            "parameter_mmax": 6.0,
                            "parameter_mmin": -70.0,
                            "parameter_modmode": 3,
                            "parameter_shortname": "live.gain~",
                            "parameter_type": 0,
                            "parameter_unitstyle": 4
                        }
                    },
                    "varname": "live.gain~"
                }
            },
            {
                "box": {
                    "id": "obj-dac",
                    "maxclass": "ezdac~",
                    "numinlets": 2,
                    "numoutlets": 0,
                    "patching_rect": [ 50.0, 540.0, 45.0, 45.0 ]
                }
            },
            {
                "box": {
                    "id": "obj-instructions",
                    "linecount": 6,
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 260.0, 450.0, 300.0, 79.0 ],
                    "text": "Instructions:\n1. Turn on audio (click ezdac~)\n2. Click 'run' or bang to load ChucK file\n3. Click 'tap 1 osc1' and 'tap 2 osc2' to tap independently\n4. Observe: outlet 1=slow sine 2Hz, outlet 2=square 5Hz\n5. 'tap' clears all taps"
                }
            }
        ],
        "lines": [
            {
                "patchline": {
                    "destination": [ "obj-chuck", 0 ],
                    "source": [ "obj-button", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-gain", 0 ],
                    "order": 0,
                    "source": [ "obj-chuck", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-meter-tap1", 0 ],
                    "order": 0,
                    "source": [ "obj-chuck", 1 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-meter-tap2", 0 ],
                    "order": 0,
                    "source": [ "obj-chuck", 2 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-scope-main", 0 ],
                    "order": 1,
                    "source": [ "obj-chuck", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-scope-tap1", 0 ],
                    "order": 1,
                    "source": [ "obj-chuck", 1 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-scope-tap2", 0 ],
                    "order": 1,
                    "source": [ "obj-chuck", 2 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-dac", 0 ],
                    "source": [ "obj-gain", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-chuck", 0 ],
                    "source": [ "obj-reset", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-chuck", 0 ],
                    "source": [ "obj-run", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-chuck", 0 ],
                    "source": [ "obj-status", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-chuck", 0 ],
                    "source": [ "obj-tap-clear", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-chuck", 0 ],
                    "source": [ "obj-tap-osc1", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-chuck", 0 ],
                    "source": [ "obj-tap-osc2", 0 ]
                }
            }
        ],
        "parameters": {
            "obj-gain": [ "live.gain~", "live.gain~", 0 ],
            "parameterbanks": {
                "0": {
                    "index": 0,
                    "name": "",
                    "parameters": [ "-", "-", "-", "-", "-", "-", "-", "-" ],
                    "buttons": [ "-", "-", "-", "-", "-", "-", "-", "-" ]
                }
            },
            "inherited_shortname": 1
        },
        "autosave": 0
    }
}