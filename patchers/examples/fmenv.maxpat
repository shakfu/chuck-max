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
		"rect" : [ 514.0, 120.0, 1101.0, 757.0 ],
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
					"fontsize" : 8.0,
					"id" : "obj-39",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "bang" ],
					"patching_rect" : [ 2.000000059604645, 650.666686058044434, 43.0, 17.0 ],
					"text" : "delay 100"
				}

			}
, 			{
				"box" : 				{
					"fontsize" : 8.0,
					"id" : "obj-40",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "bang" ],
					"patching_rect" : [ 2.000000059604645, 634.666685581207275, 41.0, 17.0 ],
					"text" : "loadbang"
				}

			}
, 			{
				"box" : 				{
					"fontsize" : 8.0,
					"id" : "obj-41",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 2.000000059604645, 667.333353221416473, 66.0, 17.0 ],
					"text" : "zoomfactor 1.25"
				}

			}
, 			{
				"box" : 				{
					"fontsize" : 8.0,
					"id" : "obj-43",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "" ],
					"patching_rect" : [ 2.000000059604645, 686.66668713092804, 48.0, 17.0 ],
					"save" : [ "#N", "thispatcher", ";", "#Q", "end", ";" ],
					"text" : "thispatcher"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-38",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 79.0, 58.66666841506958, 60.0, 20.0 ],
					"text" : "loadmess 1"
				}

			}
, 			{
				"box" : 				{
					"fontface" : 1,
					"fontname" : "Verdana",
					"id" : "obj-36",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 355.0, 147.0, 128.0, 19.0 ],
					"text" : "FM synthesis values",
					"textjustification" : 1
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Verdana",
					"id" : "obj-34",
					"linecount" : 6,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 495.0, 250.0, 309.0, 79.0 ],
					"text" : "FM synthesis: modulator values are multipliers:\n   carrier freq * harmonicity = mod freq\n   mod freq * modIndex = mod gain\nHarmonicity at integer values = harmonic series partials\nHarmonicity integer ratios will be more consonant\nHarmonicity at decimal values will be more dissonant"
				}

			}
, 			{
				"box" : 				{
					"fontface" : 2,
					"fontname" : "Verdana",
					"id" : "obj-32",
					"linecount" : 2,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 561.3333500623703, 117.33333683013916, 116.0, 31.0 ],
					"text" : "gap adds time (ms) between env loops"
				}

			}
, 			{
				"box" : 				{
					"fontface" : 2,
					"fontname" : "Verdana",
					"id" : "obj-25",
					"linecount" : 3,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 704.666687667369843, 151.999998331069946, 99.333312332630157, 43.0 ],
					"text" : "reverb mix only changes on new shred"
				}

			}
, 			{
				"box" : 				{
					"fontface" : 1,
					"fontname" : "Verdana",
					"fontsize" : 20.0,
					"id" : "obj-216",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 312.000009298324585, 47.333334743976593, 198.0, 31.0 ],
					"text" : "ChucK => Max",
					"textjustification" : 1
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Verdana",
					"id" : "obj-31",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 108.666669905185699, 80.666669070720673, 58.0, 19.0 ],
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
					"fontname" : "Verdana",
					"id" : "obj-29",
					"linecount" : 2,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 660.666686356067657, 151.999998331069946, 42.0, 31.0 ],
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
					"patching_rect" : [ 662.666686415672302, 208.0, 58.0, 21.0 ],
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
					"patching_rect" : [ 662.666686415672302, 182.6666659116745, 41.0, 21.0 ]
				}

			}
, 			{
				"box" : 				{
					"fontface" : 1,
					"fontname" : "Verdana",
					"id" : "obj-23",
					"linecount" : 2,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 146.923090934753418, 152.307706832885742, 42.0, 31.0 ],
					"text" : "run\nadd ",
					"textjustification" : 1
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Verdana",
					"id" : "obj-20",
					"linecount" : 2,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 572.307746887207031, 152.307706832885742, 69.0, 31.0 ],
					"text" : "gap duration ms",
					"textjustification" : 1
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Verdana",
					"id" : "obj-16",
					"linecount" : 2,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 493.333348035812378, 152.000004529953003, 74.0, 31.0 ],
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
					"patching_rect" : [ 413.333345651626587, 163.33333820104599, 75.0, 19.0 ],
					"text" : "mod Index",
					"textjustification" : 1
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Verdana",
					"id" : "obj-3",
					"maxclass" : "number",
					"maximum" : 20000,
					"minimum" : 0,
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "bang" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 585.333350777626038, 183.333338797092438, 50.0, 21.0 ]
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Verdana",
					"fontsize" : 10.0,
					"id" : "obj-7",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 579.333350598812103, 208.000006198883057, 81.0, 21.0 ],
					"text" : "gapTimeMs $1"
				}

			}
