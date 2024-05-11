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
		"rect" : [ 176.0, 87.0, 907.0, 676.0 ],
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
					"id" : "obj-76",
					"linecount" : 2,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 295.000008791685104, 101.500002861022949, 251.33334082365036, 29.0 ],
					"text" : "This patch illustrates how to combine synthesis output from ChucK with DSP processing in Max/gen~. "
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-74",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 658.16667228937149, 500.000014901161194, 100.000002980232239, 18.0 ],
					"text" : "Output signal R"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-73",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 658.16667228937149, 428.000012755393982, 100.000002980232239, 18.0 ],
					"text" : "Amp Mod envelope"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-72",
					"maxclass" : "live.scope~",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "bang" ],
					"patching_rect" : [ 469.5, 475.333347499370575, 184.0, 68.0 ]
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-71",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 658.16667228937149, 358.666677355766296, 56.666668355464935, 18.0 ],
					"text" : "FM signal "
				}

			}
, 			{
				"box" : 				{
					"fontface" : 1,
					"id" : "obj-69",
					"linecount" : 2,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 52.333333939313889, 393.166678488254547, 119.333336889743805, 29.0 ],
					"text" : "DSP Amp Modulation\n Max/gen~",
					"textcolor" : [ 0.827450980392157, 0.086274509803922, 0.0, 1.0 ],
					"textjustification" : 1
				}

			}
, 			{
				"box" : 				{
					"fontface" : 1,
					"id" : "obj-67",
					"linecount" : 2,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 91.999998807907104, 294.500008881092072, 75.0, 29.0 ],
					"text" : "FM synthesis (ChucK)",
					"textcolor" : [ 0.827450980392157, 0.086274509803922, 0.0, 1.0 ],
					"textjustification" : 1
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-65",
					"maxclass" : "live.scope~",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "bang" ],
					"patching_rect" : [ 469.5, 338.000010073184967, 184.0, 68.0 ]
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-60",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 349.333343744277954, 348.000010371208191, 28.666667520999908, 18.0 ],
					"text" : "duty"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-59",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 304.000009059906006, 348.000010371208191, 28.666667520999908, 18.0 ],
					"text" : "freq"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-57",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 385.33334481716156, 348.000010371208191, 70.666668772697449, 18.0 ],
					"text" : "R Delay (ms)"
				}

			}
, 			{
				"box" : 				{
					"fontface" : 1,
					"fontsize" : 12.0,
					"id" : "obj-55",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 211.333339631557465, 332.666676580905914, 142.0, 20.0 ],
					"text" : "Amplitude Modulation"
				}

			}
, 			{
				"box" : 				{
					"fontsize" : 12.0,
					"id" : "obj-53",
					"maxclass" : "number",
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "bang" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 391.333344995975494, 362.666677474975586, 52.0, 22.0 ]
				}

			}
, 			{
				"box" : 				{
					"fontsize" : 12.0,
					"format" : 6,
					"id" : "obj-46",
					"maxclass" : "flonum",
					"maximum" : 1.0,
					"minimum" : 0.0,
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "bang" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 342.000010192394257, 362.666677474975586, 43.0, 22.0 ]
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-41",
					"maxclass" : "live.tab",
					"num_lines_patching" : 1,
					"num_lines_presentation" : 0,
					"numinlets" : 1,
					"numoutlets" : 3,
					"outlettype" : [ "", "", "float" ],
					"parameter_enable" : 1,
					"patching_rect" : [ 207.166667312383652, 362.666677474975586, 82.666669130325317, 19.333333909511566 ],
					"saved_attribute_attributes" : 					{
						"valueof" : 						{
							"parameter_enum" : [ "off", "on" ],
							"parameter_longname" : "live.tab",
							"parameter_mmax" : 1,
							"parameter_modmode" : 0,
							"parameter_shortname" : "live.tab",
							"parameter_type" : 2,
							"parameter_unitstyle" : 9
						}

					}
,
					"varname" : "live.tab"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-40",
					"maxclass" : "live.scope~",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "bang" ],
					"patching_rect" : [ 469.5, 410.000012218952179, 184.0, 62.000001847743988 ],
					"range" : [ -0.1, 1.1 ]
				}

			}
, 			{
				"box" : 				{
					"fontsize" : 12.0,
					"format" : 6,
					"id" : "obj-39",
					"maxclass" : "flonum",
					"minimum" : 0.0,
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "bang" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 297.33334219455719, 362.666677474975586, 42.0, 22.0 ]
				}

			}
