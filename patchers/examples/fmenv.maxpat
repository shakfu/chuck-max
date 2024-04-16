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
		"rect" : [ 129.0, 92.0, 797.0, 774.0 ],
		"bglocked" : 0,
		"openinpresentation" : 0,
		"default_fontsize" : 10.0,
		"default_fontface" : 0,
		"default_fontname" : "Arial",
		"gridonopen" : 1,
		"gridsize" : [ 15.0, 15.0 ],
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
		"description" : "",
		"digest" : "",
		"tags" : "",
		"style" : "",
		"subpatcher_template" : "bigpatcher",
		"assistshowspatchername" : 0,
		"boxes" : [ 			{
				"box" : 				{
					"fontface" : 1,
					"id" : "obj-36",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 355.0, 147.0, 111.333336651325226, 18.0 ],
					"text" : "FM synthesis values",
					"textjustification" : 1
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-34",
					"linecount" : 6,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 495.0, 250.0, 268.333337366580963, 74.0 ],
					"text" : "FM synthesis: modulator values are multipliers:\n   carrier freq * harmonicity = mod freq\n   mod freq * modIndex = mod gain\nHarmonicity at integer values = harmonic series partials\nHarmonicity integer ratios will be more consonant\nHarmonicity at decimal values will be more dissonant"
				}

			}
, 			{
				"box" : 				{
					"fontface" : 2,
					"id" : "obj-32",
					"linecount" : 2,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 549.0, 117.0, 100.5, 29.0 ],
					"text" : "gap adds time (ms) between env loops"
				}

			}
, 			{
				"box" : 				{
					"fontface" : 2,
					"id" : "obj-25",
					"linecount" : 2,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 646.0, 117.0, 134.0, 29.0 ],
					"text" : "reverb mix only changes on new shred"
				}

			}
