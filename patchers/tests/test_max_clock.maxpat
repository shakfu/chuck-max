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
  "rect": [
   100.0,
   100.0,
   720.0,
   400.0
  ],
  "default_fontsize": 12.0,
  "default_fontface": 0,
  "default_fontname": "Arial",
  "gridsize": [
   15.0,
   15.0
  ],
  "boxes": [
   {
    "box": {
     "id": "c-title",
     "maxclass": "comment",
     "numinlets": 1,
     "numoutlets": 0,
     "patching_rect": [
      20,
      10,
      680,
      64
     ],
     "text": "MaxClock test. 1) DSP on. 2) add max_clock_check.ck, start transport: err (samples) should stay within +-1. 3) add max_clock.ck: clicks on each beat, beat 1 higher; beat box counts 1-4. 4) change tempo, stop/start, seek to 0 (right inlet of transport): clicks follow without a late or doubled click."
    }
   },
   {
    "box": {
     "id": "ezdac",
     "maxclass": "ezdac~",
     "numinlets": 2,
     "numoutlets": 0,
     "patching_rect": [
      20,
      85,
      45,
      22.0
     ]
    }
   },
   {
    "box": {
     "id": "tog-t",
     "maxclass": "toggle",
     "numinlets": 1,
     "numoutlets": 1,
     "patching_rect": [
      100,
      85,
      24,
      22.0
     ],
     "outlettype": [
      "int"
     ],
     "parameter_enable": 0
    }
   },
   {
    "box": {
     "id": "c-tr",
     "maxclass": "comment",
     "numinlets": 1,
     "numoutlets": 0,
     "patching_rect": [
      128,
      85,
      100,
      22.0
     ],
     "text": "transport"
    }
   },
   {
    "box": {
     "id": "tempo-num",
     "maxclass": "flonum",
     "numinlets": 1,
     "numoutlets": 2,
     "patching_rect": [
      230,
      85,
      50,
      22.0
     ],
     "outlettype": [
      "",
      "bang"
     ],
     "parameter_enable": 0
    }
   },
   {
    "box": {
     "id": "tempo-msg",
     "maxclass": "message",
     "numinlets": 2,
     "numoutlets": 1,
     "patching_rect": [
      230,
      115,
      60,
      22.0
     ],
     "outlettype": [
      ""
     ],
     "text": "tempo $1"
    }
   },
   {
    "box": {
     "id": "seek",
     "maxclass": "message",
     "numinlets": 2,
     "numoutlets": 1,
     "patching_rect": [
      300,
      115,
      30,
      22.0
     ],
     "outlettype": [
      ""
     ],
     "text": "0"
    }
   },
   {
    "box": {
     "id": "transport",
     "maxclass": "newobj",
     "numinlets": 2,
     "numoutlets": 9,
     "patching_rect": [
      100,
      150,
      70,
      22.0
     ],
     "outlettype": [
      "int",
      "int",
      "float",
      "float",
      "float",
      "",
      "int",
      "float",
      ""
     ],
     "text": "transport"
    }
   },
   {
    "box": {
     "id": "phasor",
     "maxclass": "newobj",
     "numinlets": 2,
     "numoutlets": 1,
     "patching_rect": [
      20,
      200,
      130,
      22.0
     ],
     "outlettype": [
      "signal"
     ],
     "text": "phasor~ 4n @lock 1"
    }
   },
   {
    "box": {
     "id": "m-check",
     "maxclass": "message",
     "numinlets": 2,
     "numoutlets": 1,
     "patching_rect": [
      170,
      200,
      175,
      22.0
     ],
     "outlettype": [
      ""
     ],
     "text": "add max/max_clock_check.ck"
    }
   },
   {
    "box": {
     "id": "m-clock",
     "maxclass": "message",
     "numinlets": 2,
     "numoutlets": 1,
     "patching_rect": [
      355,
      200,
      135,
      22.0
     ],
     "outlettype": [
      ""
     ],
     "text": "add max/max_clock.ck"
    }
   },
   {
    "box": {
     "id": "m-rm",
     "maxclass": "message",
     "numinlets": 2,
     "numoutlets": 1,
     "patching_rect": [
      500,
      200,
      70,
      22.0
     ],
     "outlettype": [
      ""
     ],
     "text": "removeall"
    }
   },
   {
    "box": {
     "id": "ck",
     "maxclass": "newobj",
     "numinlets": 1,
     "numoutlets": 2,
     "patching_rect": [
      20,
      250,
      60,
      22.0
     ],
     "outlettype": [
      "signal",
      ""
     ],
     "text": "chuck~ 1"
    }
   },
   {
    "box": {
     "id": "gain",
     "maxclass": "newobj",
     "numinlets": 2,
     "numoutlets": 1,
     "patching_rect": [
      20,
      285,
      50,
      22.0
     ],
     "outlettype": [
      "signal"
     ],
     "text": "*~ 0.5"
    }
   },
   {
    "box": {
     "id": "dac",
     "maxclass": "newobj",
     "numinlets": 2,
     "numoutlets": 0,
     "patching_rect": [
      20,
      315,
      40,
      22.0
     ],
     "text": "dac~"
    }
   },
   {
    "box": {
     "id": "route",
     "maxclass": "newobj",
     "numinlets": 2,
     "numoutlets": 2,
     "patching_rect": [
      120,
      285,
      70,
      22.0
     ],
     "outlettype": [
      "",
      ""
     ],
     "text": "route msg"
    }
   },
   {
    "box": {
     "id": "route2",
     "maxclass": "newobj",
     "numinlets": 3,
     "numoutlets": 3,
     "patching_rect": [
      120,
      315,
      90,
      22.0
     ],
     "outlettype": [
      "",
      "",
      ""
     ],
     "text": "route beat err"
    }
   },
   {
    "box": {
     "id": "n-beat",
     "maxclass": "number",
     "numinlets": 1,
     "numoutlets": 2,
     "patching_rect": [
      120,
      350,
      50,
      22.0
     ],
     "outlettype": [
      "",
      "bang"
     ],
     "parameter_enable": 0
    }
   },
   {
    "box": {
     "id": "c-beat",
     "maxclass": "comment",
     "numinlets": 1,
     "numoutlets": 0,
     "patching_rect": [
      172,
      350,
      40,
      22.0
     ],
     "text": "beat"
    }
   },
   {
    "box": {
     "id": "n-err",
     "maxclass": "flonum",
     "numinlets": 1,
     "numoutlets": 2,
     "patching_rect": [
      220,
      350,
      70,
      22.0
     ],
     "outlettype": [
      "",
      "bang"
     ],
     "parameter_enable": 0
    }
   },
   {
    "box": {
     "id": "c-err",
     "maxclass": "comment",
     "numinlets": 1,
     "numoutlets": 0,
     "patching_rect": [
      292,
      350,
      120,
      22.0
     ],
     "text": "err (samples)"
    }
   },
   {
    "box": {
     "id": "print",
     "maxclass": "newobj",
     "numinlets": 1,
     "numoutlets": 0,
     "patching_rect": [
      300,
      285,
      80,
      22.0
     ],
     "text": "print clock"
    }
   }
  ],
  "lines": [
   {
    "patchline": {
     "source": [
      "tog-t",
      0
     ],
     "destination": [
      "transport",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "tempo-num",
      0
     ],
     "destination": [
      "tempo-msg",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "tempo-msg",
      0
     ],
     "destination": [
      "transport",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "seek",
      0
     ],
     "destination": [
      "transport",
      1
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "phasor",
      0
     ],
     "destination": [
      "ck",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "m-check",
      0
     ],
     "destination": [
      "ck",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "m-clock",
      0
     ],
     "destination": [
      "ck",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "m-rm",
      0
     ],
     "destination": [
      "ck",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "ck",
      0
     ],
     "destination": [
      "gain",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "gain",
      0
     ],
     "destination": [
      "dac",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "gain",
      0
     ],
     "destination": [
      "dac",
      1
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "ck",
      1
     ],
     "destination": [
      "route",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "route",
      0
     ],
     "destination": [
      "route2",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "route",
      0
     ],
     "destination": [
      "print",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "route2",
      0
     ],
     "destination": [
      "n-beat",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "route2",
      1
     ],
     "destination": [
      "n-err",
      0
     ]
    }
   }
  ]
 }
}