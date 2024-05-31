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
		"rect" : [ 885.0, 134.0, 1280.0, 837.0 ],
		"bglocked" : 0,
		"openinpresentation" : 1,
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
					"fontsize" : 12.0,
					"id" : "obj-99",
					"linecount" : 9,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 655.333352863788605, 388.000011563301086, 174.0, 127.0 ],
					"presentation" : 1,
					"presentation_linecount" : 7,
					"presentation_rect" : [ 518.000015437602997, 275.333341538906097, 226.666673421859741, 100.0 ],
					"text" : "ChucK  fmenv-event.ck\n\nThis illustrates how to use event processing with chuck~. A public class is created so that multiple event shreds can be used. This is important when events have overlapping durations."
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-95",
					"linecount" : 2,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 381.000014364719391, 33.33333432674408, 150.0, 29.0 ],
					"text" : "OSC routing & parameter mapping"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-93",
					"linecount" : 4,
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 846.00002521276474, 866.666692495346069, 164.0, 53.0 ],
					"presentation" : 1,
					"presentation_linecount" : 2,
					"presentation_rect" : [ 1.666666060686111, 452.000013470649719, 335.0, 31.0 ],
					"text" : ";\rmax launchbrowser https://youtu.be/ajA6BjsLuoU?si=oNzv8zFCTZxrjakW"
				}

			}
, 			{
				"box" : 				{
					"fontsize" : 12.0,
					"id" : "obj-92",
					"linecount" : 15,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 972.000028967857361, 278.000012069940567, 416.0, 208.0 ],
					"presentation" : 1,
					"presentation_linecount" : 12,
					"presentation_rect" : [ 1.666666060686111, 275.333341538906097, 507.333348453044891, 167.0 ],
					"text" : "Purpose\n\nThis patch demonstrates how ChucK => Max can be used in either standalone mode or being controlled from an external program via OSC messages. \n\nTidalcycles files included with the patch are ready to use and will send OSC messages with parameter values. This shows the versatility of ChucK => Max and how to map the Max/MSP patch for use with a live coding platform (Tidalcycles: https://tidalcycles.org). See the README file included with the ChucK / Max files for more detail. \n\nA YouTube recording is available which shows a full scale solution based on these concepts:\nhttps://youtu.be/ajA6BjsLuoU?si=oNzv8zFCTZxrjakW"
				}

			}
, 			{
				"box" : 				{
					"fontsize" : 12.0,
					"id" : "obj-89",
					"linecount" : 13,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 972.000028967857361, 81.333339542150497, 416.0, 181.0 ],
					"presentation" : 1,
					"presentation_linecount" : 17,
					"presentation_rect" : [ 518.000015437602997, 4.666669607162476, 219.333339869976044, 234.0 ],
					"text" : "Usage notes:\n• event trigger will generate a single pass of the FM envelope, using the envelope duration (ms)\n• loop on will repeat the envelope\n• gap duration (ms) will add time before the next envelope starts\n\nModulator envelope\n• attack, decay, release values represent the % of the total envelope duration time.\n• release will add time to the duration\n• envelope curves: values  0.01 - 8\n   < 1 are sharp, > 1 are soft\n• carrier envelope divider sets how sharp the amplitude envelope is"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-71",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 460.000014364719391, 417.333345770835876, 60.0, 20.0 ],
					"text" : "loadmess 0"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-90",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 18,
					"outlettype" : [ "", "float", "float", "float", "float", "float", "int", "float", "float", "float", "float", "float", "float", "float", "int", "float", "float", "int" ],
					"patching_rect" : [ 289.666674315929413, 82.0, 197.5, 20.0 ],
					"text" : "unpack s f f f f f i f f f f f f f i f f i"
				}

			}
, 			{
				"box" : 				{
					"fontsize" : 12.0,
					"id" : "obj-88",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 318.000009477138519, 29.666667342185974, 44.0, 20.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 46.666668057441711, 78.000002324581146, 35.0, 20.0 ],
					"text" : "OSC"
				}

			}
, 			{
				"box" : 				{
					"fontsize" : 8.0,
					"id" : "obj-84",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 289.666674315929413, 4.666666686534882, 49.0, 17.0 ],
					"text" : "loadmess 1"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-86",
					"maxclass" : "toggle",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "int" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 289.666674315929413, 26.666667342185974, 24.0, 24.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 52.166668057441711, 51.750001490116119, 24.0, 24.0 ]
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-76",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "" ],
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
						"classnamespace" : "box",
						"rect" : [ 783.0, 438.0, 964.0, 409.0 ],
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
									"id" : "obj-5",
									"maxclass" : "newobj",
									"numinlets" : 2,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 101.282052278518677, 49.466666698455811, 70.000003814697266, 20.0 ],
									"text" : "gate"
								}

							}
, 							{
								"box" : 								{
									"comment" : "",
									"id" : "obj-3",
									"index" : 1,
									"maxclass" : "inlet",
									"numinlets" : 0,
									"numoutlets" : 1,
									"outlettype" : [ "int" ],
									"patching_rect" : [ 101.282052278518677, 2.800000190734863, 30.0, 30.0 ]
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-4",
									"linecount" : 4,
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 308.0, 20.800000309944153, 236.400001287460327, 51.0 ],
									"text" : "OSC Routing subpatch is designed to receive all OSC messages from the Tidal target designed for Max messages and route them to outlets based on the orbit number."
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-2",
									"linecount" : 8,
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 280.0, 83.738459348678589, 317.200002491474152, 96.0 ],
									"text" : "OSC target is set up in Tidal using the \"/play\" address on\nport (5050). \n\nThe first route object matches on the /play address.\n\nThe 2nd route object corresponds to the first argument which is the orbit number. Here the orbit number directs the remaining address message to its own outlet. "
								}

							}
, 							{
								"box" : 								{
									"comment" : "orbit1",
									"id" : "obj-32",
									"index" : 2,
									"maxclass" : "outlet",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 141.282056093215942, 149.738459348678589, 30.0, 30.0 ]
								}

							}
, 							{
								"box" : 								{
									"comment" : "orbit0",
									"id" : "obj-31",
									"index" : 1,
									"maxclass" : "outlet",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 101.282052278518677, 149.738459348678589, 30.0, 30.0 ]
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-29",
									"maxclass" : "newobj",
									"numinlets" : 3,
									"numoutlets" : 3,
									"outlettype" : [ "", "", "" ],
									"patching_rect" : [ 101.282052278518677, 104.276935338973999, 99.000007629394531, 20.0 ],
									"text" : "route 0 1"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-42",
									"maxclass" : "newobj",
									"numinlets" : 1,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 152.282056093215942, 12.800000190734863, 82.0, 20.0 ],
									"text" : "udpreceive 5050"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-41",
									"maxclass" : "newobj",
									"numinlets" : 2,
									"numoutlets" : 2,
									"outlettype" : [ "", "" ],
									"patching_rect" : [ 101.282052278518677, 76.174369394779205, 55.0, 20.0 ],
									"text" : "route /play"
								}

							}
 ],
						"lines" : [ 							{
								"patchline" : 								{
									"destination" : [ "obj-31", 0 ],
									"source" : [ "obj-29", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-32", 0 ],
									"source" : [ "obj-29", 1 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-5", 0 ],
									"source" : [ "obj-3", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-29", 0 ],
									"source" : [ "obj-41", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-5", 1 ],
									"source" : [ "obj-42", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-41", 0 ],
									"source" : [ "obj-5", 0 ]
								}

							}
 ],
						"bgcolor" : [ 1.0, 1.0, 1.0, 1.0 ]
					}
,
					"patching_rect" : [ 289.666674315929413, 55.333334982395172, 74.333334743976593, 20.0 ],
					"saved_object_attributes" : 					{
						"description" : "",
						"digest" : "",
						"fontsize" : 10.0,
						"globalpatchername" : "",
						"locked_bgcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
						"tags" : ""
					}
,
					"text" : "p oscRouting"
				}

			}
, 			{
				"box" : 				{
					"fontsize" : 12.0,
					"id" : "obj-75",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 590.5, 455.666680216789246, 53.0, 20.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 77.333335638046265, 5.999999791383743, 56.0, 20.0 ],
					"text" : "loop ms"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Verdana",
					"fontsize" : 12.0,
					"id" : "obj-27",
					"linecount" : 2,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 541.333348721265793, 156.33333832025528, 52.0, 36.0 ],
					"presentation" : 1,
					"presentation_linecount" : 2,
					"presentation_rect" : [ 378.666677951812744, 156.666671335697174, 50.000001490116119, 36.0 ],
					"text" : "reverb  Mix",
					"textjustification" : 1
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 10.0,
					"id" : "obj-25",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 539.333349406719208, 201.666673004627228, 52.0, 20.0 ],
					"text" : "revMix $1"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"format" : 6,
					"id" : "obj-22",
					"maxclass" : "flonum",
					"maximum" : 0.8,
					"minimum" : 0.0,
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "bang" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 539.333349406719208, 176.333338916301727, 52.0, 22.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 378.666677951812744, 188.66667228937149, 50.0, 22.0 ]
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-157",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 718.000021457672119, 372.000012069940567, 60.0, 20.0 ],
					"text" : "loadmess 8"
				}

			}
, 			{
				"box" : 				{
					"fontsize" : 12.0,
					"id" : "obj-156",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 762.500023782253265, 401.666678488254547, 70.0, 20.0 ],
					"presentation" : 1,
					"presentation_linecount" : 2,
					"presentation_rect" : [ 440.000013113021851, 158.000004708766937, 51.333334863185883, 33.0 ],
					"text" : "car env div",
					"textjustification" : 1
				}

			}
, 			{
				"box" : 				{
					"fontsize" : 8.0,
					"id" : "obj-153",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 720.000021457672119, 422.000012576580048, 56.0, 17.0 ],
					"text" : "carEnvDiv $1"
				}

			}
, 			{
				"box" : 				{
					"fontsize" : 12.0,
					"id" : "obj-149",
					"maxclass" : "number",
					"maximum" : 100,
					"minimum" : 2,
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "bang" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 720.000021457672119, 396.666678488254547, 42.0, 22.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 444.666679918766022, 188.66667228937149, 42.0, 22.0 ]
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-145",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 218.166672050952911, 83.0, 53.0, 18.0 ],
					"text" : "s param"
				}

			}
