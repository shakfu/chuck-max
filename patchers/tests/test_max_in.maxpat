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
   360.0
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
     "text": "HostIn test. 1) DSP on. 2) add max/max_in.ck. 3) click the note messages: each plays and 'got' shows the pitch; 'legato' changes pitch without a new attack; 'off' releases. 4) removeall, then a note: no sound, no error. 5) reset, add again: one 'got' per note, not two.",
     "linecount": 4
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
     "id": "m-add",
     "maxclass": "message",
     "numinlets": 2,
     "numoutlets": 1,
     "patching_rect": [
      20,
      120,
      120,
      22.0
     ],
     "outlettype": [
      ""
     ],
     "text": "add max/max_in.ck"
    }
   },
   {
    "box": {
     "id": "m-n1",
     "maxclass": "message",
     "numinlets": 2,
     "numoutlets": 1,
     "patching_rect": [
      150,
      120,
      110,
      22.0
     ],
     "outlettype": [
      ""
     ],
     "text": "msg note 60 0.8"
    }
   },
   {
    "box": {
     "id": "m-n2",
     "maxclass": "message",
     "numinlets": 2,
     "numoutlets": 1,
     "patching_rect": [
      270,
      120,
      150,
      22.0
     ],
     "outlettype": [
      ""
     ],
     "text": "msg note 64 0.5 legato"
    }
   },
   {
    "box": {
     "id": "m-n3",
     "maxclass": "message",
     "numinlets": 2,
     "numoutlets": 1,
     "patching_rect": [
      430,
      120,
      100,
      22.0
     ],
     "outlettype": [
      ""
     ],
     "text": "msg note 67 1."
    }
   },
   {
    "box": {
     "id": "m-off",
     "maxclass": "message",
     "numinlets": 2,
     "numoutlets": 1,
     "patching_rect": [
      540,
      120,
      55,
      22.0
     ],
     "outlettype": [
      ""
     ],
     "text": "msg off"
    }
   },
   {
    "box": {
     "id": "m-rm",
     "maxclass": "message",
     "numinlets": 2,
     "numoutlets": 1,
     "patching_rect": [
      150,
      155,
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
     "id": "m-reset",
     "maxclass": "message",
     "numinlets": 2,
     "numoutlets": 1,
     "patching_rect": [
      230,
      155,
      45,
      22.0
     ],
     "outlettype": [
      ""
     ],
     "text": "reset"
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
      200,
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
      235,
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
      265,
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
      235,
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
     "numinlets": 2,
     "numoutlets": 2,
     "patching_rect": [
      120,
      265,
      70,
      22.0
     ],
     "outlettype": [
      "",
      ""
     ],
     "text": "route got"
    }
   },
   {
    "box": {
     "id": "n-got",
     "maxclass": "number",
     "numinlets": 1,
     "numoutlets": 2,
     "patching_rect": [
      120,
      300,
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
     "id": "c-got",
     "maxclass": "comment",
     "numinlets": 1,
     "numoutlets": 0,
     "patching_rect": [
      172,
      300,
      60,
      22.0
     ],
     "text": "got"
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
      235,
      80,
      22.0
     ],
     "text": "print maxin"
    }
   }
  ],
  "lines": [
   {
    "patchline": {
     "source": [
      "m-add",
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
      "m-n1",
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
      "m-n2",
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
      "m-n3",
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
      "m-off",
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
      "m-reset",
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
      "n-got",
      0
     ]
    }
   }
  ]
 }
}