, 			{
				"box" : 				{
					"fontface" : 1,
					"fontsize" : 20.0,
					"id" : "obj-216",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 313.0, 17.0, 166.666669428348541, 29.0 ],
					"text" : "ChucK => Max",
					"textjustification" : 1
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-31",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 83.0, 81.0, 52.666668236255646, 18.0 ],
					"text" : "presets"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-30",
					"maxclass" : "preset",
					"numinlets" : 1,
					"numoutlets" : 5,
					"outlettype" : [ "preset", "int", "preset", "int", "" ],
					"patching_rect" : [ 79.0, 101.0, 100.0, 40.0 ],
					"preset_data" : [ 						{
							"number" : 1,
							"data" : [ 5, "obj-63", "live.gain~", "float", -2.5, 5, "obj-52", "flonum", "float", 110.0, 5, "obj-50", "flonum", "float", 1.519999980926514, 5, "obj-2", "flonum", "float", 0.200000002980232, 5, "obj-8", "number", "int", 1000, 5, "obj-10", "flonum", "float", 3.200000047683716, 5, "obj-14", "attrui", "attr", "debug", 5, "obj-14", "attrui", "int", 1, 5, "obj-3", "number", "int", 0, 5, "obj-27", "flonum", "float", 0.200000002980232, 5, "<invalid>", "flonum", "float", -2.5 ]
						}
, 						{
							"number" : 2,
							"data" : [ 5, "obj-63", "live.gain~", "float", -2.506885051727295, 5, "obj-52", "flonum", "float", 110.0, 5, "obj-50", "flonum", "float", 2.0, 5, "obj-2", "flonum", "float", 0.200000002980232, 5, "obj-8", "number", "int", 1000, 5, "obj-10", "flonum", "float", 6.0, 5, "obj-14", "attrui", "attr", "debug", 5, "obj-14", "attrui", "int", 1, 5, "obj-3", "number", "int", 0, 5, "obj-27", "flonum", "float", 0.200000002980232 ]
						}
, 						{
							"number" : 3,
							"data" : [ 5, "obj-63", "live.gain~", "float", -2.5, 5, "obj-52", "flonum", "float", 110.0, 5, "obj-50", "flonum", "float", 2.049999952316284, 5, "obj-2", "flonum", "float", 0.200000002980232, 5, "obj-8", "number", "int", 1000, 5, "obj-10", "flonum", "float", 8.0, 5, "obj-14", "attrui", "attr", "debug", 5, "obj-14", "attrui", "int", 1, 5, "obj-3", "number", "int", 0, 5, "obj-27", "flonum", "float", 0.200000002980232, 5, "<invalid>", "flonum", "float", -2.5 ]
						}
, 						{
							"number" : 4,
							"data" : [ 5, "obj-63", "live.gain~", "float", -2.5, 5, "obj-52", "flonum", "float", 110.0, 5, "obj-50", "flonum", "float", 2.200000047683716, 5, "obj-2", "flonum", "float", 0.200000002980232, 5, "obj-8", "number", "int", 1000, 5, "obj-10", "flonum", "float", 12.0, 5, "obj-14", "attrui", "attr", "debug", 5, "obj-14", "attrui", "int", 1, 5, "obj-3", "number", "int", 0, 5, "obj-27", "flonum", "float", 0.200000002980232, 5, "<invalid>", "flonum", "float", -2.5 ]
						}
, 						{
							"number" : 5,
							"data" : [ 5, "obj-63", "live.gain~", "float", -2.5, 5, "obj-52", "flonum", "float", 110.0, 5, "obj-50", "flonum", "float", 0.509999990463257, 5, "obj-2", "flonum", "float", 0.200000002980232, 5, "obj-8", "number", "int", 500, 5, "obj-10", "flonum", "float", 22.0, 5, "obj-14", "attrui", "attr", "debug", 5, "obj-14", "attrui", "int", 1, 5, "obj-3", "number", "int", 250, 5, "obj-27", "flonum", "float", 0.200000002980232, 5, "<invalid>", "flonum", "float", -2.5 ]
						}
, 						{
							"number" : 9,
							"data" : [ 5, "obj-63", "live.gain~", "float", -2.5, 5, "obj-52", "flonum", "float", 110.0, 5, "obj-50", "flonum", "float", 3.0, 5, "obj-2", "flonum", "float", 0.200000002980232, 5, "obj-8", "number", "int", 250, 5, "obj-10", "flonum", "float", 4.0, 5, "obj-14", "attrui", "attr", "debug", 5, "obj-14", "attrui", "int", 1, 5, "obj-3", "number", "int", 500, 5, "obj-27", "flonum", "float", 0.300000011920929, 5, "<invalid>", "flonum", "float", -2.5 ]
						}
, 						{
							"number" : 10,
							"data" : [ 5, "obj-63", "live.gain~", "float", -2.5, 5, "obj-52", "flonum", "float", 110.0, 5, "obj-50", "flonum", "float", 3.0, 5, "obj-2", "flonum", "float", 0.200000002980232, 5, "obj-8", "number", "int", 250, 5, "obj-10", "flonum", "float", 14.0, 5, "obj-14", "attrui", "attr", "debug", 5, "obj-14", "attrui", "int", 1, 5, "obj-3", "number", "int", 500, 5, "obj-27", "flonum", "float", 0.300000011920929, 5, "<invalid>", "flonum", "float", -2.5 ]
						}
, 						{
							"number" : 11,
							"data" : [ 5, "obj-63", "live.gain~", "float", -2.5, 5, "obj-52", "flonum", "float", 110.0, 5, "obj-50", "flonum", "float", 3.160000085830688, 5, "obj-2", "flonum", "float", 0.200000002980232, 5, "obj-8", "number", "int", 250, 5, "obj-10", "flonum", "float", 6.5, 5, "obj-14", "attrui", "attr", "debug", 5, "obj-14", "attrui", "int", 1, 5, "obj-3", "number", "int", 250, 5, "obj-27", "flonum", "float", 0.300000011920929, 5, "<invalid>", "flonum", "float", -2.5 ]
						}
, 						{
							"number" : 12,
							"data" : [ 5, "obj-63", "live.gain~", "float", -2.5, 5, "obj-52", "flonum", "float", 110.0, 5, "obj-50", "flonum", "float", 3.309999942779541, 5, "obj-2", "flonum", "float", 0.200000002980232, 5, "obj-8", "number", "int", 2000, 5, "obj-10", "flonum", "float", 6.5, 5, "obj-14", "attrui", "attr", "debug", 5, "obj-14", "attrui", "int", 1, 5, "obj-3", "number", "int", 0, 5, "obj-27", "flonum", "float", 0.400000005960464, 5, "<invalid>", "flonum", "float", -2.5 ]
						}
, 						{
							"number" : 13,
							"data" : [ 5, "obj-63", "live.gain~", "float", -2.5, 5, "obj-52", "flonum", "float", 220.0, 5, "obj-50", "flonum", "float", 3.329999923706055, 5, "obj-2", "flonum", "float", 0.200000002980232, 5, "obj-8", "number", "int", 100, 5, "obj-10", "flonum", "float", 4.0, 5, "obj-14", "attrui", "attr", "debug", 5, "obj-14", "attrui", "int", 1, 5, "obj-3", "number", "int", 0, 5, "obj-27", "flonum", "float", 0.200000002980232, 5, "<invalid>", "flonum", "float", -2.5 ]
						}
, 						{
							"number" : 14,
							"data" : [ 5, "obj-63", "live.gain~", "float", -2.5, 5, "obj-52", "flonum", "float", 400.0, 5, "obj-50", "flonum", "float", 2.339999914169312, 5, "obj-2", "flonum", "float", 0.200000002980232, 5, "obj-8", "number", "int", 100, 5, "obj-10", "flonum", "float", 5.880000114440918, 5, "obj-14", "attrui", "attr", "debug", 5, "obj-14", "attrui", "int", 1, 5, "obj-3", "number", "int", 0, 5, "obj-27", "flonum", "float", 0.200000002980232, 5, "<invalid>", "flonum", "float", -2.5 ]
						}
, 						{
							"number" : 15,
							"data" : [ 5, "obj-63", "live.gain~", "float", -2.5, 5, "obj-52", "flonum", "float", 400.0, 5, "obj-50", "flonum", "float", 2.410000085830688, 5, "obj-2", "flonum", "float", 0.28999999165535, 5, "obj-8", "number", "int", 10, 5, "obj-10", "flonum", "float", 5.960000038146973, 5, "obj-14", "attrui", "attr", "debug", 5, "obj-14", "attrui", "int", 1, 5, "obj-3", "number", "int", 10, 5, "obj-27", "flonum", "float", 0.200000002980232, 5, "<invalid>", "flonum", "float", -2.5 ]
						}
, 						{
							"number" : 16,
							"data" : [ 5, "obj-63", "live.gain~", "float", -2.5, 5, "obj-52", "flonum", "float", 200.0, 5, "obj-50", "flonum", "float", 2.299999952316284, 5, "obj-2", "flonum", "float", 0.28999999165535, 5, "obj-8", "number", "int", 500, 5, "obj-10", "flonum", "float", 6.900000095367432, 5, "obj-14", "attrui", "attr", "debug", 5, "obj-14", "attrui", "int", 1, 5, "obj-3", "number", "int", 20, 5, "obj-27", "flonum", "float", 0.200000002980232, 5, "<invalid>", "flonum", "float", -2.5 ]
						}
 ]
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-29",
					"linecount" : 2,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 646.0, 152.0, 41.333334565162659, 29.0 ],
					"text" : "JCRev\nmix",
					"textjustification" : 1
				}

			}