, 			{
				"box" : 				{
					"annotation" : "route object filters the \"s\" parameter of the OSC message\nonly values of \"h\" will go to the event trigger\nthis allows other values of \"s\" to send parameter changes without triggering the event, ie, for continuous parameter changes during a sustained event",
					"id" : "obj-72",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 2,
					"outlettype" : [ "", "" ],
					"patching_rect" : [ 211.333339631557465, 110.000003457069397, 40.0, 20.0 ],
					"text" : "route h"
				}

			}
, 			{
				"box" : 				{
					"fontsize" : 12.0,
					"id" : "obj-141",
					"maxclass" : "number",
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "bang" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 80.000002384185791, 301.500008881092072, 37.0, 22.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 10.000000298023224, 188.66667228937149, 33.0, 22.0 ]
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-158",
					"maxclass" : "newobj",
					"numinlets" : 3,
					"numoutlets" : 1,
					"outlettype" : [ "int" ],
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
						"classnamespace" : "box",
						"rect" : [ 712.0, 345.0, 681.0, 452.0 ],
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
									"id" : "obj-10",
									"maxclass" : "newobj",
									"numinlets" : 2,
									"numoutlets" : 1,
									"outlettype" : [ "int" ],
									"patching_rect" : [ 112.250003337860107, 383.333344757556915, 29.5, 20.0 ],
									"text" : "+"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-9",
									"maxclass" : "newobj",
									"numinlets" : 1,
									"numoutlets" : 2,
									"outlettype" : [ "bang", "float" ],
									"patching_rect" : [ 163.33333820104599, 280.000008344650269, 52.666668236255646, 20.0 ],
									"text" : "t b f"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-147",
									"maxclass" : "newobj",
									"numinlets" : 1,
									"numoutlets" : 2,
									"outlettype" : [ "bang", "int" ],
									"patching_rect" : [ 144.666670978069305, 352.00001049041748, 56.666668355464935, 20.0 ],
									"text" : "t b i"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-155",
									"maxclass" : "newobj",
									"numinlets" : 2,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 112.000003337860107, 322.000009596347809, 104.000003099441528, 20.0 ],
									"text" : "expr $f1 + ($f2 * $f1)"
								}

							}
, 							{
								"box" : 								{
									"comment" : "",
									"id" : "obj-8",
									"index" : 1,
									"maxclass" : "outlet",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 112.250003337860107, 419.333345830440521, 30.0, 30.0 ]
								}

							}
, 							{
								"box" : 								{
									"comment" : "",
									"id" : "obj-6",
									"index" : 3,
									"maxclass" : "inlet",
									"numinlets" : 0,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 234.66667366027832, 228.666673481464386, 30.0, 30.0 ]
								}

							}
, 							{
								"box" : 								{
									"comment" : "",
									"id" : "obj-4",
									"index" : 2,
									"maxclass" : "inlet",
									"numinlets" : 0,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 163.33333820104599, 228.666673481464386, 30.0, 30.0 ]
								}

							}
, 							{
								"box" : 								{
									"comment" : "",
									"id" : "obj-2",
									"index" : 1,
									"maxclass" : "inlet",
									"numinlets" : 0,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 112.000003337860107, 228.666673481464386, 30.0, 30.0 ]
								}

							}
 ],
						"lines" : [ 							{
								"patchline" : 								{
									"destination" : [ "obj-8", 0 ],
									"source" : [ "obj-10", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-10", 1 ],
									"midpoints" : [ 191.833339333534241, 378.000011265277863, 132.250003337860107, 378.000011265277863 ],
									"source" : [ "obj-147", 1 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-10", 0 ],
									"midpoints" : [ 154.166670978069305, 378.000011265277863, 121.750003337860107, 378.000011265277863 ],
									"source" : [ "obj-147", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-10", 0 ],
									"source" : [ "obj-155", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-155", 0 ],
									"source" : [ "obj-2", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-9", 0 ],
									"source" : [ "obj-4", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-147", 0 ],
									"midpoints" : [ 244.16667366027832, 347.333343684673309, 154.166670978069305, 347.333343684673309 ],
									"source" : [ "obj-6", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-155", 1 ],
									"source" : [ "obj-9", 1 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-155", 0 ],
									"midpoints" : [ 172.83333820104599, 309.333342552185059, 121.500003337860107, 309.333342552185059 ],
									"source" : [ "obj-9", 0 ]
								}

							}
 ],
						"bgcolor" : [ 1.0, 1.0, 1.0, 1.0 ]
					}
,
					"patching_rect" : [ 528.666682422161102, 417.333345770835876, 79.0, 20.0 ],
					"saved_object_attributes" : 					{
						"description" : "",
						"digest" : "",
						"fontsize" : 10.0,
						"globalpatchername" : "",
						"locked_bgcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
						"tags" : ""
					}
,
					"text" : "p calcLoopTime"
				}

			}
, 			{
				"box" : 				{
					"fontsize" : 12.0,
					"id" : "obj-154",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 454.666680216789246, 456.000013589859009, 35.0, 20.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 45.333334684371948, 5.999999791383743, 35.0, 20.0 ],
					"text" : "loop"
				}

			}
, 			{
				"box" : 				{
					"fontsize" : 12.0,
					"id" : "obj-143",
					"maxclass" : "number",
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "bang" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 528.666682422161102, 454.666680216789246, 56.0, 22.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 77.333335638046265, 25.500000908970833, 56.0, 22.0 ]
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-140",
					"maxclass" : "toggle",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "int" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 488.666681230068207, 450.666680097579956, 24.0, 24.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 52.666668236255646, 25.3333340883255, 22.333333641290665, 22.333333641290665 ]
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-82",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "bang" ],
					"patching_rect" : [ 488.666681230068207, 480.666680991649628, 59.0, 20.0 ],
					"text" : "metro 1000"
				}

			}
, 			{
				"box" : 				{
					"fontsize" : 10.0,
					"id" : "obj-138",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 26.666667461395264, 331.333343148231506, 48.0, 18.0 ],
					"presentation" : 1,
					"presentation_linecount" : 2,
					"presentation_rect" : [ 44.000001311302185, 188.66667228937149, 29.0, 29.0 ],
					"text" : "log level",
					"textjustification" : 1
				}

			}
, 			{
				"box" : 				{
					"fontface" : 0,
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-96",
					"linecount" : 2,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 242.000006258487701, 232.333340704441071, 45.0, 33.0 ],
					"presentation" : 1,
					"presentation_linecount" : 2,
					"presentation_rect" : [ 1.333333373069763, 78.000002324581146, 45.0, 33.0 ],
					"text" : "event\ntrigger",
					"textjustification" : 1
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-144",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 211.333339631557465, 264.00000786781311, 30.0, 20.0 ],
					"text" : "sig e"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-142",
					"maxclass" : "button",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "bang" ],
					"parameter_enable" : 1,
					"patching_rect" : [ 211.333339631557465, 232.333340704441071, 28.666666626930237, 28.666666626930237 ],
					"presentation" : 1,
					"presentation_rect" : [ 10.000000298023224, 50.000001490116119, 27.5, 27.5 ],
					"saved_attribute_attributes" : 					{
						"valueof" : 						{
							"parameter_enum" : [ "off", "on" ],
							"parameter_longname" : "button",
							"parameter_mmax" : 1,
							"parameter_modmode" : 0,
							"parameter_shortname" : "button",
							"parameter_type" : 2
						}

					}
,
					"varname" : "button"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-108",
					"maxclass" : "number",
					"minimum" : 1,
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "bang" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 18.000000536441803, 55.333334982395172, 33.33333432674408, 20.0 ]
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-31",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "bang", "bang" ],
					"patching_rect" : [ 155.333337962627411, 199.333339273929596, 29.5, 20.0 ],
					"text" : "t b b"
				}

			}
, 			{
				"box" : 				{
					"fontsize" : 12.0,
					"id" : "obj-134",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 173.333338141441345, 84.5, 46.0, 20.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 6.999999433755875, 7.166666805744171, 41.0, 20.0 ],
					"text" : "on/off"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-116",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 893.666694194078445, 276.666674911975861, 60.0, 20.0 ],
					"text" : "loadmess 1"
				}

			}
, 			{
				"box" : 				{
					"fontsize" : 8.0,
					"id" : "obj-73",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 10.000000298023224, 267.333341240882874, 49.0, 17.0 ],
					"text" : "loadmess 5"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-74",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 80.000002384185791, 329.333343148231506, 56.0, 20.0 ],
					"text" : "loglevel $1"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-70",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 624.666685283184052, 382.666678071022034, 30.0, 22.0 ],
					"text" : "s ck"
				}

			}
, 			{
				"box" : 				{
					"fontface" : 1,
					"fontsize" : 12.0,
					"id" : "obj-14",
					"linecount" : 2,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 504.000015079975128, 276.666674911975861, 80.0, 33.0 ],
					"presentation" : 1,
					"presentation_linecount" : 2,
					"presentation_rect" : [ 110.000003278255463, 102.666669726371765, 70.000002086162567, 33.0 ],
					"text" : "modulator env",
					"textjustification" : 1
				}

			}
, 			{
				"box" : 				{
					"fontsize" : 12.0,
					"id" : "obj-48",
					"linecount" : 2,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 354.000010550022125, 325.333343029022217, 51.0, 33.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 221.333339929580688, 134.666670680046082, 103.0, 20.0 ],
					"text" : "% of env dur",
					"textjustification" : 1
				}

			}
, 			{
				"box" : 				{
					"fontsize" : 12.0,
					"id" : "obj-45",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 630.666685461997986, 300.000008940696716, 42.0, 20.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 318.000009477138519, 98.666669607162476, 42.0, 20.0 ],
					"text" : "rel",
					"textjustification" : 1
				}

			}
, 			{
				"box" : 				{
					"fontsize" : 12.0,
					"format" : 6,
					"id" : "obj-26",
					"maxclass" : "flonum",
					"maximum" : 20.0,
					"minimum" : 0.0,
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "bang" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 624.666685283184052, 320.000009536743164, 47.0, 22.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 313.333342671394348, 114.666670083999634, 47.0, 22.0 ]
				}

			}
, 			{
				"box" : 				{
					"fontsize" : 8.0,
					"id" : "obj-47",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 624.666685283184052, 345.333343625068665, 70.0, 17.0 ],
					"text" : "gModRelPrcnt $1"
				}

			}