, 			{
				"box" : 				{
					"attr" : "debug",
					"fontname" : "Verdana",
					"id" : "obj-14",
					"maxclass" : "attrui",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 211.0, 272.0, 125.0, 21.0 ]
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
					"fontname" : "Verdana",
					"fontsize" : 10.0,
					"id" : "obj-9",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 423.333345949649811, 208.000006198883057, 75.0, 21.0 ],
					"text" : "modIndex $1"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Verdana",
					"format" : 6,
					"id" : "obj-10",
					"maxclass" : "flonum",
					"maximum" : 50.0,
					"minimum" : 0.1,
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "bang" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 423.333345949649811, 183.333338797092438, 49.0, 21.0 ]
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Verdana",
					"id" : "obj-8",
					"maxclass" : "number",
					"maximum" : 20000,
					"minimum" : 1,
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "bang" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 501.333348274230957, 183.333338797092438, 54.0, 21.0 ]
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Verdana",
					"fontsize" : 10.0,
					"id" : "obj-4",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 501.333348274230957, 208.000006198883057, 75.0, 21.0 ],
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
					"patching_rect" : [ 326.66667640209198, 163.33333820104599, 75.0, 19.0 ],
					"text" : "harmonicity",
					"textjustification" : 1
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Verdana",
					"id" : "obj-151",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 278.000008285045624, 163.33333820104599, 38.0, 19.0 ],
					"text" : "gain",
					"textjustification" : 1
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Verdana",
					"id" : "obj-150",
					"linecount" : 2,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 203.333339393138885, 152.000004529953003, 64.0, 31.0 ],
					"text" : "carrier frequency",
					"textjustification" : 1
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Verdana",
					"fontsize" : 10.0,
					"id" : "obj-6",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 272.666674792766571, 208.000006198883057, 63.0, 21.0 ],
					"text" : "carGain $1"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Verdana",
					"format" : 6,
					"id" : "obj-2",
					"maxclass" : "flonum",
					"maximum" : 1.0,
					"minimum" : 0.0,
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "bang" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 272.666674792766571, 183.333338797092438, 41.0, 21.0 ]
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Verdana",
					"fontsize" : 10.0,
					"id" : "obj-49",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 336.666676700115204, 208.000006198883057, 84.0, 21.0 ],
					"text" : "harmonicity $1"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Verdana",
					"format" : 6,
					"id" : "obj-50",
					"maxclass" : "flonum",
					"maximum" : 50.0,
					"minimum" : 0.1,
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "bang" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 336.666676700115204, 183.333338797092438, 49.0, 21.0 ]
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Verdana",
					"format" : 6,
					"id" : "obj-52",
					"maxclass" : "flonum",
					"maximum" : 4000.0,
					"minimum" : 0.0,
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "bang" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 206.000006139278412, 183.333338797092438, 53.0, 21.0 ]
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Verdana",
					"id" : "obj-37",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 70.0, 144.0, 34.0, 21.0 ],
					"text" : "reset"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Verdana",
					"id" : "obj-21",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 206.000006139278412, 207.333339512348175, 62.0, 21.0 ],
					"text" : "carFreq $1"
				}

			}