, 			{
				"box" : 				{
					"fontsize" : 10.0,
					"id" : "obj-28",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 648.0, 208.0, 52.0, 20.0 ],
					"text" : "revMix $1"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 0.211764705882353, 0.196078431372549, 0.776470588235294, 1.0 ],
					"format" : 6,
					"id" : "obj-27",
					"maxclass" : "flonum",
					"maximum" : 1.0,
					"minimum" : 0.0,
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "bang" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 648.0, 183.0, 36.666667759418488, 20.0 ]
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-23",
					"linecount" : 2,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 145.0, 157.0, 44.666667997837067, 29.0 ],
					"text" : "run\nadd ",
					"textjustification" : 1
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-22",
					"linecount" : 2,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 231.0, 115.0, 41.333334565162659, 29.0 ],
					"text" : "default values",
					"textjustification" : 1
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-20",
					"linecount" : 2,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 566.0, 152.0, 67.0, 29.0 ],
					"text" : "gap duration ms",
					"textjustification" : 1
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-16",
					"linecount" : 2,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 487.0, 152.0, 65.333333075046539, 29.0 ],
					"text" : "envelope duration ms",
					"textjustification" : 1
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-15",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 411.0, 163.0, 65.333333075046539, 18.0 ],
					"text" : "mod Index",
					"textjustification" : 1
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-13",
					"maxclass" : "button",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "bang" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 212.0, 118.0, 21.833334416151047, 21.833334416151047 ]
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-3",
					"maxclass" : "number",
					"maximum" : 20000,
					"minimum" : 0,
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "bang" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 566.0, 183.0, 50.0, 20.0 ]
				}

			}
