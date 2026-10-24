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
			480.0
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
						50
					],
					"text": "Host.send test. 1) DSP on. 2) add max_send.ck: freq updates 4x/s. 3) reset, then add max_send_once.ck: 'once 1' still arrives (Host survives reset). 4) send max_send_once.ck to B only: only 'B' prints."
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
						70,
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
						120,
						150,
						22.0
					],
					"outlettype": [
						""
					],
					"text": "add max/max_send.ck"
				}
			},
			{
				"box": {
					"id": "a-once",
					"maxclass": "message",
					"numinlets": 2,
					"numoutlets": 1,
					"patching_rect": [
						180,
						120,
						170,
						22.0
					],
					"outlettype": [
						""
					],
					"text": "add max/max_send_once.ck"
				}
			},
			{
				"box": {
					"id": "a-reset",
					"maxclass": "message",
					"numinlets": 2,
					"numoutlets": 1,
					"patching_rect": [
						360,
						120,
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
					"id": "a-rmall",
					"maxclass": "message",
					"numinlets": 2,
					"numoutlets": 1,
					"patching_rect": [
						415,
						120,
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
					"id": "a-route",
					"maxclass": "newobj",
					"numinlets": 2,
					"numoutlets": 2,
					"patching_rect": [
						120,
						200,
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
					"id": "a-print",
					"maxclass": "newobj",
					"numinlets": 1,
					"numoutlets": 0,
					"patching_rect": [
						120,
						260,
						60,
						22.0
					],
					"text": "print A"
				}
			},
			{
				"box": {
					"id": "a-route2",
					"maxclass": "newobj",
					"numinlets": 2,
					"numoutlets": 2,
					"patching_rect": [
						200,
						230,
						80,
						22.0
					],
					"outlettype": [
						"",
						""
					],
					"text": "route freq"
				}
			},
			{
				"box": {
					"id": "a-num",
					"maxclass": "flonum",
					"numinlets": 1,
					"numoutlets": 2,
					"patching_rect": [
						200,
						260,
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
					"id": "c-a",
					"maxclass": "comment",
					"numinlets": 1,
					"numoutlets": 0,
					"patching_rect": [
						280,
						260,
						80,
						22.0
					],
					"text": "freq (A)"
				}
			},
			{
				"box": {
					"id": "b-once",
					"maxclass": "message",
					"numinlets": 2,
					"numoutlets": 1,
					"patching_rect": [
						20,
						320,
						170,
						22.0
					],
					"outlettype": [
						""
					],
					"text": "add max/max_send_once.ck"
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
						360,
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
					"id": "b-route",
					"maxclass": "newobj",
					"numinlets": 2,
					"numoutlets": 2,
					"patching_rect": [
						120,
						390,
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
					"id": "b-print",
					"maxclass": "newobj",
					"numinlets": 1,
					"numoutlets": 0,
					"patching_rect": [
						120,
						420,
						60,
						22.0
					],
					"text": "print B"
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
						"a-once",
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
						"a-reset",
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
						"a-ck",
						1
					],
					"destination": [
						"a-route",
						0
					]
				}
			},
			{
				"patchline": {
					"source": [
						"a-route",
						0
					],
					"destination": [
						"a-print",
						0
					]
				}
			},
			{
				"patchline": {
					"source": [
						"a-route",
						0
					],
					"destination": [
						"a-route2",
						0
					]
				}
			},
			{
				"patchline": {
					"source": [
						"a-route2",
						0
					],
					"destination": [
						"a-num",
						0
					]
				}
			},
			{
				"patchline": {
					"source": [
						"b-once",
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
						"b-ck",
						1
					],
					"destination": [
						"b-route",
						0
					]
				}
			},
			{
				"patchline": {
					"source": [
						"b-route",
						0
					],
					"destination": [
						"b-print",
						0
					]
				}
			}
		]
	}
}