, 			{
				"box" : 				{
					"fontsize" : 12.0,
					"id" : "obj-29",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 558.000016629695892, 300.000008940696716, 49.0, 20.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 276.000008225440979, 98.666669607162476, 49.0, 20.0 ],
					"text" : "sus lev",
					"textjustification" : 1
				}

			}
, 			{
				"box" : 				{
					"fontsize" : 12.0,
					"format" : 6,
					"id" : "obj-43",
					"maxclass" : "flonum",
					"maximum" : 1.0,
					"minimum" : 0.0,
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "bang" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 553.333349823951721, 320.000009536743164, 47.0, 22.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 273.333341479301453, 114.666670083999634, 43.0, 22.0 ]
				}

			}
, 			{
				"box" : 				{
					"fontsize" : 8.0,
					"id" : "obj-32",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 553.333349823951721, 345.333343625068665, 66.0, 17.0 ],
					"text" : "gModSusLev $1"
				}

			}
, 			{
				"box" : 				{
					"fontsize" : 12.0,
					"id" : "obj-34",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 482.000014364719391, 300.000008940696716, 44.0, 20.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 233.333340287208557, 98.666669607162476, 44.0, 20.0 ],
					"text" : "decay",
					"textjustification" : 1
				}

			}
, 			{
				"box" : 				{
					"fontsize" : 12.0,
					"format" : 6,
					"id" : "obj-33",
					"maxclass" : "flonum",
					"maximum" : 1.0,
					"minimum" : 0.0,
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "bang" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 476.000014185905457, 320.000009536743164, 47.0, 22.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 227.333340108394623, 114.666670083999634, 47.0, 22.0 ]
				}

			}
, 			{
				"box" : 				{
					"fontsize" : 8.0,
					"id" : "obj-40",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 476.000014185905457, 345.333343625068665, 73.0, 17.0 ],
					"text" : "gModDecPrcnt $1"
				}

			}
, 			{
				"box" : 				{
					"fontsize" : 12.0,
					"id" : "obj-51",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 419.833345711231232, 301.500008881092072, 36.0, 20.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 191.333339035511017, 98.666669607162476, 36.0, 20.0 ],
					"text" : "attck",
					"textjustification" : 1
				}

			}
, 			{
				"box" : 				{
					"fontsize" : 12.0,
					"format" : 6,
					"id" : "obj-54",
					"maxclass" : "flonum",
					"maximum" : 1.0,
					"minimum" : 0.0,
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "bang" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 401.333345293998718, 320.000009536743164, 47.0, 22.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 182.000005424022675, 114.666670083999634, 47.0, 22.0 ]
				}

			}
, 			{
				"box" : 				{
					"fontsize" : 8.0,
					"id" : "obj-56",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 401.333345293998718, 345.333343625068665, 68.0, 17.0 ],
					"text" : "gModAttPrcnt $1"
				}

			}
, 			{
				"box" : 				{
					"fontsize" : 12.0,
					"id" : "obj-58",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 852.000025391578674, 300.000008940696716, 35.0, 20.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 462.000013768672943, 99.333336293697357, 35.0, 20.0 ],
					"text" : "rel"
				}

			}
, 			{
				"box" : 				{
					"fontsize" : 12.0,
					"id" : "obj-62",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 784.500023782253265, 300.000008940696716, 37.0, 20.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 423.333345949649811, 99.333336293697357, 36.0, 20.0 ],
					"text" : "dec"
				}

			}
, 			{
				"box" : 				{
					"fontsize" : 12.0,
					"id" : "obj-64",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 720.000021457672119, 300.000008940696716, 36.0, 20.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 382.000011384487152, 99.333336293697357, 36.0, 20.0 ],
					"text" : "attck"
				}

			}
, 			{
				"box" : 				{
					"fontsize" : 12.0,
					"id" : "obj-65",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 758.666689276695251, 283.000008940696716, 113.0, 20.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 397.333345174789429, 134.666670680046082, 83.333334445953369, 20.0 ],
					"text" : "env curves",
					"textjustification" : 1
				}

			}
, 			{
				"box" : 				{
					"fontsize" : 12.0,
					"format" : 6,
					"id" : "obj-66",
					"maxclass" : "flonum",
					"maximum" : 8.0,
					"minimum" : 0.3,
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "bang" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 846.00002521276474, 320.000009536743164, 47.0, 22.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 457.000013768672943, 115.333336770534515, 40.0, 22.0 ]
				}

			}
, 			{
				"box" : 				{
					"fontsize" : 8.0,
					"id" : "obj-67",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 846.00002521276474, 345.333343625068665, 64.0, 17.0 ],
					"text" : "gModRelCrv $1"
				}

			}
, 			{
				"box" : 				{
					"fontsize" : 12.0,
					"format" : 6,
					"id" : "obj-68",
					"maxclass" : "flonum",
					"maximum" : 8.0,
					"minimum" : 0.01,
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "bang" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 776.666689813137054, 320.000009536743164, 47.0, 22.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 416.666679084300995, 115.333336770534515, 39.0, 22.0 ]
				}

			}
, 			{
				"box" : 				{
					"fontsize" : 8.0,
					"id" : "obj-69",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 776.666689813137054, 345.333343625068665, 66.0, 17.0 ],
					"text" : "gModDecCrv $1"
				}

			}
, 			{
				"box" : 				{
					"fontsize" : 12.0,
					"format" : 6,
					"id" : "obj-110",
					"maxclass" : "flonum",
					"maximum" : 8.0,
					"minimum" : 0.01,
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "bang" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 708.00002110004425, 320.000009536743164, 47.0, 22.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 374.666677832603455, 115.333336770534515, 40.0, 22.0 ]
				}

			}
, 			{
				"box" : 				{
					"fontsize" : 8.0,
					"id" : "obj-111",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 708.00002110004425, 345.333343625068665, 62.0, 17.0 ],
					"text" : "gModAttCrv $1"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-28",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 178.000005304813385, 133.333337962627411, 60.0, 20.0 ],
					"text" : "synthOn $1"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-19",
					"maxclass" : "toggle",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "int" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 178.000005304813385, 108.000003457069397, 24.0, 24.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 10.000000298023224, 24.666667401790619, 24.0, 24.0 ]
				}

			}
, 			{
				"box" : 				{
					"fontsize" : 12.0,
					"id" : "obj-24",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 254.666674315929413, 615.333351671695709, 72.0, 22.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 328.000009775161743, 226.666673421859741, 72.0, 22.0 ],
					"text" : "startwindow"
				}

			}
, 			{
				"box" : 				{
					"fontsize" : 12.0,
					"id" : "obj-60",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 354.000010550022125, 394.000012069940567, 34.0, 20.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 257.333341002464294, 171.333338439464569, 34.0, 20.0 ],
					"text" : "duty"
				}

			}
, 			{
				"box" : 				{
					"fontsize" : 12.0,
					"id" : "obj-59",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 311.333342611789703, 394.000012069940567, 33.0, 20.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 223.333339989185333, 171.333338439464569, 33.0, 20.0 ],
					"text" : "freq"
				}

			}
, 			{
				"box" : 				{
					"fontsize" : 12.0,
					"id" : "obj-57",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 407.333345472812653, 394.000012069940567, 85.0, 20.0 ],
					"presentation" : 1,
					"presentation_linecount" : 2,
					"presentation_rect" : [ 315.333342730998993, 158.666671395301819, 61.166662693023682, 33.0 ],
					"text" : "R Delay (ms)",
					"textjustification" : 1
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
					"patching_rect" : [ 142.666665941476822, 408.333346128463745, 66.0, 20.0 ],
					"presentation" : 1,
					"presentation_linecount" : 2,
					"presentation_rect" : [ 124.000003695487976, 181.333338737487793, 37.0, 33.0 ],
					"text" : "amp mod",
					"textjustification" : 1
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
					"patching_rect" : [ 392.000012934207916, 408.666679173707962, 53.0, 22.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 319.333342850208282, 188.66667228937149, 53.0, 22.0 ]
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
					"patching_rect" : [ 342.666678130626678, 408.666679173707962, 44.0, 22.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 251.33334082365036, 188.000005602836609, 44.0, 22.0 ]
				}

			}