, 			{
				"box" : 				{
					"fontsize" : 10.0,
					"id" : "obj-7",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 566.0, 208.0, 74.0, 20.0 ],
					"text" : "gapTimeMs $1"
				}

			}
, 			{
				"box" : 				{
					"attr" : "debug",
					"id" : "obj-14",
					"maxclass" : "attrui",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 199.0, 277.0, 124.000003695487976, 20.0 ]
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-12",
					"maxclass" : "button",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "bang" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 155.0, 183.0, 24.0, 24.0 ]
				}

			}
, 			{
				"box" : 				{
					"fontsize" : 10.0,
					"id" : "obj-9",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 421.0, 208.0, 66.0, 20.0 ],
					"text" : "modIndex $1"
				}

			}
, 			{
				"box" : 				{
					"format" : 6,
					"id" : "obj-10",
					"maxclass" : "flonum",
					"maximum" : 50.0,
					"minimum" : 0.1,
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "bang" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 421.0, 183.0, 44.500000357627869, 20.0 ]
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-8",
					"maxclass" : "number",
					"maximum" : 20000,
					"minimum" : 1,
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "bang" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 495.0, 183.0, 50.0, 20.0 ]
				}

			}
, 			{
				"box" : 				{
					"fontsize" : 10.0,
					"id" : "obj-4",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 495.0, 208.0, 68.0, 20.0 ],
					"text" : "envDurMs $1"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-152",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 331.0, 163.0, 65.333333075046539, 18.0 ],
					"text" : "harmonicity",
					"textjustification" : 1
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-151",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 279.0, 163.0, 33.33333432674408, 18.0 ],
					"text" : "gain",
					"textjustification" : 1
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-150",
					"linecount" : 2,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 207.0, 152.0, 56.666668355464935, 29.0 ],
					"text" : "carrier frequency",
					"textjustification" : 1
				}

			}
, 			{
				"box" : 				{
					"fontsize" : 10.0,
					"id" : "obj-6",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 277.0, 208.0, 57.0, 20.0 ],
					"text" : "carGain $1"
				}

			}
, 			{
				"box" : 				{
					"format" : 6,
					"id" : "obj-2",
					"maxclass" : "flonum",
					"maximum" : 1.0,
					"minimum" : 0.0,
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "bang" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 277.0, 183.0, 36.666667759418488, 20.0 ]
				}

			}
