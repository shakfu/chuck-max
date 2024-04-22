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
		"rect" : [ 98.0, 106.0, 640.0, 480.0 ],
		"bglocked" : 0,
		"openinpresentation" : 0,
		"default_fontsize" : 12.0,
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
		"subpatcher_template" : "",
		"assistshowspatchername" : 0,
		"boxes" : [ 			{
				"box" : 				{
					"id" : "obj-26",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 301.0, 10.0, 330.0, 20.0 ],
					"text" : "from: https://cycling74.com/forums/sequencer-midi-note-off"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 9.0,
					"hidden" : 1,
					"id" : "obj-1",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "int" ],
					"patching_rect" : [ 81.0, 40.0, 34.0, 19.0 ],
					"text" : "int"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 9.0,
					"hidden" : 1,
					"id" : "obj-2",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 2,
					"outlettype" : [ "bang", "" ],
					"patching_rect" : [ 81.0, 20.0, 34.0, 19.0 ],
					"text" : "sel 0"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-3",
					"maxclass" : "button",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "bang" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 169.0, 62.0, 15.0, 15.0 ]
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 9.0,
					"id" : "obj-4",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 198.0, 62.0, 31.0, 19.0 ],
					"text" : "off"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 9.0,
					"hidden" : 1,
					"id" : "obj-5",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "int" ],
					"patching_rect" : [ 56.0, 41.0, 20.0, 19.0 ],
					"text" : "t 0"
				}

			}
, 			{
				"box" : 				{
					"hkeycolor" : [ 0.501960784313725, 0.501960784313725, 0.501960784313725, 1.0 ],
					"id" : "obj-6",
					"maxclass" : "kslider",
					"mode" : 1,
					"numinlets" : 2,
					"numoutlets" : 2,
					"outlettype" : [ "int", "int" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 296.0, 238.0, 196.0, 34.0 ],
					"whitekeycolor" : [ 1.0, 1.0, 1.0, 1.0 ]
				}

			}
, 			{
				"box" : 				{
					"hkeycolor" : [ 0.501960784313725, 0.501960784313725, 0.501960784313725, 1.0 ],
					"id" : "obj-7",
					"maxclass" : "kslider",
					"numinlets" : 2,
					"numoutlets" : 2,
					"outlettype" : [ "int", "int" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 258.0, 81.0, 196.0, 34.0 ],
					"whitekeycolor" : [ 1.0, 1.0, 1.0, 1.0 ]
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 9.0,
					"id" : "obj-8",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "int" ],
					"patching_rect" : [ 228.0, 191.0, 40.0, 19.0 ],
					"text" : "+ 60"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 9.0,
					"id" : "obj-9",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "int", "bang" ],
					"patching_rect" : [ 56.0, 81.0, 173.0, 19.0 ],
					"text" : "t i b"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 9.0,
					"id" : "obj-10",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 2,
					"outlettype" : [ "int", "int" ],
					"patching_rect" : [ 228.0, 236.0, 32.0, 19.0 ],
					"text" : "flush"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 9.0,
					"id" : "obj-11",
					"maxclass" : "newobj",
					"numinlets" : 3,
					"numoutlets" : 0,
					"patching_rect" : [ 228.0, 257.0, 54.0, 19.0 ],
					"text" : "noteout"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 0.866666666666667, 0.866666666666667, 0.866666666666667, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 9.0,
					"id" : "obj-12",
					"maxclass" : "number",
					"maximum" : 127,
					"minimum" : 1,
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "bang" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 271.0, 168.0, 35.0, 19.0 ],
					"textcolor" : [ 0.0, 0.0, 0.0, 1.0 ],
					"triscale" : 0.9
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 9.0,
					"id" : "obj-13",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 228.0, 213.0, 53.0, 19.0 ],
					"text" : "pack 0 64"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 9.0,
					"id" : "obj-14",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 228.0, 168.0, 26.0, 19.0 ],
					"text" : "iter"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 9.0,
					"id" : "obj-15",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 115.0, 168.0, 68.0, 19.0 ],
					"text" : "append chord"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 9.0,
					"id" : "obj-16",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "set" ],
					"patching_rect" : [ 56.0, 145.0, 31.0, 19.0 ],
					"text" : "t set"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 9.0,
					"id" : "obj-17",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 2,
					"outlettype" : [ "", "" ],
					"patching_rect" : [ 56.0, 123.0, 69.0, 19.0 ],
					"text" : "route symbol"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 9.0,
					"id" : "obj-18",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 115.0, 213.0, 103.0, 19.0 ],
					"text" : "prepend set playing a"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 18.0,
					"id" : "obj-19",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 56.0, 279.0, 437.0, 29.0 ]
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 9.0,
					"id" : "obj-20",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 2,
					"outlettype" : [ "", "" ],
					"patching_rect" : [ 115.0, 146.0, 123.0, 19.0 ],
					"text" : "zl slice 1"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 9.0,
					"id" : "obj-21",
					"items" : [ "off", ",", "Major", ",", "Minor", ",", 5, ",", "Dominant 7th", ",", "Major 7th", ",", "Minor 7th", ",", "Minor Major 7th", ",", "Sus 4", ",", "Sus 2", ",", 6, ",", "Minor 6", ",", 9, ",", "Minor 9", ",", "Major 9", ",", "Minor Major 9", ",", 11, ",", "Minor 11", ",", "Major 11", ",", "Minor Major 11", ",", 13, ",", "Minor 13", ",", "Major 13", ",", "Minor Major 13", ",", "add 9", ",", "Minor add 9", ",", "6 add 9", ",", "Minor 6 add 9", ",", "Dominant 7th add 11", ",", "Major 7th add 11", ",", "Minor 7th add 11", ",", "Minor Major 7th add 11", ",", "Dominant 7th add 13", ",", "Major 7th add 13", ",", "Minor 7th add 13", ",", "Minor Major 7th add 13", ",", "7b5", ",", "7#5", ",", "7b9", ",", "7#9", ",", "7#5b9", ",", "m7b5", ",", "m7#5", ",", "m7b9", ",", "9#11", ",", "9b13", ",", "6sus4", ",", "7sus4", ",", "Major 7th Sus4", ",", "9sus4", ",", "Major 9 Sus4" ],
					"labelclick" : 1,
					"maxclass" : "umenu",
					"numinlets" : 1,
					"numoutlets" : 3,
					"outlettype" : [ "int", "", "" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 56.0, 61.0, 100.0, 19.0 ]
				}

			}