, 			{
				"box" : 				{
					"fontsize" : 12.0,
					"id" : "obj-13",
					"maxclass" : "newobj",
					"numinlets" : 6,
					"numoutlets" : 3,
					"outlettype" : [ "signal", "signal", "signal" ],
					"patcher" : 					{
						"fileversion" : 1,
						"appversion" : 						{
							"major" : 8,
							"minor" : 6,
							"revision" : 2,
							"architecture" : "x64",
							"modernui" : 1
						}
,
						"classnamespace" : "dsp.gen",
						"rect" : [ 324.0, 299.0, 1124.0, 677.0 ],
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
						"boxes" : [ 							{
								"box" : 								{
									"id" : "obj-21",
									"linecount" : 8,
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 456.000013589859009, 54.666668295860291, 207.333339512348175, 96.0 ],
									"text" : "This gen circuit provides a simple amplitude envelope applied to the stereo input signal with a simple delay on the Right channel output. \n\nA uni-polar triangle osc is used as a multiplier on the signals. The duty input to the triangle operator will shape the output. "
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-19",
									"linecount" : 7,
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 452.000013470649719, 248.000007390975952, 150.0, 85.0 ],
									"text" : "delay applied to Right output\ninput is ms, converted to samples for delay object\n\na single channel delay is an easy way to add spatial depth and interest"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-3",
									"maxclass" : "newobj",
									"numinlets" : 1,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 379.333344638347626, 232.666673600673676, 60.0, 20.0 ],
									"text" : "mstosamps"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-16",
									"maxclass" : "newobj",
									"numinlets" : 0,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 379.333344638347626, 205.33333945274353, 143.0, 20.0 ],
									"text" : "in 6 ampModRdelay 0 @min 0"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-17",
									"maxclass" : "newobj",
									"numinlets" : 2,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 186.000005543231964, 260.000007748603821, 212.0, 20.0 ],
									"text" : "delay @name Rdelay @interp 1 @feedback 1"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-15",
									"maxclass" : "newobj",
									"numinlets" : 0,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 392.666678369045258, 14.000000417232513, 124.0, 20.0 ],
									"text" : "in 5 duty @min 0 @max 1"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-14",
									"maxclass" : "newobj",
									"numinlets" : 2,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 337.333343386650085, 72.000002145767212, 74.000002205371857, 20.0 ],
									"text" : "triangle"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-13",
									"maxclass" : "newobj",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 258.000007688999176, 170.666671752929688, 90.0, 20.0 ],
									"text" : "out 3 scopeOutput"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-12",
									"maxclass" : "newobj",
									"numinlets" : 0,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 232.666673600673676, 14.000000417232513, 82.0, 20.0 ],
									"text" : "in 3 ampModSW"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-11",
									"maxclass" : "newobj",
									"numinlets" : 0,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 337.333343386650085, 14.000000417232513, 45.0, 20.0 ],
									"text" : "in 4 freq"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-10",
									"maxclass" : "newobj",
									"numinlets" : 2,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 337.333343386650085, 43.333334624767303, 39.0, 20.0 ],
									"text" : "phasor"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-9",
									"maxclass" : "newobj",
									"numinlets" : 0,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 286.000008523464203, 116.000003457069397, 19.0, 20.0 ],
									"text" : "1"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-8",
									"maxclass" : "newobj",
									"numinlets" : 3,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 232.666673600673676, 139.333337485790253, 72.000002145767212, 20.0 ],
									"text" : "switch"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-7",
									"maxclass" : "newobj",
									"numinlets" : 2,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 186.000005543231964, 202.666672706604004, 29.5, 20.0 ],
									"text" : "*"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-6",
									"maxclass" : "newobj",
									"numinlets" : 2,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 50.0, 202.666672706604004, 29.5, 20.0 ],
									"text" : "*"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-5",
									"maxclass" : "newobj",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 186.000005543231964, 303.333342373371124, 57.0, 20.0 ],
									"text" : "out 2 Right"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-1",
									"maxclass" : "newobj",
									"numinlets" : 0,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 50.0, 14.0, 25.0, 20.0 ],
									"text" : "in 1"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-2",
									"maxclass" : "newobj",
									"numinlets" : 0,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 186.000005543231964, 14.000000417232513, 25.0, 20.0 ],
									"text" : "in 2"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-4",
									"maxclass" : "newobj",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 50.0, 303.333342373371124, 50.0, 20.0 ],
									"text" : "out 1 Left"
								}

							}
 ],
						"lines" : [ 							{
								"patchline" : 								{
									"destination" : [ "obj-6", 0 ],
									"source" : [ "obj-1", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-14", 0 ],
									"source" : [ "obj-10", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-10", 0 ],
									"source" : [ "obj-11", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-8", 0 ],
									"source" : [ "obj-12", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-8", 1 ],
									"midpoints" : [ 346.833343386650085, 101.333336353302002, 268.666674673557281, 101.333336353302002 ],
									"source" : [ "obj-14", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-14", 1 ],
									"source" : [ "obj-15", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-3", 0 ],
									"source" : [ "obj-16", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-5", 0 ],
									"source" : [ "obj-17", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-7", 0 ],
									"source" : [ "obj-2", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-17", 1 ],
									"source" : [ "obj-3", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-4", 0 ],
									"source" : [ "obj-6", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-17", 0 ],
									"source" : [ "obj-7", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-13", 0 ],
									"midpoints" : [ 242.166673600673676, 166.000004947185516, 267.500007688999176, 166.000004947185516 ],
									"order" : 0,
									"source" : [ "obj-8", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-6", 1 ],
									"midpoints" : [ 242.166673600673676, 192.66667240858078, 70.0, 192.66667240858078 ],
									"order" : 2,
									"source" : [ "obj-8", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-7", 1 ],
									"midpoints" : [ 242.166673600673676, 192.000005722045898, 206.000005543231964, 192.000005722045898 ],
									"order" : 1,
									"source" : [ "obj-8", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-8", 2 ],
									"source" : [ "obj-9", 0 ]
								}

							}
 ],
						"bgcolor" : [ 1.0, 1.0, 1.0, 1.0 ]
					}
,
					"patching_rect" : [ 168.666671693325043, 396.666678488254547, 188.0, 22.0 ],
					"text" : "gen~ @title ampMod"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-33",
					"linecount" : 3,
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 479.5, 562.000016748905182, 164.0, 42.0 ],
					"text" : ";\rmax launchbrowser https://github.com/HighHarmonics2"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-22",
					"linecount" : 2,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 314.0, 568.500016748905182, 165.0, 29.0 ],
					"text" : "by HighHarmonics2\nhttps://github.com/HighHarmonics2"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-11",
					"local" : 1,
					"maxclass" : "ezdac~",
					"numinlets" : 2,
					"numoutlets" : 0,
					"patching_rect" : [ 173.000007182359695, 517.333348751068115, 45.0, 45.0 ]
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"hidden" : 1,
					"id" : "obj-38",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 79.0, 58.66666841506958, 70.0, 22.0 ],
					"text" : "loadmess 1"
				}

			}
, 			{
				"box" : 				{
					"fontface" : 1,
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-36",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 361.0, 145.0, 134.0, 20.0 ],
					"text" : "FM synthesis values",
					"textjustification" : 1
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 10.0,
					"id" : "obj-34",
					"linecount" : 5,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 471.333347380161285, 240.000007152557373, 229.0, 62.0 ],
					"text" : "FM synthesis: modulator values are multipliers:\n   carrier freq * harmonicity = mod freq\n   mod freq * modIndex = mod gain\n\nAmplitude Modulation with Right channel Delay"
				}

			}
, 			{
				"box" : 				{
					"fontface" : 2,
					"fontname" : "Arial",
					"fontsize" : 10.0,
					"id" : "obj-32",
					"linecount" : 4,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 611.5, 97.0, 70.0, 51.0 ],
					"text" : "gap adds time (ms) between env loops"
				}

			}
, 			{
				"box" : 				{
					"fontface" : 2,
					"fontname" : "Verdana",
					"fontsize" : 10.0,
					"id" : "obj-25",
					"linecount" : 4,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 703.0, 95.0, 69.0, 55.0 ],
					"text" : "reverb mix only changes on new shred"
				}

			}
, 			{
				"box" : 				{
					"fontface" : 1,
					"fontname" : "Arial",
					"fontsize" : 18.0,
					"id" : "obj-216",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 343.666676908731461, 41.500001072883606, 155.0, 27.0 ],
					"text" : "ChucK => Max",
					"textjustification" : 1
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-31",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 116.0, 80.666669070720673, 63.0, 20.0 ],
					"text" : "presets",
					"textjustification" : 1
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Verdana",
					"id" : "obj-30",
					"maxclass" : "preset",
					"numinlets" : 1,
					"numoutlets" : 5,
					"outlettype" : [ "preset", "int", "preset", "int", "" ],
					"patching_rect" : [ 79.0, 96.000002861022949, 100.0, 40.0 ],
					"preset_data" : [ 						{
							"number" : 1,
							"data" : [ 5, "obj-63", "live.gain~", "float", -2.5, 5, "obj-52", "flonum", "float", 110.0, 5, "obj-50", "flonum", "float", 1.5, 5, "obj-2", "flonum", "float", 0.200000002980232, 5, "obj-8", "number", "int", 1000, 5, "obj-10", "flonum", "float", 3.099999904632568, 5, "obj-3", "number", "int", 0, 5, "obj-27", "flonum", "float", 0.200000002980232, 5, "obj-39", "flonum", "float", 1.0, 5, "obj-41", "live.tab", "float", 0.0, 5, "obj-46", "flonum", "float", 0.5, 5, "obj-53", "number", "int", 0 ]
						}
, 						{
							"number" : 2,
							"data" : [ 5, "obj-63", "live.gain~", "float", -2.5, 5, "obj-52", "flonum", "float", 110.0, 5, "obj-50", "flonum", "float", 1.5, 5, "obj-2", "flonum", "float", 0.349999994039536, 5, "obj-8", "number", "int", 1000, 5, "obj-10", "flonum", "float", 3.099999904632568, 5, "obj-3", "number", "int", 0, 5, "obj-27", "flonum", "float", 0.200000002980232, 5, "obj-39", "flonum", "float", 4.0, 5, "obj-41", "live.tab", "float", 1.0, 5, "obj-46", "flonum", "float", 0.5, 5, "obj-53", "number", "int", 20 ]
						}
, 						{
							"number" : 3,
							"data" : [ 5, "obj-63", "live.gain~", "float", -2.5, 5, "obj-52", "flonum", "float", 110.0, 5, "obj-50", "flonum", "float", 1.5, 5, "obj-2", "flonum", "float", 0.400000005960464, 5, "obj-8", "number", "int", 1000, 5, "obj-10", "flonum", "float", 6.900000095367432, 5, "obj-3", "number", "int", 0, 5, "obj-27", "flonum", "float", 0.200000002980232, 5, "obj-39", "flonum", "float", 8.0, 5, "obj-41", "live.tab", "float", 1.0, 5, "obj-46", "flonum", "float", 0.200000002980232, 5, "obj-53", "number", "int", 20 ]
						}
, 						{
							"number" : 4,
							"data" : [ 5, "obj-63", "live.gain~", "float", -2.5, 5, "obj-52", "flonum", "float", 110.0, 5, "obj-50", "flonum", "float", 2.609999895095825, 5, "obj-2", "flonum", "float", 0.200000002980232, 5, "obj-8", "number", "int", 1000, 5, "obj-10", "flonum", "float", 12.0, 5, "obj-3", "number", "int", 0, 5, "obj-27", "flonum", "float", 0.200000002980232, 5, "obj-39", "flonum", "float", 8.0, 5, "obj-41", "live.tab", "float", 1.0, 5, "obj-46", "flonum", "float", 0.810000002384186, 5, "obj-53", "number", "int", 50 ]
						}
, 						{
							"number" : 5,
							"data" : [ 5, "obj-63", "live.gain~", "float", -2.5, 5, "obj-52", "flonum", "float", 220.0, 5, "obj-50", "flonum", "float", 0.509999990463257, 5, "obj-2", "flonum", "float", 0.300000011920929, 5, "obj-8", "number", "int", 500, 5, "obj-10", "flonum", "float", 17.0, 5, "obj-3", "number", "int", 500, 5, "obj-27", "flonum", "float", 0.200000002980232, 5, "obj-39", "flonum", "float", 12.0, 5, "obj-41", "live.tab", "float", 1.0, 5, "obj-46", "flonum", "float", 0.129999995231628, 5, "obj-53", "number", "int", 100 ]
						}
, 						{
							"number" : 6,
							"data" : [ 5, "obj-63", "live.gain~", "float", -2.5, 5, "obj-52", "flonum", "float", 110.0, 5, "obj-50", "flonum", "float", 2.519999980926514, 5, "obj-2", "flonum", "float", 0.319999992847443, 5, "obj-8", "number", "int", 1000, 5, "obj-10", "flonum", "float", 7.199999809265137, 5, "obj-14", "attrui", "attr", "file", 4, "obj-14", "attrui", "/Users/one/dev/sound/chuck-max/examples/max/fmenv.ck", 5, "obj-3", "number", "int", 0, 5, "obj-27", "flonum", "float", 0.200000002980232, 5, "obj-39", "flonum", "float", 1.0, 5, "obj-41", "live.tab", "float", 1.0, 5, "obj-46", "flonum", "float", 0.5, 5, "obj-53", "number", "int", 250 ]
						}
, 						{
							"number" : 7,
							"data" : [ 5, "obj-63", "live.gain~", "float", -2.5, 5, "obj-52", "flonum", "float", 110.0, 5, "obj-50", "flonum", "float", 2.049999952316284, 5, "obj-2", "flonum", "float", 0.200000002980232, 5, "obj-8", "number", "int", 1000, 5, "obj-10", "flonum", "float", 8.0, 5, "obj-14", "attrui", "attr", "file", 4, "obj-14", "attrui", "/Users/one/dev/sound/chuck-max/examples/max/fmenv.ck", 5, "obj-3", "number", "int", 0, 5, "obj-27", "flonum", "float", 0.200000002980232, 5, "obj-39", "flonum", "float", 4.0, 5, "obj-41", "live.tab", "float", 1.0, 5, "obj-46", "flonum", "float", 0.5, 5, "obj-53", "number", "int", 120 ]
						}
, 						{
							"number" : 9,
							"data" : [ 5, "obj-63", "live.gain~", "float", -2.5, 5, "obj-52", "flonum", "float", 440.0, 5, "obj-50", "flonum", "float", 2.0, 5, "obj-2", "flonum", "float", 0.200000002980232, 5, "obj-8", "number", "int", 250, 5, "obj-10", "flonum", "float", 4.0, 5, "obj-3", "number", "int", 500, 5, "obj-27", "flonum", "float", 0.300000011920929, 5, "obj-39", "flonum", "float", 4.0, 5, "obj-41", "live.tab", "float", 0.0, 5, "obj-46", "flonum", "float", 0.5, 5, "obj-53", "number", "int", 0 ]
						}
, 						{
							"number" : 10,
							"data" : [ 5, "obj-63", "live.gain~", "float", -2.5, 5, "obj-52", "flonum", "float", 440.0, 5, "obj-50", "flonum", "float", 2.150000095367432, 5, "obj-2", "flonum", "float", 0.400000005960464, 5, "obj-8", "number", "int", 250, 5, "obj-10", "flonum", "float", 4.0, 5, "obj-3", "number", "int", 500, 5, "obj-27", "flonum", "float", 0.300000011920929, 5, "obj-39", "flonum", "float", 4.0, 5, "obj-41", "live.tab", "float", 1.0, 5, "obj-46", "flonum", "float", 0.5, 5, "obj-53", "number", "int", 100 ]
						}
, 						{
							"number" : 11,
							"data" : [ 5, "obj-63", "live.gain~", "float", -2.5, 5, "obj-52", "flonum", "float", 440.0, 5, "obj-50", "flonum", "float", 2.150000095367432, 5, "obj-2", "flonum", "float", 0.400000005960464, 5, "obj-8", "number", "int", 250, 5, "obj-10", "flonum", "float", 6.300000190734863, 5, "obj-3", "number", "int", 500, 5, "obj-27", "flonum", "float", 0.300000011920929, 5, "obj-39", "flonum", "float", 18.0, 5, "obj-41", "live.tab", "float", 1.0, 5, "obj-46", "flonum", "float", 0.0, 5, "obj-53", "number", "int", 100 ]
						}
, 						{
							"number" : 12,
							"data" : [ 5, "obj-63", "live.gain~", "float", -2.5, 5, "obj-52", "flonum", "float", 110.0, 5, "obj-50", "flonum", "float", 3.309999942779541, 5, "obj-2", "flonum", "float", 0.200000002980232, 5, "obj-8", "number", "int", 2000, 5, "obj-10", "flonum", "float", 6.5, 5, "obj-3", "number", "int", 0, 5, "obj-27", "flonum", "float", 0.400000005960464, 5, "obj-39", "flonum", "float", 16.0, 5, "obj-41", "live.tab", "float", 1.0, 5, "obj-46", "flonum", "float", 0.200000002980232, 5, "obj-53", "number", "int", 100 ]
						}
, 						{
							"number" : 13,
							"data" : [ 5, "obj-63", "live.gain~", "float", -2.5, 5, "obj-52", "flonum", "float", 220.0, 5, "obj-50", "flonum", "float", 0.800000011920929, 5, "obj-2", "flonum", "float", 0.270000010728836, 5, "obj-8", "number", "int", 1000, 5, "obj-10", "flonum", "float", 4.0, 5, "obj-3", "number", "int", 0, 5, "obj-27", "flonum", "float", 0.200000002980232, 5, "obj-39", "flonum", "float", 0.5, 5, "obj-41", "live.tab", "float", 1.0, 5, "obj-46", "flonum", "float", 0.200000002980232, 5, "obj-53", "number", "int", 200 ]
						}
, 						{
							"number" : 14,
							"data" : [ 5, "obj-63", "live.gain~", "float", -2.5, 5, "obj-52", "flonum", "float", 400.0, 5, "obj-50", "flonum", "float", 2.339999914169312, 5, "obj-2", "flonum", "float", 0.200000002980232, 5, "obj-8", "number", "int", 100, 5, "obj-10", "flonum", "float", 5.880000114440918, 5, "obj-3", "number", "int", 0, 5, "obj-27", "flonum", "float", 0.200000002980232, 5, "obj-39", "flonum", "float", 0.5, 5, "obj-41", "live.tab", "float", 1.0, 5, "obj-46", "flonum", "float", 0.270000010728836, 5, "obj-53", "number", "int", 100 ]
						}
, 						{
							"number" : 15,
							"data" : [ 5, "obj-63", "live.gain~", "float", -2.5, 5, "obj-52", "flonum", "float", 400.0, 5, "obj-50", "flonum", "float", 2.410000085830688, 5, "obj-2", "flonum", "float", 0.28999999165535, 5, "obj-8", "number", "int", 10, 5, "obj-10", "flonum", "float", 7.659999847412109, 5, "obj-3", "number", "int", 10, 5, "obj-27", "flonum", "float", 0.200000002980232, 5, "obj-39", "flonum", "float", 7.0, 5, "obj-41", "live.tab", "float", 1.0, 5, "obj-46", "flonum", "float", 0.730000019073486, 5, "obj-53", "number", "int", 500 ]
						}
, 						{
							"number" : 16,
							"data" : [ 5, "obj-63", "live.gain~", "float", -2.5, 5, "obj-52", "flonum", "float", 200.0, 5, "obj-50", "flonum", "float", 2.299999952316284, 5, "obj-2", "flonum", "float", 0.28999999165535, 5, "obj-8", "number", "int", 500, 5, "obj-10", "flonum", "float", 6.900000095367432, 5, "obj-3", "number", "int", 20, 5, "obj-27", "flonum", "float", 0.200000002980232, 5, "obj-39", "flonum", "float", 7.0, 5, "obj-41", "live.tab", "float", 0.0, 5, "obj-46", "flonum", "float", 0.730000019073486, 5, "obj-53", "number", "int", 500 ]
						}
, 						{
							"number" : 17,
							"data" : [ 5, "obj-63", "live.gain~", "float", -2.5, 5, "obj-52", "flonum", "float", 55.0, 5, "obj-50", "flonum", "float", 3.0, 5, "obj-2", "flonum", "float", 0.5, 5, "obj-8", "number", "int", 250, 5, "obj-10", "flonum", "float", 6.5, 5, "obj-3", "number", "int", 250, 5, "obj-27", "flonum", "float", 0.300000011920929, 5, "obj-39", "flonum", "float", 4.0, 5, "obj-41", "live.tab", "float", 0.0, 5, "obj-46", "flonum", "float", 0.439999997615814, 5, "obj-53", "number", "int", 250 ]
						}
, 						{
							"number" : 18,
							"data" : [ 5, "obj-63", "live.gain~", "float", -2.5, 5, "obj-52", "flonum", "float", 110.0, 5, "obj-50", "flonum", "float", 3.0, 5, "obj-2", "flonum", "float", 0.200000002980232, 5, "obj-8", "number", "int", 250, 5, "obj-10", "flonum", "float", 14.0, 5, "obj-3", "number", "int", 500, 5, "obj-27", "flonum", "float", 0.300000011920929, 5, "obj-39", "flonum", "float", 1.0, 5, "obj-41", "live.tab", "float", 0.0, 5, "obj-46", "flonum", "float", 0.5, 5, "obj-53", "number", "int", 250 ]
						}
, 						{
							"number" : 19,
							"data" : [ 5, "obj-63", "live.gain~", "float", -2.5, 5, "obj-52", "flonum", "float", 110.0, 5, "obj-50", "flonum", "float", 3.160000085830688, 5, "obj-2", "flonum", "float", 0.200000002980232, 5, "obj-8", "number", "int", 250, 5, "obj-10", "flonum", "float", 6.5, 5, "obj-3", "number", "int", 250, 5, "obj-27", "flonum", "float", 0.300000011920929, 5, "obj-39", "flonum", "float", 4.0, 5, "obj-41", "live.tab", "float", 1.0, 5, "obj-46", "flonum", "float", 0.5, 5, "obj-53", "number", "int", 100 ]
						}
 ]
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Verdana",
					"id" : "obj-29",
					"linecount" : 2,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 712.0, 151.0, 42.0, 31.0 ],
					"text" : "JCRev\nmix",
					"textjustification" : 1
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Verdana",
					"fontsize" : 10.0,
					"id" : "obj-28",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 714.0, 207.0, 58.0, 21.0 ],
					"text" : "revMix $1"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 0.211764705882353, 0.196078431372549, 0.776470588235294, 1.0 ],
					"fontname" : "Verdana",
					"format" : 6,
					"id" : "obj-27",
					"maxclass" : "flonum",
					"maximum" : 1.0,
					"minimum" : 0.0,
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "bang" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 714.0, 182.0, 41.0, 21.0 ]
				}

			}
, 			{
				"box" : 				{
					"fontface" : 1,
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-23",
					"linecount" : 2,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 146.923090934753418, 152.307706832885742, 43.0, 33.0 ],
					"text" : "run\nadd ",
					"textjustification" : 1
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-20",
					"linecount" : 2,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 611.0, 150.0, 71.0, 33.0 ],
					"text" : "gap duration ms",
					"textjustification" : 1
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-16",
					"linecount" : 2,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 525.0, 150.0, 77.0, 33.0 ],
					"text" : "envelope duration ms",
					"textjustification" : 1
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Verdana",
					"id" : "obj-15",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 438.0, 161.0, 75.0, 19.0 ],
					"text" : "mod Index",
					"textjustification" : 1
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-3",
					"maxclass" : "number",
					"maximum" : 20000,
					"minimum" : 0,
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "bang" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 624.0, 181.0, 52.0, 22.0 ]
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-7",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 618.0, 206.0, 87.0, 22.0 ],
					"text" : "gapTimeMs $1"
				}

			}