, 			{
				"box" : 				{
					"fontsize" : 10.0,
					"id" : "obj-49",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 341.0, 208.0, 74.0, 20.0 ],
					"text" : "harmonicity $1"
				}

			}
, 			{
				"box" : 				{
					"format" : 6,
					"id" : "obj-50",
					"maxclass" : "flonum",
					"maximum" : 50.0,
					"minimum" : 0.1,
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "bang" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 341.0, 183.0, 44.500000357627869, 20.0 ]
				}

			}
, 			{
				"box" : 				{
					"format" : 6,
					"id" : "obj-52",
					"maxclass" : "flonum",
					"maximum" : 4000.0,
					"minimum" : 0.0,
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "bang" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 211.0, 183.0, 47.0, 20.0 ]
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-37",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 70.0, 144.0, 31.0, 20.0 ],
					"text" : "reset"
				}

			}
, 			{
				"box" : 				{
					"hidden" : 1,
					"id" : "obj-26",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 7,
					"outlettype" : [ "float", "float", "float", "float", "int", "int", "float" ],
					"patching_rect" : [ 325.0, 117.0, 176.0, 20.0 ],
					"text" : "unpack f f f f i i f"
				}

			}
, 			{
				"box" : 				{
					"hidden" : 1,
					"id" : "obj-24",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 325.0, 93.0, 176.0, 20.0 ],
					"text" : "loadmess 110 0.2 1.52 3.2 1000 0 0.2"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-21",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 211.0, 207.0, 57.0, 20.0 ],
					"text" : "carFreq $1"
				}

			}
, 			{
				"box" : 				{
					"fontface" : 1,
					"fontsize" : 14.0,
					"id" : "obj-35",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 261.0, 50.0, 293.3333420753479, 22.0 ],
					"text" : "FM Synthesis with Modulator Envelope",
					"textjustification" : 1
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-19",
					"maxclass" : "live.scope~",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "bang" ],
					"patching_rect" : [ 328.0, 337.0, 230.75000587105751, 104.666669189929962 ]
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-18",
					"maxclass" : "newobj",
					"numinlets" : 0,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 171.0, 277.0, 25.0, 20.0 ],
					"text" : "r ck"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-17",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 370.0, 239.0, 26.0, 20.0 ],
					"text" : "s ck"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-5",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 104.0, 144.0, 29.5, 20.0 ],
					"text" : "info"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-167",
					"linecount" : 2,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 21.0, 185.0, 52.666668236255646, 29.0 ],
					"text" : "remove shred #",
					"textjustification" : 1
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-85",
					"maxclass" : "number",
					"maximum" : 20,
					"minimum" : 1,
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "bang" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 81.0, 185.0, 37.333334445953369, 20.0 ]
				}

			}
, 			{
				"box" : 				{
					"fontsize" : 10.0,
					"id" : "obj-83",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 81.0, 208.0, 56.0, 20.0 ],
					"text" : "remove $1"
				}

			}
, 			{
				"box" : 				{
					"fontsize" : 10.0,
					"id" : "obj-42",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 81.0, 262.0, 55.0, 20.0 ],
					"text" : "remove all"
				}

			}
, 			{
				"box" : 				{
					"fontsize" : 10.0,
					"id" : "obj-44",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 79.0, 235.0, 60.0, 20.0 ],
					"text" : "remove last"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-61",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 2,
					"outlettype" : [ "signal", "signal" ],
					"patching_rect" : [ 178.0, 395.0, 115.0, 20.0 ],
					"text" : "limi~ 2 @threshold -0.5."
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-62",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 221.0, 422.0, 64.666668593883514, 20.0 ],
					"text" : "startwindow"
				}

			}