, 			{
				"box" : 				{
					"fontsize" : 12.0,
					"id" : "obj-41",
					"maxclass" : "live.tab",
					"num_lines_patching" : 1,
					"num_lines_presentation" : 1,
					"numinlets" : 1,
					"numoutlets" : 3,
					"outlettype" : [ "", "", "float" ],
					"parameter_enable" : 1,
					"patching_rect" : [ 208.000007450580597, 408.666679173707962, 82.666669130325317, 19.333333909511566 ],
					"presentation" : 1,
					"presentation_rect" : [ 162.666671514511108, 188.000005602836609, 46.666668057441711, 20.000000596046448 ],
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
					"fontsize" : 12.0,
					"format" : 6,
					"id" : "obj-39",
					"maxclass" : "flonum",
					"minimum" : 0.0,
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "bang" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 298.000010132789612, 408.666679173707962, 43.0, 22.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 210.666672945022583, 188.000005602836609, 43.0, 22.0 ]
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
					"patching_rect" : [ 170.000006318092346, 448.666680365800858, 156.666671335697174, 22.0 ],
					"text" : "gen~ @title ampMod"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-11",
					"local" : 1,
					"maxclass" : "ezdac~",
					"numinlets" : 2,
					"numoutlets" : 0,
					"patching_rect" : [ 179.333338737487793, 611.33335155248642, 45.0, 45.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 407.333345472812653, 222.00000661611557, 31.333334267139435, 31.333334267139435 ]
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 8.0,
					"hidden" : 1,
					"id" : "obj-38",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 18.000000536441803, 26.666667342185974, 49.0, 17.0 ],
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
					"patching_rect" : [ 390.666678309440613, 133.333337962627411, 136.0, 20.0 ],
					"presentation" : 1,
					"presentation_linecount" : 2,
					"presentation_rect" : [ 100.000002980232239, 52.666668236255646, 64.0, 33.0 ],
					"text" : "FM synth values",
					"textjustification" : 1
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Verdana",
					"fontsize" : 12.0,
					"id" : "obj-30",
					"maxclass" : "preset",
					"numinlets" : 1,
					"numoutlets" : 5,
					"outlettype" : [ "preset", "int", "preset", "int", "" ],
					"patching_rect" : [ 53.5, 92.99999725818634, 100.0, 40.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 1.333333373069763, 216.666673123836517, 126.000003755092621, 42.000000596046448 ],
					"preset_data" : [ 						{
							"number" : 1,
							"data" : [ 5, "obj-63", "live.gain~", "float", -2.5, 5, "obj-52", "flonum", "float", 110.0, 5, "obj-50", "flonum", "float", 1.5, 5, "obj-2", "flonum", "float", 0.200000002980232, 5, "obj-8", "flonum", "float", 1000.0, 5, "obj-10", "flonum", "float", 3.099999904632568, 5, "obj-3", "number", "int", 0, 5, "obj-39", "flonum", "float", 8.0, 5, "obj-41", "live.tab", "float", 0.0, 5, "obj-46", "flonum", "float", 0.200000002980232, 5, "obj-53", "number", "int", 20, 5, "obj-110", "flonum", "float", 1.0, 5, "obj-68", "flonum", "float", 1.0, 5, "obj-66", "flonum", "float", 1.0, 5, "obj-54", "flonum", "float", 0.5, 5, "obj-33", "flonum", "float", 0.5, 5, "obj-43", "flonum", "float", 0.100000001490116, 5, "obj-26", "flonum", "float", 0.5, 4, "<invalid>", "function", "clear", 8, "<invalid>", "function", "add_with_curve", 0.0, 0.0, 0, 0.0, 8, "<invalid>", "function", "add_with_curve", 431.506834508979978, 0.0, 0, 0.0, 8, "<invalid>", "function", "add_with_curve", 678.082175162770227, 0.028985514081944, 0, 0.995, 8, "<invalid>", "function", "add_with_curve", 828.767105562308643, 0.260869564541346, 0, 0.0, 8, "<invalid>", "function", "add_with_curve", 952.054775889203711, 0.666666652845301, 0, 0.0, 8, "<invalid>", "function", "add_with_curve", 1000.0, 1.0, 0, 0.0, 8, "<invalid>", "function", "add_with_curve", 1000.0, 1.0, 0, 0.0, 5, "<invalid>", "function", "domain", 1000.0, 6, "<invalid>", "function", "range", 0.0, 1.0, 5, "<invalid>", "function", "mode", 1, 4, "<invalid>", "function", "clear", 8, "<invalid>", "function", "add_with_curve", 0.0, 0.0, 0, 0.0, 8, "<invalid>", "function", "add_with_curve", 89.041083600938066, 0.391304331262983, 0, 0.0, 8, "<invalid>", "function", "add_with_curve", 226.027383964154836, 0.884057938489213, 0, 0.0, 8, "<invalid>", "function", "add_with_curve", 458.904094581623326, 1.0, 0, 0.0, 8, "<invalid>", "function", "add_with_curve", 1000.0, 0.999999963718914, 0, 0.0, 5, "<invalid>", "function", "domain", 1000.0, 6, "<invalid>", "function", "range", 0.0, 1.0, 5, "<invalid>", "function", "mode", 1, 5, "obj-108", "number", "int", 1, 5, "obj-140", "toggle", "int", 1, 5, "obj-143", "number", "int", 1500, 5, "obj-149", "number", "int", 2, 5, "obj-22", "flonum", "float", 0.349999994039536 ]
						}
, 						{
							"number" : 2,
							"data" : [ 5, "obj-63", "live.gain~", "float", -2.5, 5, "obj-52", "flonum", "float", 110.0, 5, "obj-50", "flonum", "float", 1.5, 5, "obj-2", "flonum", "float", 0.349999994039536, 5, "obj-8", "flonum", "float", 1000.0, 5, "obj-10", "flonum", "float", 3.099999904632568, 5, "obj-3", "number", "int", 0, 5, "obj-39", "flonum", "float", 4.0, 5, "obj-41", "live.tab", "float", 1.0, 5, "obj-46", "flonum", "float", 0.5, 5, "obj-53", "number", "int", 20, 5, "obj-110", "flonum", "float", 1.0, 5, "obj-68", "flonum", "float", 1.0, 5, "obj-66", "flonum", "float", 1.0, 5, "obj-54", "flonum", "float", 0.5, 5, "obj-33", "flonum", "float", 0.5, 5, "obj-43", "flonum", "float", 0.100000001490116, 5, "obj-26", "flonum", "float", 0.5, 4, "<invalid>", "function", "clear", 8, "<invalid>", "function", "add_with_curve", 0.0, 0.0, 0, 0.0, 8, "<invalid>", "function", "add_with_curve", 431.506834508979978, 0.0, 0, 0.0, 8, "<invalid>", "function", "add_with_curve", 678.082175162770227, 0.028985514081944, 0, 0.995, 8, "<invalid>", "function", "add_with_curve", 828.767105562308643, 0.260869564541346, 0, 0.0, 8, "<invalid>", "function", "add_with_curve", 952.054775889203711, 0.666666652845301, 0, 0.0, 8, "<invalid>", "function", "add_with_curve", 1000.0, 1.0, 0, 0.0, 8, "<invalid>", "function", "add_with_curve", 1000.0, 1.0, 0, 0.0, 5, "<invalid>", "function", "domain", 1000.0, 6, "<invalid>", "function", "range", 0.0, 1.0, 5, "<invalid>", "function", "mode", 1, 4, "<invalid>", "function", "clear", 8, "<invalid>", "function", "add_with_curve", 0.0, 0.0, 0, 0.0, 8, "<invalid>", "function", "add_with_curve", 89.041083600938066, 0.391304331262983, 0, 0.0, 8, "<invalid>", "function", "add_with_curve", 226.027383964154836, 0.884057938489213, 0, 0.0, 8, "<invalid>", "function", "add_with_curve", 458.904094581623326, 1.0, 0, 0.0, 8, "<invalid>", "function", "add_with_curve", 1000.0, 0.999999963718914, 0, 0.0, 5, "<invalid>", "function", "domain", 1000.0, 6, "<invalid>", "function", "range", 0.0, 1.0, 5, "<invalid>", "function", "mode", 1, 5, "obj-108", "number", "int", 1, 5, "obj-140", "toggle", "int", 1, 5, "obj-143", "number", "int", 1500, 5, "obj-149", "number", "int", 8, 5, "obj-22", "flonum", "float", 0.430000007152557 ]
						}
, 						{
							"number" : 3,
							"data" : [ 5, "obj-63", "live.gain~", "float", -2.5, 5, "obj-52", "flonum", "float", 110.0, 5, "obj-50", "flonum", "float", 1.5, 5, "obj-2", "flonum", "float", 0.400000005960464, 5, "obj-8", "flonum", "float", 1000.0, 5, "obj-10", "flonum", "float", 6.900000095367432, 5, "obj-3", "number", "int", 0, 5, "obj-39", "flonum", "float", 8.0, 5, "obj-41", "live.tab", "float", 1.0, 5, "obj-46", "flonum", "float", 0.200000002980232, 5, "obj-53", "number", "int", 20, 5, "obj-110", "flonum", "float", 0.419999986886978, 5, "obj-68", "flonum", "float", 2.900000095367432, 5, "obj-66", "flonum", "float", 1.0, 5, "obj-54", "flonum", "float", 0.400000005960464, 5, "obj-33", "flonum", "float", 0.400000005960464, 5, "obj-43", "flonum", "float", 0.400000005960464, 5, "obj-26", "flonum", "float", 0.5, 4, "<invalid>", "function", "clear", 8, "<invalid>", "function", "add_with_curve", 0.0, 0.0, 0, 0.0, 8, "<invalid>", "function", "add_with_curve", 431.506834508979978, 0.0, 0, 0.0, 8, "<invalid>", "function", "add_with_curve", 678.082175162770227, 0.028985514081944, 0, 0.995, 8, "<invalid>", "function", "add_with_curve", 828.767105562308643, 0.260869564541346, 0, 0.0, 8, "<invalid>", "function", "add_with_curve", 952.054775889203711, 0.666666652845301, 0, 0.0, 8, "<invalid>", "function", "add_with_curve", 1000.0, 1.0, 0, 0.0, 8, "<invalid>", "function", "add_with_curve", 1000.0, 1.0, 0, 0.0, 5, "<invalid>", "function", "domain", 1000.0, 6, "<invalid>", "function", "range", 0.0, 1.0, 5, "<invalid>", "function", "mode", 1, 4, "<invalid>", "function", "clear", 8, "<invalid>", "function", "add_with_curve", 0.0, 0.0, 0, 0.0, 8, "<invalid>", "function", "add_with_curve", 89.041083600938066, 0.391304331262983, 0, 0.0, 8, "<invalid>", "function", "add_with_curve", 226.027383964154836, 0.884057938489213, 0, 0.0, 8, "<invalid>", "function", "add_with_curve", 458.904094581623326, 1.0, 0, 0.0, 8, "<invalid>", "function", "add_with_curve", 1000.0, 0.999999963718914, 0, 0.0, 5, "<invalid>", "function", "domain", 1000.0, 6, "<invalid>", "function", "range", 0.0, 1.0, 5, "<invalid>", "function", "mode", 1, 5, "obj-108", "number", "int", 1, 5, "obj-140", "toggle", "int", 1, 5, "obj-143", "number", "int", 1500, 5, "obj-149", "number", "int", 20, 5, "obj-22", "flonum", "float", 0.430000007152557 ]
						}
, 						{
							"number" : 4,
							"data" : [ 5, "obj-63", "live.gain~", "float", -2.5, 5, "obj-52", "flonum", "float", 110.0, 5, "obj-50", "flonum", "float", 2.609999895095825, 5, "obj-2", "flonum", "float", 0.200000002980232, 5, "obj-8", "flonum", "float", 1000.0, 5, "obj-10", "flonum", "float", 12.0, 5, "obj-3", "number", "int", 0, 5, "obj-39", "flonum", "float", 8.0, 5, "obj-41", "live.tab", "float", 1.0, 5, "obj-46", "flonum", "float", 0.810000002384186, 5, "obj-53", "number", "int", 50, 5, "obj-110", "flonum", "float", 0.419999986886978, 5, "obj-68", "flonum", "float", 2.900000095367432, 5, "obj-66", "flonum", "float", 1.0, 5, "obj-54", "flonum", "float", 0.400000005960464, 5, "obj-33", "flonum", "float", 0.400000005960464, 5, "obj-43", "flonum", "float", 0.400000005960464, 5, "obj-26", "flonum", "float", 0.5, 4, "<invalid>", "function", "clear", 8, "<invalid>", "function", "add_with_curve", 0.0, 0.0, 0, 0.0, 8, "<invalid>", "function", "add_with_curve", 431.506834508979978, 0.0, 0, 0.0, 8, "<invalid>", "function", "add_with_curve", 678.082175162770227, 0.028985514081944, 0, 0.995, 8, "<invalid>", "function", "add_with_curve", 828.767105562308643, 0.260869564541346, 0, 0.0, 8, "<invalid>", "function", "add_with_curve", 952.054775889203711, 0.666666652845301, 0, 0.0, 8, "<invalid>", "function", "add_with_curve", 1000.0, 1.0, 0, 0.0, 8, "<invalid>", "function", "add_with_curve", 1000.0, 1.0, 0, 0.0, 5, "<invalid>", "function", "domain", 1000.0, 6, "<invalid>", "function", "range", 0.0, 1.0, 5, "<invalid>", "function", "mode", 1, 4, "<invalid>", "function", "clear", 8, "<invalid>", "function", "add_with_curve", 0.0, 0.0, 0, 0.0, 8, "<invalid>", "function", "add_with_curve", 89.041083600938066, 0.391304331262983, 0, 0.0, 8, "<invalid>", "function", "add_with_curve", 226.027383964154836, 0.884057938489213, 0, 0.0, 8, "<invalid>", "function", "add_with_curve", 458.904094581623326, 1.0, 0, 0.0, 8, "<invalid>", "function", "add_with_curve", 1000.0, 0.999999963718914, 0, 0.0, 5, "<invalid>", "function", "domain", 1000.0, 6, "<invalid>", "function", "range", 0.0, 1.0, 5, "<invalid>", "function", "mode", 1, 5, "obj-108", "number", "int", 1, 5, "obj-140", "toggle", "int", 1, 5, "obj-143", "number", "int", 1500, 5, "obj-149", "number", "int", 8, 5, "obj-22", "flonum", "float", 0.600000023841858 ]
						}
, 						{
							"number" : 5,
							"data" : [ 5, "obj-63", "live.gain~", "float", -2.5, 5, "obj-52", "flonum", "float", 220.0, 5, "obj-50", "flonum", "float", 0.509999990463257, 5, "obj-2", "flonum", "float", 0.300000011920929, 5, "obj-8", "flonum", "float", 500.0, 5, "obj-10", "flonum", "float", 17.0, 5, "obj-3", "number", "int", 500, 5, "obj-39", "flonum", "float", 12.0, 5, "obj-41", "live.tab", "float", 1.0, 5, "obj-46", "flonum", "float", 0.129999995231628, 5, "obj-53", "number", "int", 100, 5, "obj-110", "flonum", "float", 0.419999986886978, 5, "obj-68", "flonum", "float", 2.900000095367432, 5, "obj-66", "flonum", "float", 1.0, 5, "obj-54", "flonum", "float", 0.400000005960464, 5, "obj-33", "flonum", "float", 0.400000005960464, 5, "obj-43", "flonum", "float", 0.400000005960464, 5, "obj-26", "flonum", "float", 0.5, 4, "<invalid>", "function", "clear", 8, "<invalid>", "function", "add_with_curve", 0.0, 0.0, 0, 0.0, 8, "<invalid>", "function", "add_with_curve", 431.506834508979978, 0.0, 0, 0.0, 8, "<invalid>", "function", "add_with_curve", 678.082175162770227, 0.028985514081944, 0, 0.995, 8, "<invalid>", "function", "add_with_curve", 828.767105562308643, 0.260869564541346, 0, 0.0, 8, "<invalid>", "function", "add_with_curve", 952.054775889203711, 0.666666652845301, 0, 0.0, 8, "<invalid>", "function", "add_with_curve", 1000.0, 1.0, 0, 0.0, 8, "<invalid>", "function", "add_with_curve", 1000.0, 1.0, 0, 0.0, 5, "<invalid>", "function", "domain", 1000.0, 6, "<invalid>", "function", "range", 0.0, 1.0, 5, "<invalid>", "function", "mode", 1, 4, "<invalid>", "function", "clear", 8, "<invalid>", "function", "add_with_curve", 0.0, 0.0, 0, 0.0, 8, "<invalid>", "function", "add_with_curve", 89.041083600938066, 0.391304331262983, 0, 0.0, 8, "<invalid>", "function", "add_with_curve", 226.027383964154836, 0.884057938489213, 0, 0.0, 8, "<invalid>", "function", "add_with_curve", 458.904094581623326, 1.0, 0, 0.0, 8, "<invalid>", "function", "add_with_curve", 1000.0, 0.999999963718914, 0, 0.0, 5, "<invalid>", "function", "domain", 1000.0, 6, "<invalid>", "function", "range", 0.0, 1.0, 5, "<invalid>", "function", "mode", 1, 5, "obj-108", "number", "int", 1, 5, "obj-140", "toggle", "int", 1, 5, "obj-143", "number", "int", 1250, 5, "obj-149", "number", "int", 8, 5, "obj-22", "flonum", "float", 0.600000023841858 ]
						}
, 						{
							"number" : 6,
							"data" : [ 5, "obj-63", "live.gain~", "float", -2.5, 5, "obj-52", "flonum", "float", 110.0, 5, "obj-50", "flonum", "float", 2.490000009536743, 5, "obj-2", "flonum", "float", 0.319999992847443, 5, "obj-8", "flonum", "float", 6000.0, 5, "obj-10", "flonum", "float", 7.199999809265137, 5, "obj-3", "number", "int", 0, 5, "obj-39", "flonum", "float", 1.0, 5, "obj-41", "live.tab", "float", 0.0, 5, "obj-46", "flonum", "float", 0.5, 5, "obj-53", "number", "int", 250, 5, "obj-110", "flonum", "float", 0.5, 5, "obj-68", "flonum", "float", 2.900000095367432, 5, "obj-66", "flonum", "float", 1.0, 5, "obj-54", "flonum", "float", 0.800000011920929, 5, "obj-33", "flonum", "float", 0.200000002980232, 5, "obj-43", "flonum", "float", 0.400000005960464, 5, "obj-26", "flonum", "float", 0.5, 4, "<invalid>", "function", "clear", 8, "<invalid>", "function", "add_with_curve", 0.0, 0.0, 0, 0.0, 8, "<invalid>", "function", "add_with_curve", 431.506834508979978, 0.0, 0, 0.0, 8, "<invalid>", "function", "add_with_curve", 678.082175162770227, 0.028985514081944, 0, 0.995, 8, "<invalid>", "function", "add_with_curve", 828.767105562308643, 0.260869564541346, 0, 0.0, 8, "<invalid>", "function", "add_with_curve", 952.054775889203711, 0.666666652845301, 0, 0.0, 8, "<invalid>", "function", "add_with_curve", 1000.0, 1.0, 0, 0.0, 8, "<invalid>", "function", "add_with_curve", 1000.0, 1.0, 0, 0.0, 5, "<invalid>", "function", "domain", 1000.0, 6, "<invalid>", "function", "range", 0.0, 1.0, 5, "<invalid>", "function", "mode", 1, 4, "<invalid>", "function", "clear", 8, "<invalid>", "function", "add_with_curve", 0.0, 0.0, 0, 0.0, 8, "<invalid>", "function", "add_with_curve", 89.041083600938066, 0.391304331262983, 0, 0.0, 8, "<invalid>", "function", "add_with_curve", 226.027383964154836, 0.884057938489213, 0, 0.0, 8, "<invalid>", "function", "add_with_curve", 458.904094581623326, 1.0, 0, 0.0, 8, "<invalid>", "function", "add_with_curve", 1000.0, 0.999999963718914, 0, 0.0, 5, "<invalid>", "function", "domain", 1000.0, 6, "<invalid>", "function", "range", 0.0, 1.0, 5, "<invalid>", "function", "mode", 1, 5, "obj-108", "number", "int", 1, 5, "obj-140", "toggle", "int", 1, 5, "obj-143", "number", "int", 9000, 5, "obj-149", "number", "int", 2, 5, "obj-22", "flonum", "float", 0.600000023841858 ]
						}
, 						{
							"number" : 7,
							"data" : [ 5, "obj-63", "live.gain~", "float", -2.5, 5, "obj-52", "flonum", "float", 110.0, 5, "obj-50", "flonum", "float", 2.049999952316284, 5, "obj-2", "flonum", "float", 0.200000002980232, 5, "obj-8", "number", "int", 1000, 5, "obj-10", "flonum", "float", 8.0, 5, "<invalid>", "attrui", "attr", "file", 4, "<invalid>", "attrui", "/Users/one/dev/sound/chuck-max/examples/max/fmenv.ck", 5, "obj-3", "number", "int", 0, 5, "<invalid>", "flonum", "float", 0.200000002980232, 5, "obj-39", "flonum", "float", 4.0, 5, "obj-41", "live.tab", "float", 1.0, 5, "obj-46", "flonum", "float", 0.5, 5, "obj-53", "number", "int", 120 ]
						}
, 						{
							"number" : 9,
							"data" : [ 5, "obj-63", "live.gain~", "float", -2.5, 5, "obj-52", "flonum", "float", 440.0, 5, "obj-50", "flonum", "float", 2.0, 5, "obj-2", "flonum", "float", 0.200000002980232, 5, "obj-8", "number", "int", 250, 5, "obj-10", "flonum", "float", 4.0, 5, "obj-3", "number", "int", 500, 5, "<invalid>", "flonum", "float", 0.300000011920929, 5, "obj-39", "flonum", "float", 4.0, 5, "obj-41", "live.tab", "float", 0.0, 5, "obj-46", "flonum", "float", 0.5, 5, "obj-53", "number", "int", 0 ]
						}
, 						{
							"number" : 10,
							"data" : [ 5, "obj-63", "live.gain~", "float", -2.5, 5, "obj-52", "flonum", "float", 440.0, 5, "obj-50", "flonum", "float", 2.150000095367432, 5, "obj-2", "flonum", "float", 0.400000005960464, 5, "obj-8", "number", "int", 250, 5, "obj-10", "flonum", "float", 4.0, 5, "obj-3", "number", "int", 500, 5, "<invalid>", "flonum", "float", 0.300000011920929, 5, "obj-39", "flonum", "float", 4.0, 5, "obj-41", "live.tab", "float", 1.0, 5, "obj-46", "flonum", "float", 0.5, 5, "obj-53", "number", "int", 100 ]
						}
, 						{
							"number" : 11,
							"data" : [ 5, "obj-63", "live.gain~", "float", -2.5, 5, "obj-52", "flonum", "float", 440.0, 5, "obj-50", "flonum", "float", 2.150000095367432, 5, "obj-2", "flonum", "float", 0.400000005960464, 5, "obj-8", "number", "int", 250, 5, "obj-10", "flonum", "float", 6.300000190734863, 5, "obj-3", "number", "int", 500, 5, "<invalid>", "flonum", "float", 0.300000011920929, 5, "obj-39", "flonum", "float", 18.0, 5, "obj-41", "live.tab", "float", 1.0, 5, "obj-46", "flonum", "float", 0.0, 5, "obj-53", "number", "int", 100 ]
						}
, 						{
							"number" : 12,
							"data" : [ 5, "obj-63", "live.gain~", "float", -2.5, 5, "obj-52", "flonum", "float", 110.0, 5, "obj-50", "flonum", "float", 3.309999942779541, 5, "obj-2", "flonum", "float", 0.200000002980232, 5, "obj-8", "number", "int", 2000, 5, "obj-10", "flonum", "float", 6.5, 5, "obj-3", "number", "int", 0, 5, "<invalid>", "flonum", "float", 0.400000005960464, 5, "obj-39", "flonum", "float", 16.0, 5, "obj-41", "live.tab", "float", 1.0, 5, "obj-46", "flonum", "float", 0.200000002980232, 5, "obj-53", "number", "int", 100 ]
						}
, 						{
							"number" : 13,
							"data" : [ 5, "obj-63", "live.gain~", "float", -2.5, 5, "obj-52", "flonum", "float", 220.0, 5, "obj-50", "flonum", "float", 0.800000011920929, 5, "obj-2", "flonum", "float", 0.270000010728836, 5, "obj-8", "number", "int", 1000, 5, "obj-10", "flonum", "float", 4.0, 5, "obj-3", "number", "int", 0, 5, "<invalid>", "flonum", "float", 0.200000002980232, 5, "obj-39", "flonum", "float", 0.5, 5, "obj-41", "live.tab", "float", 1.0, 5, "obj-46", "flonum", "float", 0.200000002980232, 5, "obj-53", "number", "int", 200 ]
						}
, 						{
							"number" : 14,
							"data" : [ 5, "obj-63", "live.gain~", "float", -2.5, 5, "obj-52", "flonum", "float", 400.0, 5, "obj-50", "flonum", "float", 2.339999914169312, 5, "obj-2", "flonum", "float", 0.200000002980232, 5, "obj-8", "number", "int", 100, 5, "obj-10", "flonum", "float", 5.880000114440918, 5, "obj-3", "number", "int", 0, 5, "<invalid>", "flonum", "float", 0.200000002980232, 5, "obj-39", "flonum", "float", 0.5, 5, "obj-41", "live.tab", "float", 1.0, 5, "obj-46", "flonum", "float", 0.270000010728836, 5, "obj-53", "number", "int", 100 ]
						}
, 						{
							"number" : 15,
							"data" : [ 5, "obj-63", "live.gain~", "float", -2.5, 5, "obj-52", "flonum", "float", 400.0, 5, "obj-50", "flonum", "float", 2.410000085830688, 5, "obj-2", "flonum", "float", 0.28999999165535, 5, "obj-8", "number", "int", 10, 5, "obj-10", "flonum", "float", 7.659999847412109, 5, "obj-3", "number", "int", 10, 5, "<invalid>", "flonum", "float", 0.200000002980232, 5, "obj-39", "flonum", "float", 7.0, 5, "obj-41", "live.tab", "float", 1.0, 5, "obj-46", "flonum", "float", 0.730000019073486, 5, "obj-53", "number", "int", 500 ]
						}
, 						{
							"number" : 16,
							"data" : [ 5, "obj-63", "live.gain~", "float", -2.5, 5, "obj-52", "flonum", "float", 200.0, 5, "obj-50", "flonum", "float", 2.299999952316284, 5, "obj-2", "flonum", "float", 0.28999999165535, 5, "obj-8", "number", "int", 500, 5, "obj-10", "flonum", "float", 6.900000095367432, 5, "obj-3", "number", "int", 20, 5, "<invalid>", "flonum", "float", 0.200000002980232, 5, "obj-39", "flonum", "float", 7.0, 5, "obj-41", "live.tab", "float", 0.0, 5, "obj-46", "flonum", "float", 0.730000019073486, 5, "obj-53", "number", "int", 500 ]
						}
, 						{
							"number" : 17,
							"data" : [ 5, "obj-63", "live.gain~", "float", -2.5, 5, "obj-52", "flonum", "float", 55.0, 5, "obj-50", "flonum", "float", 3.0, 5, "obj-2", "flonum", "float", 0.5, 5, "obj-8", "number", "int", 250, 5, "obj-10", "flonum", "float", 6.5, 5, "obj-3", "number", "int", 250, 5, "<invalid>", "flonum", "float", 0.300000011920929, 5, "obj-39", "flonum", "float", 4.0, 5, "obj-41", "live.tab", "float", 0.0, 5, "obj-46", "flonum", "float", 0.439999997615814, 5, "obj-53", "number", "int", 250 ]
						}
, 						{
							"number" : 18,
							"data" : [ 5, "obj-63", "live.gain~", "float", -2.5, 5, "obj-52", "flonum", "float", 110.0, 5, "obj-50", "flonum", "float", 3.0, 5, "obj-2", "flonum", "float", 0.200000002980232, 5, "obj-8", "number", "int", 250, 5, "obj-10", "flonum", "float", 14.0, 5, "obj-3", "number", "int", 500, 5, "<invalid>", "flonum", "float", 0.300000011920929, 5, "obj-39", "flonum", "float", 1.0, 5, "obj-41", "live.tab", "float", 0.0, 5, "obj-46", "flonum", "float", 0.5, 5, "obj-53", "number", "int", 250 ]
						}
, 						{
							"number" : 19,
							"data" : [ 5, "obj-63", "live.gain~", "float", -2.5, 5, "obj-52", "flonum", "float", 110.0, 5, "obj-50", "flonum", "float", 3.160000085830688, 5, "obj-2", "flonum", "float", 0.200000002980232, 5, "obj-8", "number", "int", 250, 5, "obj-10", "flonum", "float", 6.5, 5, "obj-3", "number", "int", 250, 5, "<invalid>", "flonum", "float", 0.300000011920929, 5, "obj-39", "flonum", "float", 4.0, 5, "obj-41", "live.tab", "float", 1.0, 5, "obj-46", "flonum", "float", 0.5, 5, "obj-53", "number", "int", 100 ]
						}
 ]
				}

			}