, 			{
				"box" : 				{
					"coll_data" : 					{
						"count" : 51,
						"data" : [ 							{
								"key" : 0,
								"value" : [ "off" ]
							}
, 							{
								"key" : 1,
								"value" : [ "Major", 0, 4, 7 ]
							}
, 							{
								"key" : 2,
								"value" : [ "Minor", 0, 3, 7 ]
							}
, 							{
								"key" : 3,
								"value" : [ 5, 0, 5 ]
							}
, 							{
								"key" : 4,
								"value" : [ "Dominant 7th", 0, 4, 7, 10 ]
							}
, 							{
								"key" : 5,
								"value" : [ "Major 7th", 0, 4, 7, 11 ]
							}
, 							{
								"key" : 6,
								"value" : [ "Minor 7th", 0, 3, 7, 10 ]
							}
, 							{
								"key" : 7,
								"value" : [ "Minor Major 7th", 0, 3, 7, 11 ]
							}
, 							{
								"key" : 8,
								"value" : [ "Sus 4", 0, 5, 7 ]
							}
, 							{
								"key" : 9,
								"value" : [ "Sus 2", 0, 2, 7 ]
							}
, 							{
								"key" : 10,
								"value" : [ 6, 0, 4, 7, 9 ]
							}
, 							{
								"key" : 11,
								"value" : [ "Minor 6", 0, 3, 7, 9 ]
							}
, 							{
								"key" : 12,
								"value" : [ 9, 0, 2, 4, 7, 10 ]
							}
, 							{
								"key" : 13,
								"value" : [ "Minor 9", 0, 2, 3, 7, 10 ]
							}
, 							{
								"key" : 14,
								"value" : [ "Major 9", 0, 2, 4, 7, 11 ]
							}
, 							{
								"key" : 15,
								"value" : [ "Minor Major 9", 0, 2, 3, 7, 11 ]
							}
, 							{
								"key" : 16,
								"value" : [ 11, 0, 2, 4, 5, 7, 11 ]
							}
, 							{
								"key" : 17,
								"value" : [ "Minor 11", 0, 2, 3, 5, 7, 10 ]
							}
, 							{
								"key" : 18,
								"value" : [ "Major 11", 0, 2, 4, 5, 7, 11 ]
							}
, 							{
								"key" : 19,
								"value" : [ "Minor Major 11", 0, 2, 3, 5, 7, 11 ]
							}
, 							{
								"key" : 20,
								"value" : [ 13, 0, 2, 4, 7, 9, 10 ]
							}
, 							{
								"key" : 21,
								"value" : [ "Minor 13", 0, 2, 3, 7, 9, 10 ]
							}
, 							{
								"key" : 22,
								"value" : [ "Major 13", 0, 2, 4, 7, 9, 11 ]
							}
, 							{
								"key" : 23,
								"value" : [ "Minor Major 13", 0, 2, 3, 7, 9, 11 ]
							}
, 							{
								"key" : 24,
								"value" : [ "add 9", 0, 2, 4, 7 ]
							}
, 							{
								"key" : 25,
								"value" : [ "Minor add 9", 0, 2, 3, 7 ]
							}
, 							{
								"key" : 26,
								"value" : [ "6 add 9", 0, 2, 4, 7, 9 ]
							}
, 							{
								"key" : 27,
								"value" : [ "Minor 6 add 9", 0, 2, 3, 7, 9 ]
							}
, 							{
								"key" : 28,
								"value" : [ "Dominant 7th add 11", 0, 4, 5, 7, 10 ]
							}
, 							{
								"key" : 29,
								"value" : [ "Major 7th add 11", 0, 4, 5, 7, 11 ]
							}
, 							{
								"key" : 30,
								"value" : [ "Minor 7th add 11", 0, 3, 5, 7, 10 ]
							}
, 							{
								"key" : 31,
								"value" : [ "Minor Major 7th add 11", 0, 3, 5, 7, 11 ]
							}
, 							{
								"key" : 32,
								"value" : [ "Dominant 7th add 13", 0, 4, 7, 9, 10 ]
							}
, 							{
								"key" : 33,
								"value" : [ "Major 7th add 13", 0, 4, 7, 9, 11 ]
							}
, 							{
								"key" : 34,
								"value" : [ "Minor 7th add 13", 0, 3, 7, 9, 10 ]
							}
, 							{
								"key" : 35,
								"value" : [ "Minor Major 7th add 13", 0, 3, 7, 9, 11 ]
							}
, 							{
								"key" : 36,
								"value" : [ "7b5", 0, 4, 6, 10 ]
							}
, 							{
								"key" : 37,
								"value" : [ "7#5", 0, 4, 8, 10 ]
							}
, 							{
								"key" : 38,
								"value" : [ "7b9", 0, 1, 4, 7, 10 ]
							}
, 							{
								"key" : 39,
								"value" : [ "7#9", 0, 3, 4, 7, 10 ]
							}
, 							{
								"key" : 40,
								"value" : [ "7#5b9", 0, 1, 4, 8, 10 ]
							}
, 							{
								"key" : 41,
								"value" : [ "m7b5", 0, 3, 6, 10 ]
							}
, 							{
								"key" : 42,
								"value" : [ "m7#5", 0, 3, 8, 10 ]
							}
, 							{
								"key" : 43,
								"value" : [ "m7b9", 0, 1, 3, 7, 10 ]
							}
, 							{
								"key" : 44,
								"value" : [ "9#11", 0, 2, 4, 6, 7, 10 ]
							}
, 							{
								"key" : 45,
								"value" : [ "9b13", 0, 2, 4, 7, 8, 10 ]
							}
, 							{
								"key" : 46,
								"value" : [ "6sus4", 0, 5, 7, 9 ]
							}
, 							{
								"key" : 47,
								"value" : [ "7sus4", 0, 5, 7, 10 ]
							}
, 							{
								"key" : 48,
								"value" : [ "Major 7th Sus4", 0, 5, 7, 11 ]
							}
, 							{
								"key" : 49,
								"value" : [ "9sus4", 0, 2, 5, 7, 10 ]
							}
, 							{
								"key" : 50,
								"value" : [ "Major 9 Sus4", 0, 2, 5, 7, 11 ]
							}
 ]
					}
,
					"fontname" : "Arial",
					"fontsize" : 9.0,
					"id" : "obj-22",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 4,
					"outlettype" : [ "", "", "", "" ],
					"patching_rect" : [ 56.0, 102.0, 99.0, 19.0 ],
					"saved_object_attributes" : 					{
						"embed" : 1,
						"precision" : 6
					}
,
					"text" : "coll Chords 1"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 9.0,
					"id" : "obj-23",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 272.0, 147.0, 47.0, 17.0 ],
					"text" : "velocity"
				}

			}
 ],
		"lines" : [ 			{
				"patchline" : 				{
					"destination" : [ "obj-21", 0 ],
					"hidden" : 1,
					"source" : [ "obj-1", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-11", 1 ],
					"order" : 1,
					"source" : [ "obj-10", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-11", 0 ],
					"order" : 1,
					"source" : [ "obj-10", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-6", 1 ],
					"hidden" : 1,
					"order" : 0,
					"source" : [ "obj-10", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-6", 0 ],
					"hidden" : 1,
					"order" : 0,
					"source" : [ "obj-10", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-13", 1 ],
					"source" : [ "obj-12", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-10", 0 ],
					"source" : [ "obj-13", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-8", 0 ],
					"source" : [ "obj-14", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-18", 0 ],
					"source" : [ "obj-15", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-19", 0 ],
					"source" : [ "obj-16", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-16", 0 ],
					"source" : [ "obj-17", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-20", 0 ],
					"source" : [ "obj-17", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-19", 0 ],
					"source" : [ "obj-18", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 1 ],
					"hidden" : 1,
					"source" : [ "obj-2", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-14", 0 ],
					"source" : [ "obj-20", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-15", 0 ],
					"source" : [ "obj-20", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-2", 0 ],
					"hidden" : 1,
					"order" : 0,
					"source" : [ "obj-21", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-9", 0 ],
					"order" : 1,
					"source" : [ "obj-21", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-17", 0 ],
					"source" : [ "obj-22", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"hidden" : 1,
					"source" : [ "obj-3", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-5", 0 ],
					"hidden" : 1,
					"source" : [ "obj-4", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-21", 0 ],
					"hidden" : 1,
					"source" : [ "obj-5", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-8", 1 ],
					"source" : [ "obj-7", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-13", 0 ],
					"source" : [ "obj-8", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-10", 0 ],
					"midpoints" : [ 219.5, 233.0, 237.5, 233.0 ],
					"source" : [ "obj-9", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-22", 0 ],
					"source" : [ "obj-9", 0 ]
				}

			}
 ],
		"dependency_cache" : [  ],
		"autosave" : 0
	}

}