, 			{
				"box" : 				{
					"attr" : "file",
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-14",
					"maxclass" : "attrui",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 211.333339631557465, 269.333341360092163, 127.0, 22.0 ]
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
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-9",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 448.0, 206.0, 78.0, 22.0 ],
					"text" : "modIndex $1"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"format" : 6,
					"id" : "obj-10",
					"maxclass" : "flonum",
					"maximum" : 50.0,
					"minimum" : 0.1,
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "bang" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 448.0, 181.0, 50.0, 22.0 ]
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-8",
					"maxclass" : "number",
					"maximum" : 20000,
					"minimum" : 1,
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "bang" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 533.0, 181.0, 57.0, 22.0 ]
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-4",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 533.0, 206.0, 80.0, 22.0 ],
					"text" : "envDurMs $1"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Verdana",
					"id" : "obj-152",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 343.0, 161.0, 75.0, 19.0 ],
					"text" : "harmonicity",
					"textjustification" : 1
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-151",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 284.0, 161.0, 41.0, 20.0 ],
					"text" : "gain",
					"textjustification" : 1
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-150",
					"linecount" : 2,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 203.333339393138885, 152.000004529953003, 69.0, 33.0 ],
					"text" : "carrier frequency",
					"textjustification" : 1
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-6",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 279.0, 206.0, 67.0, 22.0 ],
					"text" : "carGain $1"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"format" : 6,
					"id" : "obj-2",
					"maxclass" : "flonum",
					"maximum" : 1.0,
					"minimum" : 0.0,
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "bang" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 279.0, 181.0, 42.0, 22.0 ]
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-49",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 353.0, 206.0, 87.0, 22.0 ],
					"text" : "harmonicity $1"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"format" : 6,
					"id" : "obj-50",
					"maxclass" : "flonum",
					"maximum" : 50.0,
					"minimum" : 0.1,
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "bang" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 353.0, 181.0, 52.0, 22.0 ]
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"format" : 6,
					"id" : "obj-52",
					"maxclass" : "flonum",
					"maximum" : 4000.0,
					"minimum" : 0.0,
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "bang" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 206.000006139278412, 183.333338797092438, 54.0, 22.0 ]
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-37",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 70.0, 144.0, 35.0, 22.0 ],
					"text" : "reset"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-21",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 206.000006139278412, 207.333339512348175, 67.0, 22.0 ],
					"text" : "carFreq $1"
				}

			}