, 			{
				"box" : 				{
					"fontface" : 0,
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-23",
					"linecount" : 2,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 180.000004768371582, 170.666671752929688, 48.0, 33.0 ],
					"presentation" : 1,
					"presentation_linecount" : 2,
					"presentation_rect" : [ 44.000001311302185, 127.333337128162384, 41.0, 33.0 ],
					"text" : "load /\nrun",
					"textjustification" : 1
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-20",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 725.000021874904633, 157.000005006790161, 78.0, 20.0 ],
					"presentation" : 1,
					"presentation_linecount" : 2,
					"presentation_rect" : [ 445.333346605300903, 34.666667699813843, 51.333334863185883, 33.0 ],
					"text" : "gap dur ms",
					"textjustification" : 1
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-16",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 630.333352386951447, 157.000005006790161, 69.0, 20.0 ],
					"presentation" : 1,
					"presentation_linecount" : 2,
					"presentation_rect" : [ 383.333344757556915, 34.666667699813843, 59.500002682209015, 33.0 ],
					"text" : "env dur ms",
					"textjustification" : 1
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Verdana",
					"fontsize" : 12.0,
					"id" : "obj-15",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 463.000014305114746, 156.33333832025528, 76.0, 21.0 ],
					"presentation" : 1,
					"presentation_linecount" : 2,
					"presentation_rect" : [ 316.666676104068756, 34.000001013278961, 55.333335041999817, 36.0 ],
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
					"patching_rect" : [ 717.000021636486053, 176.333338916301727, 52.0, 22.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 444.666679918766022, 66.000001966953278, 52.0, 22.0 ]
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 10.0,
					"id" : "obj-7",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 711.000021457672119, 201.666673004627228, 74.0, 20.0 ],
					"text" : "gapTimeMs $1"
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
					"patching_rect" : [ 155.333337962627411, 170.666671752929688, 24.0, 24.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 12.000000357627869, 130.00000387430191, 28.500000715255737, 28.500000715255737 ]
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 10.0,
					"id" : "obj-9",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 465.000014364719391, 201.666673004627228, 66.0, 20.0 ],
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
					"patching_rect" : [ 465.000014364719391, 176.333338916301727, 51.0, 22.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 319.333342850208282, 66.666668653488159, 50.0, 22.0 ]
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"format" : 6,
					"id" : "obj-8",
					"maxclass" : "flonum",
					"maximum" : 20000.0,
					"minimum" : 1.0,
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "bang" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 626.333352267742157, 176.333338916301727, 58.0, 22.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 382.000011384487152, 66.000001966953278, 61.0, 22.0 ]
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 10.0,
					"id" : "obj-4",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 626.333352267742157, 201.666673004627228, 68.0, 20.0 ],
					"text" : "envDurMs $1"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Verdana",
					"fontsize" : 12.0,
					"id" : "obj-152",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 408.666679263114929, 156.33333832025528, 47.0, 21.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 268.0000079870224, 49.333334803581238, 47.0, 21.0 ],
					"text" : "harm",
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
					"patching_rect" : [ 269.666674315929413, 156.33333832025528, 42.0, 20.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 165.333338260650635, 50.000001490116119, 42.0, 20.0 ],
					"text" : "gain",
					"textjustification" : 1
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-150",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 333.66667628288269, 156.33333832025528, 44.0, 20.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 218.000006496906281, 50.000001490116119, 37.0, 20.0 ],
					"text" : "freq",
					"textjustification" : 1
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 10.0,
					"id" : "obj-6",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 256.666674315929413, 201.666673004627228, 57.0, 20.0 ],
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
					"patching_rect" : [ 256.666674315929413, 176.333338916301727, 43.0, 22.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 165.333338260650635, 66.666668653488159, 42.0, 22.0 ]
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 10.0,
					"id" : "obj-49",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 400.166679263114929, 201.000006318092346, 45.0, 20.0 ],
					"text" : "harm $1"
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
					"patching_rect" : [ 400.166679263114929, 175.666672229766846, 53.0, 22.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 265.333341240882874, 66.666668653488159, 52.0, 22.0 ]
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
					"patching_rect" : [ 322.66667628288269, 176.333338916301727, 55.0, 22.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 209.33333957195282, 66.666668653488159, 54.0, 22.0 ]
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
					"presentation" : 1,
					"presentation_rect" : [ 80.666669070720673, 162.000004827976227, 35.0, 22.0 ],
					"text" : "reset"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 10.0,
					"id" : "obj-21",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 322.66667628288269, 201.000006318092346, 57.0, 20.0 ],
					"text" : "carFreq $1"
				}

			}
