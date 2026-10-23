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
   340.0
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
      78
     ],
     "text": "MaxBuffer test. 1) DSP on. 2) add max_buffer.ck: 'written' shows one second of frames at the Max sample rate (48000 at 48 kHz); double-click buffer~: a decaying tone. 3) add max_buffer_play.ck: you hear it loop; the rate messages change speed and direction. 4) replace: load a sound file; playback follows it. 5) delete the buffer~ box: silence, no error. Undo (cmd-Z), then add max_buffer.ck again: playback resumes. 6) removeall.",
     "linecount": 5
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
      95,
      45,
      22.0
     ]
    }
   },
   {
    "box": {
     "id": "buf",
     "maxclass": "newobj",
     "numinlets": 1,
     "numoutlets": 2,
     "patching_rect": [
      480,
      135,
      110,
      22.0
     ],
     "outlettype": [
      "float",
      "bang"
     ],
     "text": "buffer~ ckbuf 1000"
    }
   },
   {
    "box": {
     "id": "m-replace",
     "maxclass": "message",
     "numinlets": 2,
     "numoutlets": 1,
     "patching_rect": [
      480,
      100,
      55,
      22.0
     ],
     "outlettype": [
      ""
     ],
     "text": "replace"
    }
   },
   {
    "box": {
     "id": "m-write",
     "maxclass": "message",
     "numinlets": 2,
     "numoutlets": 1,
     "patching_rect": [
      20,
      135,
      140,
      22.0
     ],
     "outlettype": [
      ""
     ],
     "text": "add max/max_buffer.ck"
    }
   },
   {
    "box": {
     "id": "m-play",
     "maxclass": "message",
     "numinlets": 2,
     "numoutlets": 1,
     "patching_rect": [
      170,
      135,
      165,
      22.0
     ],
     "outlettype": [
      ""
     ],
     "text": "add max/max_buffer_play.ck"
    }
   },
   {
    "box": {
     "id": "m-rm",
     "maxclass": "message",
     "numinlets": 2,
     "numoutlets": 1,
     "patching_rect": [
      345,
      135,
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
      180,
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
      215,
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
      245,
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
      215,
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
      245,
      90,
      22.0
     ],
     "outlettype": [
      "",
      ""
     ],
     "text": "route written"
    }
   },
   {
    "box": {
     "id": "n-w",
     "maxclass": "number",
     "numinlets": 1,
     "numoutlets": 2,
     "patching_rect": [
      120,
      280,
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
     "id": "c-w",
     "maxclass": "comment",
     "numinlets": 1,
     "numoutlets": 0,
     "patching_rect": [
      192,
      280,
      60,
      22.0
     ],
     "text": "written"
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
      215,
      90,
      22.0
     ],
     "text": "print maxbuffer"
    }
   },
   {
    "box": {
     "id": "m-rate0",
     "maxclass": "message",
     "numinlets": 2,
     "numoutlets": 1,
     "outlettype": [
      ""
     ],
     "patching_rect": [
      170,
      100,
      80,
      22.0
     ],
     "text": "msg rate 0.5"
    }
   },
   {
    "box": {
     "id": "m-rate1",
     "maxclass": "message",
     "numinlets": 2,
     "numoutlets": 1,
     "outlettype": [
      ""
     ],
     "patching_rect": [
      260,
      100,
      65,
      22.0
     ],
     "text": "msg rate 1"
    }
   },
   {
    "box": {
     "id": "m-rate2",
     "maxclass": "message",
     "numinlets": 2,
     "numoutlets": 1,
     "outlettype": [
      ""
     ],
     "patching_rect": [
      335,
      100,
      65,
      22.0
     ],
     "text": "msg rate -1"
    }
   }
  ],
  "lines": [
   {
    "patchline": {
     "source": [
      "m-write",
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
      "m-play",
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
      "m-replace",
      0
     ],
     "destination": [
      "buf",
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
      "n-w",
      0
     ]
    }
   },
   {
    "patchline": {
     "source": [
      "m-rate0",
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
      "m-rate1",
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
      "m-rate2",
      0
     ],
     "destination": [
      "ck",
      0
     ]
    }
   }
  ]
 }
}