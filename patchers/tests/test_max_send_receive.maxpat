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
			420.0
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
						660,
						64
					],
					"text": "Host.send* test (max_send_receive.ck). 1) DSP on. 2) add to A: TICK bangs 4x/s, freq updates 4x/s, RANGE prints 'symbol low' or 'symbol high', PAIR prints two floats. No cord runs from chuck~ to any [r]. 3) add to B as well: freq updates 8x/s, since [r freq] is shared by the whole Max process. 4) removeall on both: all four stop, no error."
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
					"id": "a-add",
					"maxclass": "message",
					"numinlets": 2,
					"numoutlets": 1,
					"patching_rect": [
						20,
						130,
						190,
						22.0
					],
					"outlettype": [
						""
					],
					"text": "add max/max_send_receive.ck"
				}
			},
			{
				"box": {
					"id": "a-rmall",
					"maxclass": "message",
					"numinlets": 2,
					"numoutlets": 1,
					"patching_rect": [
						220,
						130,
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
					"id": "a-ck",
					"maxclass": "newobj",
					"numinlets": 1,
					"numoutlets": 2,
					"patching_rect": [
						20,
						170,
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
					"id": "a-gain",
					"maxclass": "newobj",
					"numinlets": 2,
					"numoutlets": 1,
					"patching_rect": [
						20,
						200,
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
					"id": "a-dac",
					"maxclass": "newobj",
					"numinlets": 2,
					"numoutlets": 0,
					"patching_rect": [
						20,
						230,
						40,
						22.0
					],
					"text": "dac~"
				}
			},
			{
				"box": {
					"id": "c-a",
					"maxclass": "comment",
					"numinlets": 1,
					"numoutlets": 0,
					"patching_rect": [
						90,
						170,
						40,
						22.0
					],
					"text": "A"
				}
			},
			{
				"box": {
					"id": "b-add",
					"maxclass": "message",
					"numinlets": 2,
					"numoutlets": 1,
					"patching_rect": [
						20,
						290,
						190,
						22.0
					],
					"outlettype": [
						""
					],
					"text": "add max/max_send_receive.ck"
				}
			},
			{
				"box": {
					"id": "b-rmall",
					"maxclass": "message",
					"numinlets": 2,
					"numoutlets": 1,
					"patching_rect": [
						220,
						290,
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
					"id": "b-ck",
					"maxclass": "newobj",
					"numinlets": 1,
					"numoutlets": 2,
					"patching_rect": [
						20,
						330,
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
					"id": "c-b",
					"maxclass": "comment",
					"numinlets": 1,
					"numoutlets": 0,
					"patching_rect": [
						90,
						330,
						40,
						22.0
					],
					"text": "B"
				}
			},
			{
				"box": {
					"id": "r-tick",
					"maxclass": "newobj",
					"numinlets": 0,
					"numoutlets": 1,
					"patching_rect": [
						360,
						130,
						50,
						22.0
					],
					"outlettype": [
						""
					],
					"text": "r tick"
				}
			},
			{
				"box": {
					"id": "p-tick",
					"maxclass": "newobj",
					"numinlets": 1,
					"numoutlets": 0,
					"patching_rect": [
						360,
						170,
						70,
						22.0
					],
					"text": "print TICK"
				}
			},
			{
				"box": {
					"id": "r-freq",
					"maxclass": "newobj",
					"numinlets": 0,
					"numoutlets": 1,
					"patching_rect": [
						450,
						130,
						50,
						22.0
					],
					"outlettype": [
						""
					],
					"text": "r freq"
				}
			},
			{
				"box": {
					"id": "n-freq",
					"maxclass": "flonum",
					"numinlets": 1,
					"numoutlets": 2,
					"patching_rect": [
						450,
						170,
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
					"id": "c-freq",
					"maxclass": "comment",
					"numinlets": 1,
					"numoutlets": 0,
					"patching_rect": [
						525,
						170,
						50,
						22.0
					],
					"text": "freq"
				}
			},
			{
				"box": {
					"id": "r-range",
					"maxclass": "newobj",
					"numinlets": 0,
					"numoutlets": 1,
					"patching_rect": [
						360,
						220,
						60,
						22.0
					],
					"outlettype": [
						""
					],
					"text": "r range"
				}
			},
			{
				"box": {
					"id": "p-range",
					"maxclass": "newobj",
					"numinlets": 1,
					"numoutlets": 0,
					"patching_rect": [
						360,
						260,
						85,
						22.0
					],
					"text": "print RANGE"
				}
			},
			{
				"box": {
					"id": "r-pair",
					"maxclass": "newobj",
					"numinlets": 0,
					"numoutlets": 1,
					"patching_rect": [
						450,
						220,
						50,
						22.0
					],
					"outlettype": [
						""
					],
					"text": "r pair"
				}
			},
			{
				"box": {
					"id": "p-pair",
					"maxclass": "newobj",
					"numinlets": 1,
					"numoutlets": 0,
					"patching_rect": [
						450,
						260,
						75,
						22.0
					],
					"text": "print PAIR"
				}
			},
			{
				"box": {
					"id": "c-r",
					"maxclass": "comment",
					"numinlets": 1,
					"numoutlets": 0,
					"patching_rect": [
						360,
						300,
						320,
						50
					],
					"text": "[r tick], [r freq], [r range], [r pair] bind to names, as the ';' prefix in a message box does. Delivery is on the main thread: TICK is not a clock."
				}
			}
		],
		"lines": [
			{
				"patchline": {
					"source": [
						"a-add",
						0
					],
					"destination": [
						"a-ck",
						0
					]
				}
			},
			{
				"patchline": {
					"source": [
						"a-rmall",
						0
					],
					"destination": [
						"a-ck",
						0
					]
				}
			},
			{
				"patchline": {
					"source": [
						"a-ck",
						0
					],
					"destination": [
						"a-gain",
						0
					]
				}
			},
			{
				"patchline": {
					"source": [
						"a-gain",
						0
					],
					"destination": [
						"a-dac",
						0
					]
				}
			},
			{
				"patchline": {
					"source": [
						"a-gain",
						0
					],
					"destination": [
						"a-dac",
						1
					]
				}
			},
			{
				"patchline": {
					"source": [
						"b-add",
						0
					],
					"destination": [
						"b-ck",
						0
					]
				}
			},
			{
				"patchline": {
					"source": [
						"b-rmall",
						0
					],
					"destination": [
						"b-ck",
						0
					]
				}
			},
			{
				"patchline": {
					"source": [
						"r-tick",
						0
					],
					"destination": [
						"p-tick",
						0
					]
				}
			},
			{
				"patchline": {
					"source": [
						"r-freq",
						0
					],
					"destination": [
						"n-freq",
						0
					]
				}
			},
			{
				"patchline": {
					"source": [
						"r-range",
						0
					],
					"destination": [
						"p-range",
						0
					]
				}
			},
			{
				"patchline": {
					"source": [
						"r-pair",
						0
					],
					"destination": [
						"p-pair",
						0
					]
				}
			}
		]
	}
}