, 			{
				"box" : 				{
					"fontface" : 1,
					"fontname" : "Arial",
					"fontsize" : 14.0,
					"id" : "obj-35",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 966.666695475578308, 52.666672021150589, 204.0, 22.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 186.666672229766846, 10.666667699813843, 203.0, 22.0 ],
					"text" : "FM envelopes w Amp Mod",
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
					"patching_rect" : [ 170.000005066394806, 265.333341240882874, 28.0, 22.0 ],
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
					"patching_rect" : [ 429.333346128463745, 235.666674017906189, 30.0, 22.0 ],
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
					"patching_rect" : [ 107.333336532115936, 144.000004291534424, 41.0, 22.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 77.333335638046265, 188.66667228937149, 41.0, 22.0 ],
					"text" : "status"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 8.0,
					"id" : "obj-167",
					"linecount" : 2,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 37.333334445953369, 184.0, 40.0, 24.0 ],
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
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-42",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 80.000002384185791, 260.000007748603821, 64.0, 22.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 8.666666924953461, 162.000004827976227, 64.0, 22.0 ],
					"text" : "remove all"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 10.0,
					"id" : "obj-44",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 77.500002384185791, 236.666674017906189, 60.0, 20.0 ],
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
					"patching_rect" : [ 179.333338737487793, 582.00001734495163, 136.0, 22.0 ],
					"text" : "limi~ 2 @threshold -0.5."
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Verdana",
					"fontsize" : 9.0,
					"id" : "obj-63",
					"lastchannelcount" : 0,
					"maxclass" : "live.gain~",
					"numinlets" : 2,
					"numoutlets" : 5,
					"orientation" : 1,
					"outlettype" : [ "signal", "signal", "", "float", "list" ],
					"parameter_enable" : 1,
					"patching_rect" : [ 178.666672050952911, 517.333348751068115, 132.0, 46.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 186.666672229766846, 220.000006556510925, 131.0, 46.0 ],
					"saved_attribute_attributes" : 					{
						"valueof" : 						{
							"parameter_longname" : "live.gain~[1]",
							"parameter_mmax" : 6.0,
							"parameter_mmin" : -70.0,
							"parameter_modmode" : 3,
							"parameter_shortname" : "gain",
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
					"patching_rect" : [ 170.000005066394806, 298.000008881092072, 142.0, 22.0 ],
					"text" : "chuck~ 2 fmenv-event.ck",
					"varname" : "chuck-0"
				}

			}
