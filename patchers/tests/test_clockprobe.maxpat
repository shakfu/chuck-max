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
			700.0,
			540.0
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
						640,
						80.0
					],
					"text": "clockprobe: compares Max transport (itm) read on the audio thread with [phasor~ 4n @lock 1]. 1) DSP on  2) transport on  3) click clockprobe. Report: once per second of audio. Error in samples; positive = transport reads ahead of phasor~. Repeat with SIAI on/off and vector size 64/512. jmin/jmax: per-block transport advance minus frames rendered, in samples; this exposes transport jitter, which phasor~ @lock 1 shares and so hides."
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
						90,
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
						90,
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
					"id": "transport",
					"maxclass": "newobj",
					"numinlets": 2,
					"numoutlets": 9,
					"patching_rect": [
						100,
						130,
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
					"id": "c-tr",
					"maxclass": "comment",
					"numinlets": 1,
					"numoutlets": 0,
					"patching_rect": [
						130,
						90,
						120,
						22.0
					],
					"text": "transport on/off"
				}
			},
			{
				"box": {
					"id": "tempo-msg",
					"maxclass": "message",
					"numinlets": 2,
					"numoutlets": 1,
					"patching_rect": [
						200,
						90,
						70,
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
					"id": "tempo-num",
					"maxclass": "flonum",
					"numinlets": 1,
					"numoutlets": 2,
					"patching_rect": [
						200,
						60,
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
					"id": "m-on",
					"maxclass": "message",
					"numinlets": 2,
					"numoutlets": 1,
					"patching_rect": [
						170,
						200,
						75,
						22.0
					],
					"outlettype": [
						""
					],
					"text": "clockprobe"
				}
			},
			{
				"box": {
					"id": "m-1n",
					"maxclass": "message",
					"numinlets": 2,
					"numoutlets": 1,
					"patching_rect": [
						255,
						200,
						100,
						22.0
					],
					"outlettype": [
						""
					],
					"text": "clockprobe 1920"
				}
			},
			{
				"box": {
					"id": "m-off",
					"maxclass": "message",
					"numinlets": 2,
					"numoutlets": 1,
					"patching_rect": [
						365,
						200,
						85,
						22.0
					],
					"outlettype": [
						""
					],
					"text": "clockprobe 0"
				}
			},
			{
				"box": {
					"id": "chuck",
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
					"id": "route",
					"maxclass": "newobj",
					"numinlets": 2,
					"numoutlets": 2,
					"patching_rect": [
						20,
						290,
						100,
						22.0
					],
					"outlettype": [
						"",
						""
					],
					"text": "route clockprobe"
				}
			},
			{
				"box": {
					"id": "unpack",
					"maxclass": "newobj",
					"numinlets": 1,
					"numoutlets": 9,
					"patching_rect": [
						20,
						320,
						280.0,
						22.0
					],
					"outlettype": [
						"int",
						"float",
						"float",
						"float",
						"float",
						"int",
						"float",
						"float",
						"float"
					],
					"text": "unpack 0 0. 0. 0. 0. 0 0. 0. 0."
				}
			},
			{
				"box": {
					"id": "print",
					"maxclass": "newobj",
					"numinlets": 1,
					"numoutlets": 0,
					"patching_rect": [
						260,
						320,
						100,
						22.0
					],
					"text": "print clockprobe"
				}
			},
			{
				"box": {
					"id": "n-blocks",
					"maxclass": "number",
					"numinlets": 1,
					"numoutlets": 2,
					"patching_rect": [
						20,
						360,
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
					"id": "c-blocks",
					"maxclass": "comment",
					"numinlets": 1,
					"numoutlets": 0,
					"patching_rect": [
						20,
						385,
						70,
						22.0
					],
					"text": "blocks"
				}
			},
			{
				"box": {
					"id": "n-mean",
					"maxclass": "flonum",
					"numinlets": 1,
					"numoutlets": 2,
					"patching_rect": [
						100,
						360,
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
					"id": "c-mean",
					"maxclass": "comment",
					"numinlets": 1,
					"numoutlets": 0,
					"patching_rect": [
						100,
						385,
						70,
						22.0
					],
					"text": "mean"
				}
			},
			{
				"box": {
					"id": "n-min",
					"maxclass": "flonum",
					"numinlets": 1,
					"numoutlets": 2,
					"patching_rect": [
						180,
						360,
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
					"id": "c-min",
					"maxclass": "comment",
					"numinlets": 1,
					"numoutlets": 0,
					"patching_rect": [
						180,
						385,
						70,
						22.0
					],
					"text": "min"
				}
			},
			{
				"box": {
					"id": "n-max",
					"maxclass": "flonum",
					"numinlets": 1,
					"numoutlets": 2,
					"patching_rect": [
						260,
						360,
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
					"id": "c-max",
					"maxclass": "comment",
					"numinlets": 1,
					"numoutlets": 0,
					"patching_rect": [
						260,
						385,
						70,
						22.0
					],
					"text": "max"
				}
			},
			{
				"box": {
					"id": "n-tempo",
					"maxclass": "flonum",
					"numinlets": 1,
					"numoutlets": 2,
					"patching_rect": [
						340,
						360,
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
					"id": "c-tempo",
					"maxclass": "comment",
					"numinlets": 1,
					"numoutlets": 0,
					"patching_rect": [
						340,
						385,
						70,
						22.0
					],
					"text": "tempo"
				}
			},
			{
				"box": {
					"id": "n-running",
					"maxclass": "number",
					"numinlets": 1,
					"numoutlets": 2,
					"patching_rect": [
						420,
						360,
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
					"id": "c-running",
					"maxclass": "comment",
					"numinlets": 1,
					"numoutlets": 0,
					"patching_rect": [
						420,
						385,
						70,
						22.0
					],
					"text": "running"
				}
			},
			{
				"box": {
					"id": "n-ticks",
					"maxclass": "flonum",
					"numinlets": 1,
					"numoutlets": 2,
					"patching_rect": [
						500,
						360,
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
					"id": "c-ticks",
					"maxclass": "comment",
					"numinlets": 1,
					"numoutlets": 0,
					"patching_rect": [
						500,
						385,
						70,
						22.0
					],
					"text": "ticks"
				}
			},
			{
				"box": {
					"id": "ad-siai",
					"maxclass": "newobj",
					"numinlets": 2,
					"numoutlets": 2,
					"patching_rect": [
						20,
						440,
						120,
						22.0
					],
					"outlettype": [
						"",
						"int"
					],
					"text": "adstatus takeover"
				}
			},
			{
				"box": {
					"id": "tog-siai",
					"maxclass": "umenu",
					"numinlets": 1,
					"numoutlets": 3,
					"patching_rect": [
						20.0,
						470.0,
						70.0,
						22.0
					],
					"outlettype": [
						"int",
						"",
						""
					],
					"parameter_enable": 0
				}
			},
			{
				"box": {
					"id": "c-siai",
					"maxclass": "comment",
					"numinlets": 1,
					"numoutlets": 0,
					"patching_rect": [
						95.0,
						470.0,
						160.0,
						22.0
					],
					"text": "Scheduler in Audio Interrupt"
				}
			},
			{
				"box": {
					"id": "ad-vs",
					"maxclass": "newobj",
					"numinlets": 2,
					"numoutlets": 2,
					"patching_rect": [
						260,
						440,
						110,
						22.0
					],
					"outlettype": [
						"",
						"int"
					],
					"text": "adstatus sigvs"
				}
			},
			{
				"box": {
					"id": "menu-vs",
					"maxclass": "umenu",
					"numinlets": 1,
					"numoutlets": 3,
					"patching_rect": [
						260,
						470,
						70,
						22.0
					],
					"outlettype": [
						"int",
						"",
						""
					],
					"parameter_enable": 0
				}
			},
			{
				"box": {
					"id": "c-vs",
					"maxclass": "comment",
					"numinlets": 1,
					"numoutlets": 0,
					"patching_rect": [
						335,
						470,
						150,
						22.0
					],
					"text": "signal vector size"
				}
			},
			{
				"box": {
					"id": "n-jmin",
					"maxclass": "flonum",
					"numinlets": 1,
					"numoutlets": 2,
					"outlettype": [
						"",
						"bang"
					],
					"parameter_enable": 0,
					"patching_rect": [
						20,
						412.0,
						70.0,
						22.0
					]
				}
			},
			{
				"box": {
					"id": "c-jmin",
					"maxclass": "comment",
					"numinlets": 1,
					"numoutlets": 0,
					"patching_rect": [
						90,
						412.0,
						40.0,
						22.0
					],
					"text": "jmin"
				}
			},
			{
				"box": {
					"id": "n-jmax",
					"maxclass": "flonum",
					"numinlets": 1,
					"numoutlets": 2,
					"outlettype": [
						"",
						"bang"
					],
					"parameter_enable": 0,
					"patching_rect": [
						100,
						412.0,
						70.0,
						22.0
					]
				}
			},
			{
				"box": {
					"id": "c-jmax",
					"maxclass": "comment",
					"numinlets": 1,
					"numoutlets": 0,
					"patching_rect": [
						170,
						412.0,
						40.0,
						22.0
					],
					"text": "jmax"
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
						"phasor",
						0
					],
					"destination": [
						"chuck",
						0
					]
				}
			},
			{
				"patchline": {
					"source": [
						"m-on",
						0
					],
					"destination": [
						"chuck",
						0
					]
				}
			},
			{
				"patchline": {
					"source": [
						"m-1n",
						0
					],
					"destination": [
						"chuck",
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
						"chuck",
						0
					]
				}
			},
			{
				"patchline": {
					"source": [
						"chuck",
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
						"unpack",
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
						"unpack",
						0
					],
					"destination": [
						"n-blocks",
						0
					]
				}
			},
			{
				"patchline": {
					"source": [
						"unpack",
						1
					],
					"destination": [
						"n-mean",
						0
					]
				}
			},
			{
				"patchline": {
					"source": [
						"unpack",
						2
					],
					"destination": [
						"n-min",
						0
					]
				}
			},
			{
				"patchline": {
					"source": [
						"unpack",
						3
					],
					"destination": [
						"n-max",
						0
					]
				}
			},
			{
				"patchline": {
					"source": [
						"unpack",
						4
					],
					"destination": [
						"n-tempo",
						0
					]
				}
			},
			{
				"patchline": {
					"source": [
						"unpack",
						5
					],
					"destination": [
						"n-running",
						0
					]
				}
			},
			{
				"patchline": {
					"source": [
						"unpack",
						6
					],
					"destination": [
						"n-ticks",
						0
					]
				}
			},
			{
				"patchline": {
					"source": [
						"ad-siai",
						0
					],
					"destination": [
						"tog-siai",
						0
					]
				}
			},
			{
				"patchline": {
					"source": [
						"tog-siai",
						0
					],
					"destination": [
						"ad-siai",
						0
					]
				}
			},
			{
				"patchline": {
					"source": [
						"ad-vs",
						0
					],
					"destination": [
						"menu-vs",
						0
					]
				}
			},
			{
				"patchline": {
					"source": [
						"menu-vs",
						0
					],
					"destination": [
						"ad-vs",
						0
					]
				}
			},
			{
				"patchline": {
					"source": [
						"unpack",
						7
					],
					"destination": [
						"n-jmin",
						0
					]
				}
			},
			{
				"patchline": {
					"source": [
						"unpack",
						8
					],
					"destination": [
						"n-jmax",
						0
					]
				}
			}
		]
	}
}