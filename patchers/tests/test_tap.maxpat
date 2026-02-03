{
	"patcher" : 	{
		"fileversion" : 1,
		"appversion" : 		{
			"major" : 8,
			"minor" : 6,
			"revision" : 2,
			"architecture" : "x64",
			"modernui" : 1
		}
,
		"classnamespace" : "box",
		"rect" : [ 100.0, 100.0, 800.0, 650.0 ],
		"bglocked" : 0,
		"openinpresentation" : 0,
		"default_fontsize" : 10.0,
		"default_fontface" : 0,
		"default_fontname" : "Verdana",
		"gridonopen" : 1,
		"gridsize" : [ 5.0, 5.0 ],
		"gridsnaponopen" : 1,
		"objectsnaponopen" : 1,
		"statusbarvisible" : 2,
		"toolbarvisible" : 1,
		"lefttoolbarpinned" : 0,
		"toptoolbarpinned" : 0,
		"righttoolbarpinned" : 0,
		"bottomtoolbarpinned" : 0,
		"toolbars_unpinned_last_save" : 0,
		"tallnewobj" : 0,
		"boxanimatetime" : 200,
		"enablehscroll" : 1,
		"enablevscroll" : 1,
		"devicewidth" : 0.0,
		"description" : "Test the tap feature for reading global UGen samples",
		"digest" : "",
		"tags" : "",
		"style" : "",
		"subpatcher_template" : "",
		"assistshowspatchername" : 0,
		"boxes" : [ 			{
				"box" : 				{
					"fontface" : 1,
					"fontsize" : 14.0,
					"id" : "obj-title",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 20.0, 20.0, 500.0, 23.0 ],
					"text" : "Testing chuck~ tap feature (reading global UGen samples)"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-desc1",
					"linecount" : 2,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 20.0, 48.0, 550.0, 31.0 ],
					"text" : "The @tap attribute creates additional signal outlets for tapping into global UGens.\nUse the 'tap <name>' message to specify which global UGen to read from."
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-run",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 150.0, 120.0, 130.0, 21.0 ],
					"text" : "run test/tap_test.ck"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-tap-osc",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 300.0, 120.0, 65.0, 21.0 ],
					"text" : "tap osc"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-tap-clear",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 380.0, 120.0, 30.0, 21.0 ],
					"text" : "tap"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-status",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 430.0, 120.0, 39.0, 21.0 ],
					"text" : "status"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-reset",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 490.0, 120.0, 34.0, 21.0 ],
					"text" : "reset"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-button",
					"maxclass" : "button",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "bang" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 150.0, 160.0, 24.0, 24.0 ]
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-label-run",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 178.0, 164.0, 30.0, 19.0 ],
					"text" : "run"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Verdana",
					"fontsize" : 10.0,
					"id" : "obj-chuck",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "signal", "signal" ],
					"patching_rect" : [ 150.0, 210.0, 180.0, 21.0 ],
					"text" : "chuck~ 1 @tap 1 test/tap_test.ck"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-label-main",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 50.0, 280.0, 120.0, 19.0 ],
					"text" : "Main audio output:"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-scope-main",
					"maxclass" : "scope~",
					"numinlets" : 2,
					"numoutlets" : 0,
					"patching_rect" : [ 50.0, 300.0, 180.0, 130.0 ]
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-label-tap",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 280.0, 280.0, 180.0, 19.0 ],
					"text" : "Tapped UGen output (tap osc):"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-scope-tap",
					"maxclass" : "scope~",
					"numinlets" : 2,
					"numoutlets" : 0,
					"patching_rect" : [ 280.0, 300.0, 180.0, 130.0 ]
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-gain",
					"lastchannelcount" : 0,
					"maxclass" : "live.gain~",
					"numinlets" : 2,
					"numoutlets" : 5,
					"orientation" : 1,
					"outlettype" : [ "signal", "signal", "", "float", "list" ],
					"parameter_enable" : 1,
					"patching_rect" : [ 50.0, 470.0, 140.0, 47.0 ],
					"saved_attribute_attributes" : 					{
						"valueof" : 						{
							"parameter_longname" : "live.gain~",
							"parameter_mmax" : 6.0,
							"parameter_mmin" : -70.0,
							"parameter_modmode" : 3,
							"parameter_shortname" : "live.gain~",
							"parameter_type" : 0,
							"parameter_unitstyle" : 4
						}

					}
,
					"varname" : "live.gain~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-dac",
					"maxclass" : "ezdac~",
					"numinlets" : 2,
					"numoutlets" : 0,
					"patching_rect" : [ 50.0, 540.0, 45.0, 45.0 ]
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-meter-tap",
					"maxclass" : "meter~",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "float" ],
					"patching_rect" : [ 500.0, 300.0, 80.0, 130.0 ]
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-instructions",
					"linecount" : 5,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 500.0, 450.0, 250.0, 67.0 ],
					"text" : "Instructions:\n1. Turn on audio (click ezdac~)\n2. Click 'run' or message to load ChucK file\n3. Click 'tap osc' to tap the global oscillator\n4. Observe tapped signal in right scope/meter"
				}

			}
 ],
		"lines" : [ 			{
				"patchline" : 				{
					"destination" : [ "obj-chuck", 0 ],
					"source" : [ "obj-button", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-meter-tap", 0 ],
					"order" : 0,
					"source" : [ "obj-chuck", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-scope-tap", 0 ],
					"order" : 1,
					"source" : [ "obj-chuck", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-gain", 0 ],
					"order" : 0,
					"source" : [ "obj-chuck", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-scope-main", 0 ],
					"order" : 1,
					"source" : [ "obj-chuck", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-dac", 0 ],
					"source" : [ "obj-gain", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-chuck", 0 ],
					"source" : [ "obj-reset", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-chuck", 0 ],
					"source" : [ "obj-run", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-chuck", 0 ],
					"source" : [ "obj-status", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-chuck", 0 ],
					"source" : [ "obj-tap-clear", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-chuck", 0 ],
					"source" : [ "obj-tap-osc", 0 ]
				}

			}
 ],
		"parameters" : 		{
			"obj-gain" : [ "live.gain~", "live.gain~", 0 ],
			"parameterbanks" : 			{
				"0" : 				{
					"index" : 0,
					"name" : "",
					"parameters" : [ "-", "-", "-", "-", "-", "-", "-", "-" ]
				}

			}
,
			"inherited_shortname" : 1
		}
,
		"dependency_cache" : [ 			{
				"name" : "chuck~.mxo",
				"type" : "iLaX"
			}
 ],
		"autosave" : 0
	}

}