, 			{
				"box" : 				{
					"fontface" : 1,
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-35",
					"linecount" : 2,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 293.666675418615341, 68.16666853427887, 254.0, 33.0 ],
					"text" : "FM Synthesis with Modulator Envelope\nAmplitude Modulation via gen~",
					"textjustification" : 1
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-18",
					"maxclass" : "newobj",
					"numinlets" : 0,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 170.000005066394806, 269.333341360092163, 28.0, 22.0 ],
					"text" : "r ck"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-17",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 392.0, 240.0, 30.0, 22.0 ],
					"text" : "s ck"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-5",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 107.333336532115936, 144.000004291534424, 34.0, 22.0 ],
					"text" : "info"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-167",
					"linecount" : 2,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 27.666666865348816, 185.0, 55.0, 33.0 ],
					"text" : "remove shred #",
					"textjustification" : 1
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-85",
					"maxclass" : "number",
					"maximum" : 20,
					"minimum" : 1,
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "bang" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 81.0, 185.0, 40.0, 22.0 ]
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-83",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 81.0, 208.0, 65.0, 22.0 ],
					"text" : "remove $1"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-42",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 80.000002384185791, 260.000007748603821, 64.0, 22.0 ],
					"text" : "remove all"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-44",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 78.666669011116028, 233.333340287208557, 71.0, 22.0 ],
					"text" : "remove last"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-61",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 2,
					"outlettype" : [ "signal", "signal" ],
					"patching_rect" : [ 173.000007182359695, 488.000014543533325, 136.0, 22.0 ],
					"text" : "limi~ 2 @threshold -0.5."
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Verdana",
					"fontsize" : 10.555534164108179,
					"id" : "obj-63",
					"lastchannelcount" : 0,
					"maxclass" : "live.gain~",
					"numinlets" : 2,
					"numoutlets" : 5,
					"orientation" : 1,
					"outlettype" : [ "signal", "signal", "", "float", "list" ],
					"parameter_enable" : 1,
					"patching_rect" : [ 168.666671693325043, 430.000012814998627, 151.0, 50.0 ],
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
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-1",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 2,
					"outlettype" : [ "signal", "signal" ],
					"patching_rect" : [ 170.000005066394806, 298.000008881092072, 107.0, 22.0 ],
					"text" : "chuck~ 2 fmenv.ck",
					"varname" : "chuck-4"
				}

			}
 ],
		"lines" : [ 			{
				"patchline" : 				{
					"destination" : [ "obj-13", 1 ],
					"midpoints" : [ 267.500005066394806, 325.333343029022217, 211.966671693325054, 325.333343029022217 ],
					"order" : 1,
					"source" : [ "obj-1", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-13", 0 ],
					"midpoints" : [ 179.500005066394806, 327.333329617977142, 178.166671693325043, 327.333329617977142 ],
					"source" : [ "obj-1", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-65", 0 ],
					"midpoints" : [ 267.500005066394806, 324.666676342487335, 302.500009298324585, 325.333343029022217, 479.0, 325.333343029022217 ],
					"order" : 0,
					"source" : [ "obj-1", 1 ]
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
					"midpoints" : [ 164.5, 259.666660904884338, 179.500005066394806, 259.666660904884338 ],
					"source" : [ "obj-12", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-40", 0 ],
					"midpoints" : [ 347.166671693325043, 426.666679382324219, 464.666680514812469, 426.666679382324219, 464.666680514812469, 406.666678786277771, 479.0, 406.666678786277771 ],
					"source" : [ "obj-13", 2 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-63", 1 ],
					"midpoints" : [ 262.666671693325043, 427.3333460688591, 310.166671693325043, 427.3333460688591 ],
					"source" : [ "obj-13", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-63", 0 ],
					"order" : 1,
					"source" : [ "obj-13", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-72", 0 ],
					"midpoints" : [ 178.166671693325043, 427.3333460688591, 462.666680455207825, 427.3333460688591, 462.666680455207825, 472.000014066696167, 479.0, 472.000014066696167 ],
					"order" : 0,
					"source" : [ "obj-13", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"midpoints" : [ 220.833339631557465, 295.666670083999634, 179.500005066394806, 295.666670083999634 ],
					"source" : [ "obj-14", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"midpoints" : [ 179.500005066394806, 297.33333683013916, 179.500005066394806, 297.33333683013916 ],
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
					"midpoints" : [ 215.500006139278412, 232.999993443489075, 401.5, 232.999993443489075 ],
					"source" : [ "obj-21", 0 ]
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
					"midpoints" : [ 723.5, 234.999993503093719, 401.5, 234.999993503093719 ],
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
					"midpoints" : [ 129.0, 138.666670799255371, 141.333337545394897, 138.666670799255371, 141.333337545394897, 175.333338558673859, 90.5, 175.333338558673859 ],
					"source" : [ "obj-30", 2 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"hidden" : 1,
					"midpoints" : [ 79.5, 291.666661858558655, 179.500005066394806, 291.666661858558655 ],
					"source" : [ "obj-37", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-30", 0 ],
					"hidden" : 1,
					"source" : [ "obj-38", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-13", 3 ],
					"midpoints" : [ 306.83334219455719, 389.000023543834686, 279.56667169332502, 389.000023543834686 ],
					"source" : [ "obj-39", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-17", 0 ],
					"midpoints" : [ 542.5, 234.999993503093719, 401.5, 234.999993503093719 ],
					"source" : [ "obj-4", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-13", 2 ],
					"midpoints" : [ 216.666667312383652, 387.000023484230042, 245.766671693325037, 387.000023484230042 ],
					"source" : [ "obj-41", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"hidden" : 1,
					"midpoints" : [ 89.500002384185791, 291.333328545093536, 179.500005066394806, 291.333328545093536 ],
					"source" : [ "obj-42", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"hidden" : 1,
					"midpoints" : [ 88.166669011116028, 259.33332759141922, 179.500005066394806, 259.33332759141922 ],
					"source" : [ "obj-44", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-13", 4 ],
					"midpoints" : [ 351.500010192394257, 389.666690230369568, 313.366671693325031, 389.666690230369568 ],
					"source" : [ "obj-46", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-17", 0 ],
					"midpoints" : [ 362.5, 232.333326756954193, 401.5, 232.333326756954193 ],
					"source" : [ "obj-49", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"hidden" : 1,
					"midpoints" : [ 116.833336532115936, 176.333325088024139, 79.333329111337662, 176.333325088024139, 79.333329111337662, 292.999995231628418, 179.500005066394806, 292.999995231628418 ],
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
					"destination" : [ "obj-13", 5 ],
					"midpoints" : [ 400.833344995975494, 390.333356916904449, 347.166671693325043, 390.333356916904449 ],
					"source" : [ "obj-53", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-17", 0 ],
					"midpoints" : [ 288.5, 232.333326756954193, 401.5, 232.333326756954193 ],
					"source" : [ "obj-6", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-11", 1 ],
					"midpoints" : [ 299.500007182359695, 514.866651654243469, 208.500007182359695, 514.866651654243469 ],
					"source" : [ "obj-61", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-11", 0 ],
					"source" : [ "obj-61", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-61", 1 ],
					"midpoints" : [ 211.166671693325043, 490.199986040592194, 299.500007182359695, 490.199986040592194 ],
					"source" : [ "obj-63", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-61", 0 ],
					"midpoints" : [ 178.166671693325043, 486.199985921382904, 182.500007182359695, 486.199985921382904 ],
					"source" : [ "obj-63", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-17", 0 ],
					"midpoints" : [ 627.5, 234.333326816558838, 401.5, 234.333326816558838 ],
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
					"hidden" : 1,
					"midpoints" : [ 90.5, 230.666660070419312, 179.500005066394806, 230.666660070419312 ],
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
					"midpoints" : [ 457.5, 233.666660130023956, 401.5, 233.666660130023956 ],
					"source" : [ "obj-9", 0 ]
				}

			}
 ],
		"parameters" : 		{
			"obj-41" : [ "live.tab", "live.tab", 0 ],
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