, 			{
				"box" : 				{
					"fontsize" : 10.555534164108179,
					"id" : "obj-63",
					"lastchannelcount" : 0,
					"maxclass" : "live.gain~",
					"numinlets" : 2,
					"numoutlets" : 5,
					"orientation" : 1,
					"outlettype" : [ "signal", "signal", "", "float", "list" ],
					"parameter_enable" : 1,
					"patching_rect" : [ 178.0, 337.0, 136.0, 49.0 ],
					"saved_attribute_attributes" : 					{
						"valueof" : 						{
							"parameter_longname" : "live.gain~[1]",
							"parameter_mmax" : 6.0,
							"parameter_mmin" : -70.0,
							"parameter_modmode" : 3,
							"parameter_shortname" : "live.gain~",
							"parameter_type" : 0,
							"parameter_unitstyle" : 4
						}

					}
,
					"varname" : "live.gain~[1]"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-66",
					"maxclass" : "ezdac~",
					"numinlets" : 2,
					"numoutlets" : 0,
					"patching_rect" : [ 178.0, 425.0, 29.000020205974579, 29.000020205974579 ]
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-1",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 2,
					"outlettype" : [ "signal", "signal" ],
					"patching_rect" : [ 170.0, 301.0, 113.0, 20.0 ],
					"saved_object_attributes" : 					{
						"debug" : 1
					}
,
					"text" : "chuck~ 2 max/fmenv.ck"
				}

			}
 ],
		"lines" : [ 			{
				"patchline" : 				{
					"destination" : [ "obj-19", 0 ],
					"midpoints" : [ 179.5, 327.333329617977142, 337.5, 327.333329617977142 ],
					"order" : 0,
					"source" : [ "obj-1", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-63", 1 ],
					"source" : [ "obj-1", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-63", 0 ],
					"midpoints" : [ 179.5, 327.333329617977142, 187.5, 327.333329617977142 ],
					"order" : 1,
					"source" : [ "obj-1", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-9", 0 ],
					"source" : [ "obj-10", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"midpoints" : [ 164.5, 259.666660904884338, 179.5, 259.666660904884338 ],
					"source" : [ "obj-12", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-24", 0 ],
					"hidden" : 1,
					"midpoints" : [ 221.5, 150.999990999698639, 203.333332806825638, 150.999990999698639, 203.333332806825638, 88.333322465419769, 334.5, 88.333322465419769 ],
					"source" : [ "obj-13", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"midpoints" : [ 208.5, 298.333328723907471, 179.5, 298.333328723907471 ],
					"source" : [ "obj-14", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"midpoints" : [ 180.5, 299.999995470046997, 179.5, 299.999995470046997 ],
					"source" : [ "obj-18", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-6", 0 ],
					"source" : [ "obj-2", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-17", 0 ],
					"midpoints" : [ 220.5, 232.999993443489075, 379.5, 232.999993443489075 ],
					"source" : [ "obj-21", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-26", 0 ],
					"hidden" : 1,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-10", 0 ],
					"hidden" : 1,
					"midpoints" : [ 413.0, 142.333324074745178, 430.5, 142.333324074745178 ],
					"source" : [ "obj-26", 3 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-2", 0 ],
					"hidden" : 1,
					"midpoints" : [ 360.666666666666686, 146.999990880489349, 286.5, 146.999990880489349 ],
					"source" : [ "obj-26", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-27", 0 ],
					"hidden" : 1,
					"midpoints" : [ 491.5, 143.666657447814941, 657.5, 143.666657447814941 ],
					"source" : [ "obj-26", 6 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-3", 0 ],
					"hidden" : 1,
					"midpoints" : [ 465.333333333333371, 143.666657447814941, 575.5, 143.666657447814941 ],
					"source" : [ "obj-26", 5 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-50", 0 ],
					"hidden" : 1,
					"midpoints" : [ 386.833333333333314, 146.333324193954468, 350.5, 146.333324193954468 ],
					"source" : [ "obj-26", 2 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-52", 0 ],
					"hidden" : 1,
					"midpoints" : [ 334.5, 145.666657507419586, 220.5, 145.666657507419586 ],
					"source" : [ "obj-26", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-8", 0 ],
					"hidden" : 1,
					"midpoints" : [ 439.166666666666686, 142.99999076128006, 504.5, 142.99999076128006 ],
					"source" : [ "obj-26", 4 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-28", 0 ],
					"source" : [ "obj-27", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-17", 0 ],
					"midpoints" : [ 657.5, 234.999993503093719, 379.5, 234.999993503093719 ],
					"source" : [ "obj-28", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-7", 0 ],
					"source" : [ "obj-3", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-85", 0 ],
					"hidden" : 1,
					"midpoints" : [ 129.0, 178.333325147628784, 90.5, 178.333325147628784 ],
					"source" : [ "obj-30", 2 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"midpoints" : [ 79.5, 297.666662037372589, 179.5, 297.666662037372589 ],
					"source" : [ "obj-37", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-17", 0 ],
					"midpoints" : [ 504.5, 234.999993503093719, 379.5, 234.999993503093719 ],
					"source" : [ "obj-4", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"midpoints" : [ 90.5, 296.666662037372589, 179.5, 296.666662037372589 ],
					"source" : [ "obj-42", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"midpoints" : [ 88.5, 259.33332759141922, 179.5, 259.33332759141922 ],
					"source" : [ "obj-44", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-17", 0 ],
					"midpoints" : [ 350.5, 232.333326756954193, 379.5, 232.333326756954193 ],
					"source" : [ "obj-49", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"midpoints" : [ 113.5, 176.333325088024139, 79.333329111337662, 176.333325088024139, 79.333329111337662, 296.999995350837708, 179.5, 296.999995350837708 ],
					"source" : [ "obj-5", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-49", 0 ],
					"source" : [ "obj-50", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-21", 0 ],
					"source" : [ "obj-52", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-17", 0 ],
					"midpoints" : [ 286.5, 232.333326756954193, 379.5, 232.333326756954193 ],
					"source" : [ "obj-6", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-66", 1 ],
					"midpoints" : [ 283.5, 420.199968874454498, 197.500020205974579, 420.199968874454498 ],
					"source" : [ "obj-61", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-66", 0 ],
					"midpoints" : [ 187.5, 420.199968874454498, 187.5, 420.199968874454498 ],
					"source" : [ "obj-61", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-66", 0 ],
					"midpoints" : [ 230.5, 449.199968755245209, 214.733303874731064, 449.199968755245209, 214.733303874731064, 423.199968874454498, 187.5, 423.199968874454498 ],
					"source" : [ "obj-62", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-61", 1 ],
					"midpoints" : [ 216.75, 387.199968874454498, 283.5, 387.199968874454498 ],
					"source" : [ "obj-63", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-61", 0 ],
					"midpoints" : [ 187.5, 387.199968874454498, 187.5, 387.199968874454498 ],
					"source" : [ "obj-63", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-17", 0 ],
					"midpoints" : [ 575.5, 234.333326816558838, 379.5, 234.333326816558838 ],
					"source" : [ "obj-7", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4", 0 ],
					"source" : [ "obj-8", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"midpoints" : [ 90.5, 230.666660070419312, 179.5, 230.666660070419312 ],
					"source" : [ "obj-83", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-83", 0 ],
					"source" : [ "obj-85", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-17", 0 ],
					"midpoints" : [ 430.5, 233.666660130023956, 379.5, 233.666660130023956 ],
					"source" : [ "obj-9", 0 ]
				}

			}
 ],
		"parameters" : 		{
			"obj-63" : [ "live.gain~[1]", "live.gain~", 0 ],
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
		"autosave" : 0,
		"bgcolor" : [ 1.0, 1.0, 1.0, 1.0 ]
	}

}