, 			{
				"box" : 				{
					"fontface" : 1,
					"fontname" : "Verdana",
					"fontsize" : 14.0,
					"id" : "obj-35",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 260.000007748603821, 80.666669070720673, 342.0, 24.0 ],
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
					"fontname" : "Verdana",
					"id" : "obj-18",
					"maxclass" : "newobj",
					"numinlets" : 0,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 180.0, 272.0, 27.0, 21.0 ],
					"text" : "r ck"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Verdana",
					"id" : "obj-17",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 392.0, 240.0, 28.0, 21.0 ],
					"text" : "s ck"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Verdana",
					"id" : "obj-5",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 107.333336532115936, 144.000004291534424, 32.0, 21.0 ],
					"text" : "info"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Verdana",
					"id" : "obj-167",
					"linecount" : 2,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 27.666666865348816, 185.0, 52.000001549720764, 31.0 ],
					"text" : "remove shred #",
					"textjustification" : 1
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Verdana",
					"id" : "obj-85",
					"maxclass" : "number",
					"maximum" : 20,
					"minimum" : 1,
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "bang" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 81.0, 185.0, 38.0, 21.0 ]
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Verdana",
					"fontsize" : 10.0,
					"id" : "obj-83",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 81.0, 208.0, 62.0, 21.0 ],
					"text" : "remove $1"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Verdana",
					"fontsize" : 10.0,
					"id" : "obj-42",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 81.0, 262.0, 61.0, 21.0 ],
					"text" : "remove all"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Verdana",
					"fontsize" : 10.0,
					"id" : "obj-44",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 79.0, 235.0, 68.0, 21.0 ],
					"text" : "remove last"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Verdana",
					"id" : "obj-61",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 2,
					"outlettype" : [ "signal", "signal" ],
					"patching_rect" : [ 178.0, 395.0, 133.0, 21.0 ],
					"text" : "limi~ 2 @threshold -0.5."
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Verdana",
					"id" : "obj-62",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 221.0, 422.0, 75.0, 21.0 ],
					"text" : "startwindow"
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
					"patching_rect" : [ 167.5, 337.0, 151.0, 50.0 ],
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
					"fontname" : "Verdana",
					"id" : "obj-1",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 2,
					"outlettype" : [ "signal", "signal" ],
					"patching_rect" : [ 170.0, 301.0, 154.0, 21.0 ],
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
					"midpoints" : [ 179.5, 327.333329617977142, 177.0, 327.333329617977142 ],
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
					"destination" : [ "obj-1", 0 ],
					"midpoints" : [ 220.5, 298.333328723907471, 179.5, 298.333328723907471 ],
					"source" : [ "obj-14", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"midpoints" : [ 189.5, 299.999995470046997, 179.5, 299.999995470046997 ],
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
					"midpoints" : [ 672.166686415672302, 234.999993503093719, 401.5, 234.999993503093719 ],
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
					"midpoints" : [ 129.0, 138.666670799255371, 138.666670799255371, 138.666670799255371, 138.666670799255371, 175.333338558673859, 90.5, 175.333338558673859 ],
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
					"destination" : [ "obj-30", 0 ],
					"source" : [ "obj-38", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-41", 0 ],
					"source" : [ "obj-39", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-17", 0 ],
					"midpoints" : [ 510.833348274230957, 234.999993503093719, 401.5, 234.999993503093719 ],
					"source" : [ "obj-4", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-39", 0 ],
					"source" : [ "obj-40", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-43", 0 ],
					"source" : [ "obj-41", 0 ]
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
					"midpoints" : [ 346.166676700115204, 232.333326756954193, 401.5, 232.333326756954193 ],
					"source" : [ "obj-49", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"midpoints" : [ 116.833336532115936, 176.333325088024139, 79.333329111337662, 176.333325088024139, 79.333329111337662, 296.999995350837708, 179.5, 296.999995350837708 ],
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
					"midpoints" : [ 282.166674792766571, 232.333326756954193, 401.5, 232.333326756954193 ],
					"source" : [ "obj-6", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-66", 1 ],
					"midpoints" : [ 301.5, 420.199968874454498, 197.500020205974579, 420.199968874454498 ],
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
					"midpoints" : [ 210.0, 387.199968874454498, 301.5, 387.199968874454498 ],
					"source" : [ "obj-63", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-61", 0 ],
					"midpoints" : [ 177.0, 387.199968874454498, 187.5, 387.199968874454498 ],
					"source" : [ "obj-63", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-17", 0 ],
					"midpoints" : [ 588.833350598812103, 234.333326816558838, 401.5, 234.333326816558838 ],
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
					"midpoints" : [ 432.833345949649811, 233.666660130023956, 401.5, 233.666660130023956 ],
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
