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
  "rect": [
   80.0,
   80.0,
   1000.0,
   870.0
  ],
  "default_fontsize": 10.0,
  "default_fontname": "Verdana",
  "gridsize": [
   5.0,
   5.0
  ],
  "description": "Demonstrates chuck~ sample-rate tracking, multichannel taps, the reply outlet and the abort message",
  "boxes": [
   {
    "box": {
     "id": "obj-title",
     "maxclass": "comment",
     "numinlets": 1,
     "numoutlets": 0,
     "patching_rect": [
      20.0,
      15.0,
      620.0,
      20.0
     ],
     "text": "chuck~ : new features (sample rate, taps, reply outlet, abort)",
     "fontsize": 14.0,
     "fontface": 1
    }
   },
   {
    "box": {
     "id": "obj-intro",
     "maxclass": "comment",
     "numinlets": 1,
     "numoutlets": 0,
     "patching_rect": [
      20.0,
      42.0,
      900.0,
      35.0
     ],
     "text": "Load examples/test/help_features.ck with 'run', then turn on audio. The rightmost outlet is new: it reports 'get' values, event notifications and shred lifecycle changes as lists.",
     "linecount": 2
    }
   },
   {
    "box": {
     "id": "c-1.",
     "maxclass": "comment",
     "numinlets": 1,
     "numoutlets": 0,
     "patching_rect": [
      20.0,
      96.0,
      195.0,
      20.0
     ],
     "text": "1. Transport",
     "fontface": 1
    }
   },
   {
    "box": {
     "id": "m-run",
     "maxclass": "message",
     "numinlets": 2,
     "numoutlets": 1,
     "outlettype": [
      ""
     ],
     "patching_rect": [
      20.0,
      118.0,
      195.0,
      22.0
     ],
     "text": "run test/help_features.ck"
    }
   },
   {
    "box": {
     "id": "n-m-run",
     "maxclass": "comment",
     "numinlets": 1,
     "numoutlets": 0,
     "patching_rect": [
      26.0,
      140.0,
      189.0,
      20.0
     ],
     "text": "loads the companion ChucK file"
    }
   },
   {
    "box": {
     "id": "m-abort",
     "maxclass": "message",
     "numinlets": 2,
     "numoutlets": 1,
     "outlettype": [
      ""
     ],
     "patching_rect": [
      20.0,
      162.0,
      195.0,
      22.0
     ],
     "text": "abort"
    }
   },
   {
    "box": {
     "id": "n-m-abort",
     "maxclass": "comment",
     "numinlets": 1,
     "numoutlets": 0,
     "patching_rect": [
      26.0,
      184.0,
      189.0,
      20.0
     ],
     "text": "NEW: only bites on a shred stuck inside the VM"
    }
   },
   {
    "box": {
     "id": "m-removeall",
     "maxclass": "message",
     "numinlets": 2,
     "numoutlets": 1,
     "outlettype": [
      ""
     ],
     "patching_rect": [
      20.0,
      206.0,
      195.0,
      22.0
     ],
     "text": "removeall"
    }
   },
   {
    "box": {
     "id": "n-m-removeall",
     "maxclass": "comment",
     "numinlets": 1,
     "numoutlets": 0,
     "patching_rect": [
      26.0,
      228.0,
      189.0,
      20.0
     ],
     "text": "removes shreds; global UGens keep sounding"
    }
   },
   {
    "box": {
     "id": "m-reset",
     "maxclass": "message",
     "numinlets": 2,
     "numoutlets": 1,
     "outlettype": [
      ""
     ],
     "patching_rect": [
      20.0,
      250.0,
      195.0,
      22.0
     ],
     "text": "reset"
    }
   },
   {
    "box": {
     "id": "n-m-reset",
     "maxclass": "comment",
     "numinlets": 1,
     "numoutlets": 0,
     "patching_rect": [
      26.0,
      272.0,
      189.0,
      20.0
     ],
     "text": "clears the VM too, so globals stop"
    }
   },
   {
    "box": {
     "id": "m-status",
     "maxclass": "message",
     "numinlets": 2,
     "numoutlets": 1,
     "outlettype": [
      ""
     ],
     "patching_rect": [
      20.0,
      294.0,
      195.0,
      22.0
     ],
     "text": "status"
    }
   },
   {
    "box": {
     "id": "c-2.",
     "maxclass": "comment",
     "numinlets": 1,
     "numoutlets": 0,
     "patching_rect": [
      230.0,
      96.0,
      175.0,
      20.0
     ],
     "text": "2. Read globals",
     "fontface": 1
    }
   },
   {
    "box": {
     "id": "m-get-int",
     "maxclass": "message",
     "numinlets": 2,
     "numoutlets": 1,
     "outlettype": [
      ""
     ],
     "patching_rect": [
      230.0,
      118.0,
      175.0,
      22.0
     ],
     "text": "get int counter"
    }
   },
   {
    "box": {
     "id": "n-m-get-int",
     "maxclass": "comment",
     "numinlets": 1,
     "numoutlets": 0,
     "patching_rect": [
      236.0,
      140.0,
      169.0,
      20.0
     ],
     "text": "replies as: val counter 17"
    }
   },
   {
    "box": {
     "id": "m-get-float",
     "maxclass": "message",
     "numinlets": 2,
     "numoutlets": 1,
     "outlettype": [
      ""
     ],
     "patching_rect": [
      230.0,
      162.0,
      175.0,
      22.0
     ],
     "text": "get float freq"
    }
   },
   {
    "box": {
     "id": "m-get-string",
     "maxclass": "message",
     "numinlets": 2,
     "numoutlets": 1,
     "outlettype": [
      ""
     ],
     "patching_rect": [
      230.0,
      188.0,
      175.0,
      22.0
     ],
     "text": "get string label"
    }
   },
   {
    "box": {
     "id": "m-get-ia",
     "maxclass": "message",
     "numinlets": 2,
     "numoutlets": 1,
     "outlettype": [
      ""
     ],
     "patching_rect": [
      230.0,
      214.0,
      175.0,
      22.0
     ],
     "text": "get int[] steps"
    }
   },
   {
    "box": {
     "id": "m-get-fa",
     "maxclass": "message",
     "numinlets": 2,
     "numoutlets": 1,
     "outlettype": [
      ""
     ],
     "patching_rect": [
      230.0,
      240.0,
      175.0,
      22.0
     ],
     "text": "get float[] levels"
    }
   },
   {
    "box": {
     "id": "m-get-idx",
     "maxclass": "message",
     "numinlets": 2,
     "numoutlets": 1,
     "outlettype": [
      ""
     ],
     "patching_rect": [
      230.0,
      266.0,
      175.0,
      22.0
     ],
     "text": "get int[] steps 3"
    }
   },
   {
    "box": {
     "id": "n-m-get-idx",
     "maxclass": "comment",
     "numinlets": 1,
     "numoutlets": 0,
     "patching_rect": [
      236.0,
      288.0,
      169.0,
      20.0
     ],
     "text": "single element by index"
    }
   },
   {
    "box": {
     "id": "c-3.",
     "maxclass": "comment",
     "numinlets": 1,
     "numoutlets": 0,
     "patching_rect": [
      420.0,
      96.0,
      150.0,
      20.0
     ],
     "text": "3. Events",
     "fontface": 1
    }
   },
   {
    "box": {
     "id": "m-listen",
     "maxclass": "message",
     "numinlets": 2,
     "numoutlets": 1,
     "outlettype": [
      ""
     ],
     "patching_rect": [
      420.0,
      118.0,
      150.0,
      22.0
     ],
     "text": "listen tick 1"
    }
   },
   {
    "box": {
     "id": "n-m-listen",
     "maxclass": "comment",
     "numinlets": 1,
     "numoutlets": 0,
     "patching_rect": [
      426.0,
      140.0,
      144.0,
      20.0
     ],
     "text": "NEW: fires 'event tick' out the right outlet"
    }
   },
   {
    "box": {
     "id": "m-unlisten",
     "maxclass": "message",
     "numinlets": 2,
     "numoutlets": 1,
     "outlettype": [
      ""
     ],
     "patching_rect": [
      420.0,
      162.0,
      150.0,
      22.0
     ],
     "text": "unlisten tick"
    }
   },
   {
    "box": {
     "id": "m-sig",
     "maxclass": "message",
     "numinlets": 2,
     "numoutlets": 1,
     "outlettype": [
      ""
     ],
     "patching_rect": [
      420.0,
      188.0,
      150.0,
      22.0
     ],
     "text": "sig tick"
    }
   },
   {
    "box": {
     "id": "m-broadcast",
     "maxclass": "message",
     "numinlets": 2,
     "numoutlets": 1,
     "outlettype": [
      ""
     ],
     "patching_rect": [
      420.0,
      214.0,
      150.0,
      22.0
     ],
     "text": "broadcast tick"
    }
   },
   {
    "box": {
     "id": "c-4.",
     "maxclass": "comment",
     "numinlets": 1,
     "numoutlets": 0,
     "patching_rect": [
      585.0,
      96.0,
      160.0,
      20.0
     ],
     "text": "4. Taps",
     "fontface": 1
    }
   },
   {
    "box": {
     "id": "m-tap-mono",
     "maxclass": "message",
     "numinlets": 2,
     "numoutlets": 1,
     "outlettype": [
      ""
     ],
     "patching_rect": [
      585.0,
      118.0,
      160.0,
      22.0
     ],
     "text": "tap 1 mono"
    }
   },
   {
    "box": {
     "id": "n-m-tap-mono",
     "maxclass": "comment",
     "numinlets": 1,
     "numoutlets": 0,
     "patching_rect": [
      591.0,
      140.0,
      154.0,
      20.0
     ],
     "text": "mono UGen into tap outlet 1"
    }
   },
   {
    "box": {
     "id": "m-tap-stereo",
     "maxclass": "message",
     "numinlets": 2,
     "numoutlets": 1,
     "outlettype": [
      ""
     ],
     "patching_rect": [
      585.0,
      162.0,
      160.0,
      22.0
     ],
     "text": "tap 2 stereo 2"
    }
   },
   {
    "box": {
     "id": "n-m-tap-stereo",
     "maxclass": "comment",
     "numinlets": 1,
     "numoutlets": 0,
     "patching_rect": [
      591.0,
      184.0,
      154.0,
      20.0
     ],
     "text": "NEW: 2-channel UGen spans taps 2-3"
    }
   },
   {
    "box": {
     "id": "m-tap-clear",
     "maxclass": "message",
     "numinlets": 2,
     "numoutlets": 1,
     "outlettype": [
      ""
     ],
     "patching_rect": [
      585.0,
      206.0,
      160.0,
      22.0
     ],
     "text": "tap"
    }
   },
   {
    "box": {
     "id": "n-m-tap-clear",
     "maxclass": "comment",
     "numinlets": 1,
     "numoutlets": 0,
     "patching_rect": [
      591.0,
      228.0,
      154.0,
      20.0
     ],
     "text": "clear all taps"
    }
   },
   {
    "box": {
     "id": "m-globals",
     "maxclass": "message",
     "numinlets": 2,
     "numoutlets": 1,
     "outlettype": [
      ""
     ],
     "patching_rect": [
      585.0,
      250.0,
      160.0,
      22.0
     ],
     "text": "globals"
    }
   },
   {
    "box": {
     "id": "n-m-globals",
     "maxclass": "comment",
     "numinlets": 1,
     "numoutlets": 0,
     "patching_rect": [
      591.0,
      272.0,
      154.0,
      20.0
     ],
     "text": "lists globals out the right outlet"
    }
   },
   {
    "box": {
     "id": "c-5.",
     "maxclass": "comment",
     "numinlets": 1,
     "numoutlets": 0,
     "patching_rect": [
      760.0,
      96.0,
      190.0,
      20.0
     ],
     "text": "5. VM params",
     "fontface": 1
    }
   },
   {
    "box": {
     "id": "m-param",
     "maxclass": "message",
     "numinlets": 2,
     "numoutlets": 1,
     "outlettype": [
      ""
     ],
     "patching_rect": [
      760.0,
      118.0,
      190.0,
      22.0
     ],
     "text": "param"
    }
   },
   {
    "box": {
     "id": "n-m-param",
     "maxclass": "comment",
     "numinlets": 1,
     "numoutlets": 0,
     "patching_rect": [
      766.0,
      140.0,
      184.0,
      20.0
     ],
     "text": "dumps every param"
    }
   },
   {
    "box": {
     "id": "m-param-otf",
     "maxclass": "message",
     "numinlets": 2,
     "numoutlets": 1,
     "outlettype": [
      ""
     ],
     "patching_rect": [
      760.0,
      162.0,
      190.0,
      22.0
     ],
     "text": "param OTF_PRINT_WARNINGS"
    }
   },
   {
    "box": {
     "id": "n-m-param-otf",
     "maxclass": "comment",
     "numinlets": 1,
     "numoutlets": 0,
     "patching_rect": [
      766.0,
      184.0,
      184.0,
      20.0
     ],
     "text": "NEW: was missing from the table"
    }
   },
   {
    "box": {
     "id": "m-param-rt",
     "maxclass": "message",
     "numinlets": 2,
     "numoutlets": 1,
     "outlettype": [
      ""
     ],
     "patching_rect": [
      760.0,
      206.0,
      190.0,
      22.0
     ],
     "text": "param IS_REALTIME_AUDIO_HINT"
    }
   },
   {
    "box": {
     "id": "n-m-param-rt",
     "maxclass": "comment",
     "numinlets": 1,
     "numoutlets": 0,
     "patching_rect": [
      766.0,
      228.0,
      184.0,
      20.0
     ],
     "text": "NEW: now set to 1 at startup"
    }
   },
   {
    "box": {
     "id": "obj-ck-note",
     "maxclass": "comment",
     "numinlets": 1,
     "numoutlets": 0,
     "patching_rect": [
      20.0,
      372.0,
      900.0,
      35.0
     ],
     "text": "@ntap 3 creates three tap outlets after the audio outlet, and the reply outlet sits to the right of those. Left to right: audio, tap 1-3, replies.",
     "linecount": 2
    }
   },
   {
    "box": {
     "id": "obj-chuck",
     "maxclass": "newobj",
     "numinlets": 1,
     "numoutlets": 5,
     "patching_rect": [
      20.0,
      415.0,
      180.0,
      22.0
     ],
     "text": "chuck~ @ntap 3",
     "outlettype": [
      "signal",
      "signal",
      "signal",
      "signal",
      ""
     ]
    }
   },
   {
    "box": {
     "id": "obj-onoff",
     "maxclass": "newobj",
     "numinlets": 2,
     "numoutlets": 0,
     "patching_rect": [
      20.0,
      462.0,
      60.0,
      22.0
     ],
     "text": "ezdac~"
    }
   },
   {
    "box": {
     "id": "obj-scope-lbl",
     "maxclass": "comment",
     "numinlets": 1,
     "numoutlets": 0,
     "patching_rect": [
      20.0,
      500.0,
      290.0,
      35.0
     ],
     "text": "Left scope is the mono tap; the right pair carry the two channels of the stereo Pan2 global, which the pan sweep moves in opposition.",
     "linecount": 2
    }
   },
   {
    "box": {
     "id": "obj-scope-m",
     "maxclass": "newobj",
     "numinlets": 2,
     "numoutlets": 0,
     "patching_rect": [
      20.0,
      545.0,
      90.0,
      22.0
     ],
     "text": "scope~"
    }
   },
   {
    "box": {
     "id": "obj-scope-l",
     "maxclass": "newobj",
     "numinlets": 2,
     "numoutlets": 0,
     "patching_rect": [
      115.0,
      545.0,
      90.0,
      22.0
     ],
     "text": "scope~"
    }
   },
   {
    "box": {
     "id": "obj-scope-r",
     "maxclass": "newobj",
     "numinlets": 2,
     "numoutlets": 0,
     "patching_rect": [
      210.0,
      545.0,
      90.0,
      22.0
     ],
     "text": "scope~"
    }
   },
   {
    "box": {
     "id": "obj-reply-lbl",
     "maxclass": "comment",
     "numinlets": 1,
     "numoutlets": 0,
     "patching_rect": [
      330.0,
      500.0,
      420.0,
      65.0
     ],
     "text": "Reply outlet. Selectors are always ours and ChucK names travel as arguments, so no global can collide: 'val <name> <value>', 'event <name>', 'shred add|remove <id>', 'global <name> <type>'. Route with 'route val event shred global'.",
     "linecount": 4
    }
   },
   {
    "box": {
     "id": "obj-route",
     "maxclass": "newobj",
     "numinlets": 1,
     "numoutlets": 5,
     "patching_rect": [
      330.0,
      570.0,
      210.0,
      22.0
     ],
     "text": "route val event shred global",
     "outlettype": [
      "",
      "",
      "",
      "",
      ""
     ]
    }
   },
   {
    "box": {
     "id": "obj-print-val",
     "maxclass": "newobj",
     "numinlets": 1,
     "numoutlets": 0,
     "patching_rect": [
      330.0,
      610.0,
      95.0,
      22.0
     ],
     "text": "print VAL"
    }
   },
   {
    "box": {
     "id": "obj-print-evt",
     "maxclass": "newobj",
     "numinlets": 1,
     "numoutlets": 0,
     "patching_rect": [
      435.0,
      610.0,
      105.0,
      22.0
     ],
     "text": "print EVENT"
    }
   },
   {
    "box": {
     "id": "obj-print-shr",
     "maxclass": "newobj",
     "numinlets": 1,
     "numoutlets": 0,
     "patching_rect": [
      550.0,
      610.0,
      105.0,
      22.0
     ],
     "text": "print SHRED"
    }
   },
   {
    "box": {
     "id": "obj-print-glb",
     "maxclass": "newobj",
     "numinlets": 1,
     "numoutlets": 0,
     "patching_rect": [
      665.0,
      610.0,
      110.0,
      22.0
     ],
     "text": "print GLOBAL"
    }
   },
   {
    "box": {
     "id": "obj-sr-lbl",
     "maxclass": "comment",
     "numinlets": 1,
     "numoutlets": 0,
     "patching_rect": [
      20.0,
      650.0,
      900.0,
      50.0
     ],
     "text": "Sample rate: change the rate in Max's Audio Status window while this patch runs. chuck~ now follows the host rate; previously the VM stayed at the rate captured when the object was created, so pitch and timing drifted.",
     "linecount": 3
    }
   },
   {
    "box": {
     "id": "obj-removeall-lbl",
     "maxclass": "comment",
     "numinlets": 1,
     "numoutlets": 0,
     "patching_rect": [
      20.0,
      710.0,
      900.0,
      50.0
     ],
     "text": "removeall vs reset: help_features.ck makes its sound from global UGens (needed for tapping). Global UGens belong to the VM, not to any shred, so 'removeall' empties the shred list while the sound continues. Use 'reset' to clear the VM and globals as well.",
     "linecount": 3
    }
   },
   {
    "box": {
     "id": "obj-multi-lbl",
     "maxclass": "comment",
     "numinlets": 1,
     "numoutlets": 0,
     "patching_rect": [
      20.0,
      770.0,
      900.0,
      50.0
     ],
     "text": "Multiple instances: duplicate this chuck~ object and confirm each one's 'get' replies come out of its own reply outlet rather than being mixed together or attributed to the last object created.",
     "linecount": 3
    }
   }
  ],
  "lines": [
   {
    "patchline": {
     "destination": [
      "obj-chuck",
      0
     ],
     "source": [
      "m-run",
      0
     ]
    }
   },
   {
    "patchline": {
     "destination": [
      "obj-chuck",
      0
     ],
     "source": [
      "m-abort",
      0
     ]
    }
   },
   {
    "patchline": {
     "destination": [
      "obj-chuck",
      0
     ],
     "source": [
      "m-removeall",
      0
     ]
    }
   },
   {
    "patchline": {
     "destination": [
      "obj-chuck",
      0
     ],
     "source": [
      "m-reset",
      0
     ]
    }
   },
   {
    "patchline": {
     "destination": [
      "obj-chuck",
      0
     ],
     "source": [
      "m-status",
      0
     ]
    }
   },
   {
    "patchline": {
     "destination": [
      "obj-chuck",
      0
     ],
     "source": [
      "m-get-int",
      0
     ]
    }
   },
   {
    "patchline": {
     "destination": [
      "obj-chuck",
      0
     ],
     "source": [
      "m-get-float",
      0
     ]
    }
   },
   {
    "patchline": {
     "destination": [
      "obj-chuck",
      0
     ],
     "source": [
      "m-get-string",
      0
     ]
    }
   },
   {
    "patchline": {
     "destination": [
      "obj-chuck",
      0
     ],
     "source": [
      "m-get-ia",
      0
     ]
    }
   },
   {
    "patchline": {
     "destination": [
      "obj-chuck",
      0
     ],
     "source": [
      "m-get-fa",
      0
     ]
    }
   },
   {
    "patchline": {
     "destination": [
      "obj-chuck",
      0
     ],
     "source": [
      "m-get-idx",
      0
     ]
    }
   },
   {
    "patchline": {
     "destination": [
      "obj-chuck",
      0
     ],
     "source": [
      "m-listen",
      0
     ]
    }
   },
   {
    "patchline": {
     "destination": [
      "obj-chuck",
      0
     ],
     "source": [
      "m-unlisten",
      0
     ]
    }
   },
   {
    "patchline": {
     "destination": [
      "obj-chuck",
      0
     ],
     "source": [
      "m-sig",
      0
     ]
    }
   },
   {
    "patchline": {
     "destination": [
      "obj-chuck",
      0
     ],
     "source": [
      "m-broadcast",
      0
     ]
    }
   },
   {
    "patchline": {
     "destination": [
      "obj-chuck",
      0
     ],
     "source": [
      "m-tap-mono",
      0
     ]
    }
   },
   {
    "patchline": {
     "destination": [
      "obj-chuck",
      0
     ],
     "source": [
      "m-tap-stereo",
      0
     ]
    }
   },
   {
    "patchline": {
     "destination": [
      "obj-chuck",
      0
     ],
     "source": [
      "m-tap-clear",
      0
     ]
    }
   },
   {
    "patchline": {
     "destination": [
      "obj-chuck",
      0
     ],
     "source": [
      "m-globals",
      0
     ]
    }
   },
   {
    "patchline": {
     "destination": [
      "obj-chuck",
      0
     ],
     "source": [
      "m-param",
      0
     ]
    }
   },
   {
    "patchline": {
     "destination": [
      "obj-chuck",
      0
     ],
     "source": [
      "m-param-otf",
      0
     ]
    }
   },
   {
    "patchline": {
     "destination": [
      "obj-chuck",
      0
     ],
     "source": [
      "m-param-rt",
      0
     ]
    }
   },
   {
    "patchline": {
     "destination": [
      "obj-onoff",
      0
     ],
     "source": [
      "obj-chuck",
      0
     ]
    }
   },
   {
    "patchline": {
     "destination": [
      "obj-onoff",
      1
     ],
     "source": [
      "obj-chuck",
      0
     ]
    }
   },
   {
    "patchline": {
     "destination": [
      "obj-scope-m",
      0
     ],
     "source": [
      "obj-chuck",
      1
     ]
    }
   },
   {
    "patchline": {
     "destination": [
      "obj-scope-l",
      0
     ],
     "source": [
      "obj-chuck",
      2
     ]
    }
   },
   {
    "patchline": {
     "destination": [
      "obj-scope-r",
      0
     ],
     "source": [
      "obj-chuck",
      3
     ]
    }
   },
   {
    "patchline": {
     "destination": [
      "obj-route",
      0
     ],
     "source": [
      "obj-chuck",
      4
     ]
    }
   },
   {
    "patchline": {
     "destination": [
      "obj-print-val",
      0
     ],
     "source": [
      "obj-route",
      0
     ]
    }
   },
   {
    "patchline": {
     "destination": [
      "obj-print-evt",
      0
     ],
     "source": [
      "obj-route",
      1
     ]
    }
   },
   {
    "patchline": {
     "destination": [
      "obj-print-shr",
      0
     ],
     "source": [
      "obj-route",
      2
     ]
    }
   },
   {
    "patchline": {
     "destination": [
      "obj-print-glb",
      0
     ],
     "source": [
      "obj-route",
      3
     ]
    }
   }
  ],
  "autosave": 0
 }
}