, 			{
				"box" : 				{
					"angle" : 270.0,
					"background" : 1,
					"bgcolor" : [ 0.566355109214783, 0.47860187292099, 0.569514155387878, 1.0 ],
					"id" : "obj-135",
					"maxclass" : "panel",
					"mode" : 0,
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 180.666672050952911, 666.00001984834671, 128.0, 128.0 ],
					"presentation" : 1,
					"presentation_rect" : [ -2.833334118127823, 2.666666448116302, 507.333348453044891, 267.333341300487518 ],
					"proportion" : 0.5
				}

			}
 ],
		"lines" : [ 			{
				"patchline" : 				{
					"destination" : [ "obj-13", 1 ],
					"midpoints" : [ 302.500005066394806, 325.333343029022217, 207.03334058523177, 325.333343029022217 ],
					"source" : [ "obj-1", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-13", 0 ],
					"midpoints" : [ 179.500005066394806, 327.333329617977142, 179.500006318092346, 327.333329617977142 ],
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
					"destination" : [ "obj-30", 0 ],
					"midpoints" : [ 27.500000536441803, 84.000002503395081, 63.0, 84.000002503395081 ],
					"source" : [ "obj-108", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-111", 0 ],
					"midpoints" : [ 717.50002110004425, 341.333324372768402, 717.50002110004425, 341.333324372768402 ],
					"source" : [ "obj-110", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-70", 0 ],
					"midpoints" : [ 717.50002110004425, 369.333344340324402, 633.333352208137512, 369.333344340324402, 634.166685283184052, 381.333344697952271 ],
					"source" : [ "obj-111", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-66", 0 ],
					"midpoints" : [ 903.166694194078445, 316.666676104068756, 855.50002521276474, 316.666676104068756 ],
					"source" : [ "obj-116", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-31", 0 ],
					"source" : [ "obj-12", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-63", 1 ],
					"midpoints" : [ 248.333341985940933, 488.000014543533325, 301.166672050952911, 488.000014543533325 ],
					"source" : [ "obj-13", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-63", 0 ],
					"midpoints" : [ 179.500006318092346, 489.333347916603088, 188.166672050952911, 489.333347916603088 ],
					"source" : [ "obj-13", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-82", 0 ],
					"source" : [ "obj-140", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-74", 0 ],
					"source" : [ "obj-141", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-144", 0 ],
					"source" : [ "obj-142", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-82", 1 ],
					"source" : [ "obj-143", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"midpoints" : [ 220.833339631557465, 292.000008702278137, 179.500005066394806, 292.000008702278137 ],
					"source" : [ "obj-144", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-153", 0 ],
					"source" : [ "obj-149", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-70", 0 ],
					"midpoints" : [ 729.500021457672119, 443.333346545696259, 708.00002110004425, 443.333346545696259, 708.00002110004425, 368.66667765378952, 634.166685283184052, 368.66667765378952 ],
					"source" : [ "obj-153", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-149", 0 ],
					"source" : [ "obj-157", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-143", 0 ],
					"source" : [ "obj-158", 0 ]
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
					"destination" : [ "obj-28", 0 ],
					"source" : [ "obj-19", 0 ]
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
					"midpoints" : [ 332.16667628288269, 230.000000834465027, 438.833346128463745, 230.000000834465027 ],
					"source" : [ "obj-21", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-25", 0 ],
					"source" : [ "obj-22", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-11", 0 ],
					"midpoints" : [ 264.166674315929413, 636.733343899250031, 232.266667097806931, 636.733343899250031, 232.266667097806931, 606.733343899250031, 188.833338737487793, 606.733343899250031 ],
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-17", 0 ],
					"midpoints" : [ 548.833349406719208, 230.000006854534149, 438.833346128463745, 230.000006854534149 ],
					"source" : [ "obj-25", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-158", 1 ],
					"midpoints" : [ 634.166685283184052, 408.000012159347534, 568.166682422161102, 408.000012159347534 ],
					"order" : 1,
					"source" : [ "obj-26", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-47", 0 ],
					"midpoints" : [ 634.166685283184052, 341.999993562698364, 634.166685283184052, 341.999993562698364 ],
					"order" : 0,
					"source" : [ "obj-26", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-17", 0 ],
					"midpoints" : [ 187.500005304813385, 161.000012278556824, 239.333352148532867, 161.000012278556824, 239.333352148532867, 229.666680991649628, 438.833346128463745, 229.666680991649628 ],
					"source" : [ "obj-28", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-158", 2 ],
					"midpoints" : [ 726.500021636486053, 408.666678845882416, 598.166682422161102, 408.666678845882416 ],
					"order" : 1,
					"source" : [ "obj-3", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-7", 0 ],
					"order" : 0,
					"source" : [ "obj-3", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-140", 0 ],
					"midpoints" : [ 103.5, 139.333337485790253, 392.666678369045258, 139.333337485790253, 392.666678369045258, 443.333346545696259, 498.166681230068207, 443.333346545696259 ],
					"order" : 0,
					"source" : [ "obj-30", 2 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-141", 0 ],
					"midpoints" : [ 103.5, 140.000004172325134, 63.333335220813751, 140.000004172325134, 63.333335220813751, 292.666675388813019, 89.500002384185791, 292.666675388813019 ],
					"order" : 3,
					"source" : [ "obj-30", 2 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-19", 0 ],
					"midpoints" : [ 103.5, 140.000004172325134, 166.000004947185516, 140.000004172325134, 166.000004947185516, 101.166669487953186, 187.500005304813385, 101.166669487953186 ],
					"order" : 1,
					"source" : [ "obj-30", 2 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-85", 0 ],
					"hidden" : 1,
					"midpoints" : [ 103.5, 140.000004172325134, 141.333337545394897, 140.000004172325134, 141.333337545394897, 176.666671931743622, 90.5, 176.666671931743622 ],
					"order" : 2,
					"source" : [ "obj-30", 2 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"midpoints" : [ 164.833337962627411, 292.000008702278137, 179.500005066394806, 292.000008702278137 ],
					"source" : [ "obj-31", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-108", 0 ],
					"midpoints" : [ 175.333337962627411, 230.666673541069031, 14.000000417232513, 230.666673541069031, 14.000000417232513, 48.666668117046356, 27.500000536441803, 48.666668117046356 ],
					"source" : [ "obj-31", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-70", 0 ],
					"midpoints" : [ 562.833349823951721, 368.66667765378952, 634.166685283184052, 368.66667765378952 ],
					"source" : [ "obj-32", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-40", 0 ],
					"midpoints" : [ 485.500014185905457, 341.999993562698364, 485.500014185905457, 341.999993562698364 ],
					"source" : [ "obj-33", 0 ]
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
					"destination" : [ "obj-108", 0 ],
					"midpoints" : [ 27.500000536441803, 49.333334803581238, 27.500000536441803, 49.333334803581238 ],
					"source" : [ "obj-38", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-13", 3 ],
					"midpoints" : [ 307.500010132789612, 443.000025480985641, 262.100009119510673, 443.000025480985641 ],
					"source" : [ "obj-39", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-17", 0 ],
					"midpoints" : [ 635.833352267742157, 230.666667520999908, 438.833346128463745, 230.666667520999908 ],
					"source" : [ "obj-4", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-70", 0 ],
					"midpoints" : [ 485.500014185905457, 368.000010967254639, 634.166685283184052, 368.000010967254639 ],
					"source" : [ "obj-40", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-13", 2 ],
					"midpoints" : [ 217.500007450580597, 442.33335879445076, 234.566674852371222, 442.33335879445076 ],
					"source" : [ "obj-41", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"hidden" : 1,
					"midpoints" : [ 89.500002384185791, 292.6666619181633, 179.500005066394806, 292.6666619181633 ],
					"source" : [ "obj-42", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-32", 0 ],
					"midpoints" : [ 562.833349823951721, 341.999993562698364, 562.833349823951721, 341.999993562698364 ],
					"source" : [ "obj-43", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"hidden" : 1,
					"midpoints" : [ 87.000002384185791, 259.33332759141922, 179.500005066394806, 259.33332759141922 ],
					"source" : [ "obj-44", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-13", 4 ],
					"midpoints" : [ 352.166678130626678, 443.666692167520523, 289.633343386650097, 443.666692167520523 ],
					"source" : [ "obj-46", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-70", 0 ],
					"midpoints" : [ 634.166685283184052, 368.000010967254639, 634.166685283184052, 368.000010967254639 ],
					"source" : [ "obj-47", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-17", 0 ],
					"midpoints" : [ 409.666679263114929, 229.333334147930145, 438.833346128463745, 229.333334147930145 ],
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
					"midpoints" : [ 401.500012934207916, 443.000025480985641, 317.16667765378952, 443.000025480985641 ],
					"source" : [ "obj-53", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-56", 0 ],
					"midpoints" : [ 410.833345293998718, 341.999993562698364, 410.833345293998718, 341.999993562698364 ],
					"source" : [ "obj-54", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-70", 0 ],
					"midpoints" : [ 410.833345293998718, 368.000010967254639, 634.166685283184052, 368.000010967254639 ],
					"source" : [ "obj-56", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-17", 0 ],
					"midpoints" : [ 266.166674315929413, 230.666667520999908, 438.833346128463745, 230.666667520999908 ],
					"source" : [ "obj-6", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-11", 1 ],
					"midpoints" : [ 305.833338737487793, 608.5999955534935, 214.833338737487793, 608.5999955534935 ],
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
					"midpoints" : [ 216.416672050952911, 576.599996387958527, 305.833338737487793, 576.599996387958527 ],
					"source" : [ "obj-63", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-61", 0 ],
					"midpoints" : [ 188.166672050952911, 579.933329820632935, 188.833338737487793, 579.933329820632935 ],
					"source" : [ "obj-63", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-67", 0 ],
					"midpoints" : [ 855.50002521276474, 341.333324372768402, 855.50002521276474, 341.333324372768402 ],
					"source" : [ "obj-66", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-70", 0 ],
					"midpoints" : [ 855.50002521276474, 368.66667765378952, 634.166685283184052, 368.66667765378952 ],
					"source" : [ "obj-67", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-69", 0 ],
					"midpoints" : [ 786.166689813137054, 341.333324372768402, 786.166689813137054, 341.333324372768402 ],
					"source" : [ "obj-68", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-70", 0 ],
					"midpoints" : [ 786.166689813137054, 368.66667765378952, 634.166685283184052, 368.66667765378952 ],
					"source" : [ "obj-69", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-17", 0 ],
					"midpoints" : [ 720.500021457672119, 230.000000834465027, 438.833346128463745, 230.000000834465027 ],
					"source" : [ "obj-7", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-140", 0 ],
					"midpoints" : [ 469.500014364719391, 443.333346545696259, 498.166681230068207, 443.333346545696259 ],
					"source" : [ "obj-71", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-142", 0 ],
					"source" : [ "obj-72", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-141", 0 ],
					"midpoints" : [ 19.500000298023224, 293.3333420753479, 89.500002384185791, 293.3333420753479 ],
					"source" : [ "obj-73", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"midpoints" : [ 89.500002384185791, 354.000010550022125, 159.3333380818367, 354.000010550022125, 159.3333380818367, 293.3333420753479, 179.500005066394806, 293.3333420753479 ],
					"source" : [ "obj-74", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-90", 0 ],
					"source" : [ "obj-76", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-158", 0 ],
					"midpoints" : [ 635.833352267742157, 200.000005960464478, 622.666685223579407, 200.000005960464478, 622.666685223579407, 408.666678845882416, 538.166682422161102, 408.666678845882416 ],
					"order" : 1,
					"source" : [ "obj-8", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4", 0 ],
					"order" : 0,
					"source" : [ "obj-8", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-142", 0 ],
					"midpoints" : [ 498.166681230068207, 506.66668176651001, 392.666678369045258, 506.66668176651001, 392.666678369045258, 229.333340167999268, 220.833339631557465, 229.333340167999268 ],
					"source" : [ "obj-82", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"hidden" : 1,
					"midpoints" : [ 90.5, 231.333326756954193, 179.500005066394806, 231.333326756954193 ],
					"source" : [ "obj-83", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-86", 0 ],
					"source" : [ "obj-84", 0 ]
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
					"destination" : [ "obj-76", 0 ],
					"source" : [ "obj-86", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-17", 0 ],
					"midpoints" : [ 474.500014364719391, 229.333334147930145, 438.833346128463745, 229.333334147930145 ],
					"source" : [ "obj-9", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-10", 0 ],
					"midpoints" : [ 341.166674315929413, 108.000003218650818, 474.500014364719391, 108.000003218650818 ],
					"source" : [ "obj-90", 4 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-110", 0 ],
					"midpoints" : [ 414.666674315929413, 108.000003218650818, 393.333345055580139, 108.000003218650818, 393.333345055580139, 270.000008046627045, 717.50002110004425, 270.000008046627045 ],
					"source" : [ "obj-90", 11 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-2", 0 ],
					"midpoints" : [ 309.666674315929413, 109.333336591720581, 266.166674315929413, 109.333336591720581 ],
					"source" : [ "obj-90", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-26", 0 ],
					"midpoints" : [ 404.166674315929413, 108.000003218650818, 392.666678369045258, 108.000003218650818, 392.666678369045258, 270.666674733161926, 634.166685283184052, 270.666674733161926 ],
					"source" : [ "obj-90", 10 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-3", 0 ],
					"midpoints" : [ 362.166674315929413, 108.000003218650818, 726.500021636486053, 108.000003218650818 ],
					"source" : [ "obj-90", 6 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-33", 0 ],
					"midpoints" : [ 383.166674315929413, 109.333336591720581, 392.000011682510376, 109.333336591720581, 392.000011682510376, 273.333341479301453, 485.500014185905457, 273.333341479301453 ],
					"source" : [ "obj-90", 8 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-39", 0 ],
					"midpoints" : [ 456.666674315929413, 108.666669905185699, 394.666678428649902, 108.666669905185699, 394.666678428649902, 376.666677892208099, 307.500010132789612, 376.666677892208099 ],
					"source" : [ "obj-90", 15 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-41", 0 ],
					"midpoints" : [ 446.166674315929413, 107.333336532115936, 395.333345115184784, 107.333336532115936, 395.333345115184784, 376.000011205673218, 217.500007450580597, 376.000011205673218 ],
					"source" : [ "obj-90", 14 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-43", 0 ],
					"midpoints" : [ 393.666674315929413, 272.666674792766571, 562.833349823951721, 272.666674792766571 ],
					"source" : [ "obj-90", 9 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-46", 0 ],
					"midpoints" : [ 467.166674315929413, 108.000003218650818, 393.333345055580139, 108.000003218650818, 393.333345055580139, 376.666677892208099, 352.166678130626678, 376.666677892208099 ],
					"source" : [ "obj-90", 16 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-50", 0 ],
					"midpoints" : [ 330.666674315929413, 108.000003218650818, 409.666679263114929, 108.000003218650818 ],
					"source" : [ "obj-90", 3 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-52", 0 ],
					"midpoints" : [ 320.166674315929413, 109.333336591720581, 332.16667628288269, 109.333336591720581 ],
					"source" : [ "obj-90", 2 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-53", 0 ],
					"midpoints" : [ 477.666674315929413, 107.333336532115936, 394.666678428649902, 107.333336532115936, 394.666678428649902, 396.000011801719666, 401.500012934207916, 396.000011801719666 ],
					"source" : [ "obj-90", 17 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-54", 0 ],
					"midpoints" : [ 372.666674315929413, 107.333336532115936, 392.000011682510376, 107.333336532115936, 392.000011682510376, 272.000008106231689, 410.833345293998718, 272.000008106231689 ],
					"source" : [ "obj-90", 7 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-66", 0 ],
					"midpoints" : [ 435.666674315929413, 108.000003218650818, 394.666678428649902, 108.000003218650818, 394.666678428649902, 272.666674792766571, 855.50002521276474, 272.666674792766571 ],
					"source" : [ "obj-90", 13 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-68", 0 ],
					"midpoints" : [ 425.166674315929413, 108.000003218650818, 395.333345115184784, 108.000003218650818, 395.333345115184784, 268.666674673557281, 786.166689813137054, 268.666674673557281 ],
					"source" : [ "obj-90", 12 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-72", 0 ],
					"midpoints" : [ 299.166674315929413, 109.333336591720581, 264.666674554347992, 109.333336591720581, 264.666674554347992, 102.666669726371765, 220.833339631557465, 102.666669726371765 ],
					"source" : [ "obj-90", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-8", 0 ],
					"midpoints" : [ 351.666674315929413, 108.000003218650818, 635.833352267742157, 108.000003218650818 ],
					"source" : [ "obj-90", 5 ]
				}

			}
 ],
		"parameters" : 		{
			"obj-142" : [ "button", "button", 0 ],
			"obj-41" : [ "live.tab", "live.tab", 0 ],
			"obj-63" : [ "live.gain~[1]", "gain", 0 ],
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
		"parameter_map" : 		{
			"key" : 			{
				"button" : 				{
					"srcname" : "0.modifiers.12.code.key",
					"min" : 0.0,
					"max" : 1.0,
					"flags" : 1
				}

			}

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
