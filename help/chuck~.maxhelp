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
		"rect" : [ 194.0, 143.0, 678.0, 598.0 ],
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
		"description" : "",
		"digest" : "",
		"tags" : "",
		"style" : "",
		"subpatcher_template" : "",
		"assistshowspatchername" : 0,
		"boxes" : [ 			{
				"box" : 				{
					"id" : "obj-46",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 423.0, 260.0, 26.0, 19.0 ],
					"text" : "off",
					"textjustification" : 1
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-45",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 423.0, 294.0, 26.0, 19.0 ],
					"text" : "~fx",
					"textjustification" : 1
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-44",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 423.0, 277.0, 26.0, 19.0 ],
					"text" : "fx",
					"textjustification" : 1
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-43",
					"linecount" : 3,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 497.0, 387.0, 174.0, 43.0 ],
					"text" : "Use \"from Max 1\" midi port for MIDI playback of chuck~ patch. Try 'midi/polyfony.ck' example."
				}

			}
, 			{
				"box" : 				{
					"bgmode" : 0,
					"border" : 0,
					"clickthrough" : 0,
					"enablehscroll" : 0,
					"enablevscroll" : 0,
					"id" : "obj-33",
					"lockeddragscroll" : 0,
					"lockedsize" : 0,
					"maxclass" : "bpatcher",
					"name" : "midi_player.maxpat",
					"numinlets" : 0,
					"numoutlets" : 1,
					"offset" : [ 0.0, 0.0 ],
					"outlettype" : [ "bang" ],
					"patching_rect" : [ 508.0, 321.0, 162.0, 59.0 ],
					"viewvisibility" : 1
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-38",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 11.0, 538.0, 150.0, 19.0 ],
					"text" : "should give error"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-37",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 324.5, 245.0, 29.5, 21.0 ],
					"text" : "- 1"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-39",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 9.0, 452.0, 154.0, 21.0 ],
					"text" : "= 1 stk/honkeytonk-algo3.ck"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-35",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 238.0, 327.0, 71.0, 21.0 ],
					"text" : "clear globals"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-34",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 164.5, 327.0, 52.0, 21.0 ],
					"text" : "clear vm"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-32",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 260.5, 287.0, 46.0, 21.0 ],
					"text" : "reset id"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-31",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 104.0, 537.0, 63.0, 21.0 ],
					"text" : "loglevel 11"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-21",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 103.5, 509.0, 57.0, 21.0 ],
					"text" : "loglevel 0"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-29",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 103.5, 481.0, 57.0, 21.0 ],
					"text" : "loglevel"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-30",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 9.0, 425.0, 183.0, 21.0 ],
					"text" : "replace 1 stk/honkeytonk-algo1.ck"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-27",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 199.5, 259.0, 31.0, 21.0 ],
					"text" : "time"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-28",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 254.0, 259.0, 47.0, 21.0 ],
					"text" : "chugins"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-24",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 150.0, 259.0, 39.0, 21.0 ],
					"text" : "status"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-25",
					"lastchannelcount" : 0,
					"maxclass" : "live.gain~",
					"numinlets" : 2,
					"numoutlets" : 5,
					"orientation" : 1,
					"outlettype" : [ "signal", "signal", "", "float", "list" ],
					"parameter_enable" : 1,
					"patching_rect" : [ 230.75, 456.0, 136.0, 36.0 ],
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
					"showname" : 0,
					"varname" : "live.gain~"
				}

			}
, 			{
				"box" : 				{
					"attr" : "depth",
					"hidden" : 1,
					"id" : "obj-5",
					"maxclass" : "attrui",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 4.0, 192.0, 150.0, 21.0 ]
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-22",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 171.0, 287.0, 29.5, 21.0 ],
					"text" : "info"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-20",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 347.5, 147.0, 89.0, 21.0 ],
					"text" : "add test/sine.ck"
				}

			}
, 			{
				"box" : 				{
					"automatic" : 1,
					"id" : "obj-19",
					"maxclass" : "scope~",
					"numinlets" : 2,
					"numoutlets" : 0,
					"patching_rect" : [ 403.0, 448.0, 267.0, 118.0 ]
				}

			}
, 			{
				"box" : 				{
					"format" : 6,
					"id" : "obj-17",
					"maxclass" : "flonum",
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "bang" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 180.5, 192.0, 50.0, 21.0 ]
				}

			}
, 			{
				"box" : 				{
					"floatoutput" : 1,
					"id" : "obj-18",
					"maxclass" : "slider",
					"mult" : 0.5,
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 180.5, 96.0, 20.0, 85.0 ],
					"size" : 1.0,
					"varname" : "slider"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-14",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 180.5, 222.0, 43.0, 21.0 ],
					"text" : "wet $1"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-13",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 357.5, 179.0, 69.0, 19.0 ],
					"text" : "test effect"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-23",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 324.5, 272.0, 56.0, 21.0 ],
					"text" : "remove 2"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-16",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 324.5, 299.5, 61.0, 21.0 ],
					"text" : "remove all"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-12",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 324.5, 327.0, 68.0, 21.0 ],
					"text" : "remove last"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-62",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 76.5, 359.0, 42.0, 21.0 ],
					"text" : "run $1"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-60",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 23.5, 230.0, 44.0, 19.0 ],
					"text" : "refresh"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-58",
					"maxclass" : "button",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "bang" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 23.5, 255.0, 24.0, 24.0 ]
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-55",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
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
						"rect" : [ 91.0, 106.0, 640.0, 480.0 ],
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
						"boxes" : [ 							{
								"box" : 								{
									"comment" : "",
									"id" : "obj-1",
									"index" : 1,
									"maxclass" : "inlet",
									"numinlets" : 0,
									"numoutlets" : 1,
									"outlettype" : [ "bang" ],
									"patching_rect" : [ 160.0, 66.0, 30.0, 30.0 ]
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-53",
									"maxclass" : "newobj",
									"numinlets" : 1,
									"numoutlets" : 1,
									"outlettype" : [ "bang" ],
									"patching_rect" : [ 50.0, 74.0, 58.0, 22.0 ],
									"text" : "loadbang"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-50",
									"maxclass" : "newobj",
									"numinlets" : 1,
									"numoutlets" : 2,
									"outlettype" : [ "", "int" ],
									"patching_rect" : [ 50.0, 243.0, 150.0, 22.0 ],
									"text" : "conformpath max absolute"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-49",
									"maxclass" : "newobj",
									"numinlets" : 2,
									"numoutlets" : 2,
									"outlettype" : [ "", "" ],
									"patching_rect" : [ 50.0, 210.0, 148.0, 22.0 ],
									"text" : "combine input ../examples"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-39",
									"maxclass" : "newobj",
									"numinlets" : 1,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 50.0, 308.0, 85.0, 22.0 ],
									"text" : "prepend prefix"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-30",
									"maxclass" : "message",
									"numinlets" : 2,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 50.0, 143.0, 32.0, 22.0 ],
									"text" : "path"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-28",
									"maxclass" : "newobj",
									"numinlets" : 1,
									"numoutlets" : 2,
									"outlettype" : [ "", "" ],
									"patching_rect" : [ 50.0, 180.0, 67.0, 22.0 ],
									"save" : [ "#N", "thispatcher", ";", "#Q", "end", ";" ],
									"text" : "thispatcher"
								}

							}
, 							{
								"box" : 								{
									"comment" : "",
									"id" : "obj-54",
									"index" : 1,
									"maxclass" : "outlet",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 50.0, 389.0, 30.0, 30.0 ]
								}

							}
 ],
						"lines" : [ 							{
								"patchline" : 								{
									"destination" : [ "obj-30", 0 ],
									"source" : [ "obj-1", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-49", 0 ],
									"source" : [ "obj-28", 1 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-28", 0 ],
									"source" : [ "obj-30", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-54", 0 ],
									"source" : [ "obj-39", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-50", 0 ],
									"source" : [ "obj-49", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-39", 0 ],
									"source" : [ "obj-50", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-30", 0 ],
									"source" : [ "obj-53", 0 ]
								}

							}
 ]
					}
,
					"patching_rect" : [ 23.5, 293.0, 66.0, 21.0 ],
					"saved_object_attributes" : 					{
						"description" : "",
						"digest" : "",
						"globalpatchername" : "",
						"tags" : ""
					}
,
					"text" : "p examples"
				}

			}
, 			{
				"box" : 				{
					"autopopulate" : 1,
					"depth" : 2,
					"id" : "obj-40",
					"items" : [ "ai/features/centroid.ck", ",", "ai/features/chroma.ck", ",", "ai/features/features-combined.ck", ",", "ai/features/flux.ck", ",", "ai/features/flux0.ck", ",", "ai/features/kurtosis.ck", ",", "ai/features/mfcc-basic.ck", ",", "ai/features/mfcc-mean.ck", ",", "ai/features/rms.ck", ",", "ai/features/rolloff.ck", ",", "ai/features/rolloff2.ck", ",", "ai/features/sfm.ck", ",", "ai/features/zerox.ck", ",", "ai/genre-classify/feature-extract.ck", ",", "ai/genre-classify/genre-classify.ck", ",", "ai/genre-classify/x-validate.ck", ",", "ai/hmm/hmm-load.ck", ",", "ai/hmm/hmm-train.ck", ",", "ai/knn/knn-search.ck", ",", "ai/knn/knn2-classify.ck", ",", "ai/knn/knn2-search.ck", ",", "ai/mlp/mlp-manual.ck", ",", "ai/mlp/mlp.ck", ",", "ai/mlp/model-load.ck", ",", "ai/mlp/model-save.ck", ",", "ai/pca/pca.ck", ",", "ai/svm/svm-basic.ck", ",", "ai/wekinator/wekinator-basic.ck", ",", "ai/wekinator/wekinator-customize.ck", ",", "ai/wekinator/wekinator-import.ck", ",", "ai/word2vec/poem-i-feel.ck", ",", "ai/word2vec/poem-randomwalk.ck", ",", "ai/word2vec/poem-spew.ck", ",", "ai/word2vec/poem-ungenerate.ck", ",", "ai/word2vec/word2vec-basic.ck", ",", "ai/word2vec/word2vec-prompt.ck", ",", "analysis/autocorr.ck", ",", "analysis/concat.ck", ",", "analysis/dct.ck", ",", "analysis/fft.ck", ",", "analysis/fft1.ck", ",", "analysis/fft2.ck", ",", "analysis/fft3.ck", ",", "analysis/flip.ck", ",", "analysis/ifft.ck", ",", "analysis/ifft2.ck", ",", "analysis/ifft3.ck", ",", "analysis/PitchTrack.ck", ",", "analysis/Sigmund.ck", ",", "analysis/tracking/harm.ck", ",", "analysis/tracking/pitch-fifth.ck", ",", "analysis/tracking/pitch-seventh.ck", ",", "analysis/tracking/pitch-third.ck", ",", "analysis/tracking/pitch-track.ck", ",", "analysis/tracking/Smacking.ck", ",", "analysis/tracking/Tracking.ck", ",", "analysis/win.ck", ",", "analysis/xcorr.ck", ",", "analysis/xsynth.ck", ",", "array/array_append.ck", ",", "array/array_argument.ck", ",", "array/array_assign.ck", ",", "array/array_associative.ck", ",", "array/array_capacity.ck", ",", "array/array_dynamic.ck", ",", "array/array_erase.ck", ",", "array/array_erase2.ck", ",", "array/array_mdim.ck", ",", "array/array_mmixed.ck", ",", "array/array_negative.ck", ",", "array/array_range.ck", ",", "array/array_resize.ck", ",", "array/array_reverse.ck", ",", "array/array_shuffle.ck", ",", "array/array_sort.ck", ",", "array/array_storage.ck", ",", "array/array_sub_assign.ck", ",", "array/array_ugens.ck", ",", "array/array_zero.ck", ",", "array/foreach-1.ck", ",", "array/foreach-2.ck", ",", "array/foreach-3.ck", ",", "array/foreach-4.ck", ",", "array/foreach-5.ck", ",", "array/foreach-6.ck", ",", "array/foreach-auto1.ck", ",", "array/foreach-auto2.ck", ",", "basic/adc.ck", ",", "basic/adsr.ck", ",", "basic/alarm.ck", ",", "basic/args.ck", ",", "basic/bar.ck", ",", "basic/blit.ck", ",", "basic/blit2.ck", ",", "basic/chirp.ck", ",", "basic/chirp2.ck", ",", "basic/comb.ck", ",", "basic/curly++.ck", ",", "basic/curly.ck", ",", "basic/delay.ck", ",", "basic/demo0.ck", ",", "basic/demo1.ck", ",", "basic/demo2.ck", ",", "basic/demo3.ck", ",", "basic/doh.ck", ",", "basic/echo.ck", ",", "basic/envelope.ck", ",", "basic/fm.ck", ",", "basic/fm2.ck", ",", "basic/fm3.ck", ",", "basic/foo.ck", ",", "basic/foo2.ck", ",", "basic/func.ck", ",", "basic/i-robot.ck", ",", "basic/imp.ck", ",", "basic/infnan.ck", ",", "basic/larry++.ck", ",", "basic/larry.ck", ",", "basic/lfo.ck", ",", "basic/moe++.ck", ",", "basic/moe.ck", ",", "basic/oscillatronx.ck", ",", "basic/phasor.ck", ",", "basic/pulse.ck", ",", "basic/rec-auto-stereo.ck", ",", "basic/rec-auto.ck", ",", "basic/rec.ck", ",", "basic/ring.ck", ",", "basic/sndbuf.ck", ",", "basic/step.ck", ",", "basic/tick.ck", ",", "basic/tick2.ck", ",", "basic/unchuck.ck", ",", "basic/valueat.ck", ",", "basic/whirl.ck", ",", "basic/whole.ck", ",", "basic/wind.ck", ",", "basic/wind2.ck", ",", "basic/zerox.ck", ",", "class/constructors.ck", ",", "class/ctors-dtor.ck", ",", "class/destructor.ck", ",", "class/dinky.ck", ",", "class/try.ck", ",", "ctrl/ctrl_break.ck", ",", "ctrl/ctrl_break_nested.ck", ",", "ctrl/ctrl_continue.ck", ",", "ctrl/ctrl_do_until.ck", ",", "ctrl/ctrl_do_while.ck", ",", "ctrl/ctrl_for_loop.ck", ",", "ctrl/ctrl_foreach.ck", ",", "ctrl/ctrl_repeat.ck", ",", "ctrl/ctrl_sequencer.ck", ",", "ctrl/ctrl_until.ck", ",", "ctrl/ctrl_while.ck", ",", "deep/chant.ck", ",", "deep/dither.ck", ",", "deep/dtmf.ck", ",", "deep/follower.ck", ",", "deep/ks-chord.ck", ",", "deep/plu.ck", ",", "deep/plu2.ck", ",", "deep/plu3.ck", ",", "deep/say-chu.ck", ",", "deep/shepard.ck", ",", "deep/thx.ck", ",", "deep/unclap.ck", ",", "effects/ABSaturator.ck", ",", "effects/autotune.ck", ",", "effects/Bitcrusher.ck", ",", "effects/chorus.ck", ",", "effects/ExpDelay.ck", ",", "effects/ExpEnv-modes.ck", ",", "effects/ExpEnv.ck", ",", "effects/FoldbackSaturator-index.ck", ",", "effects/FoldbackSaturator-threshold.ck", ",", "effects/GVerb.ck", ",", "effects/KasFilter-demo.ck", ",", "effects/pitch-shift.ck", ",", "effects/PowerADSR-feedback-beatings.ck", ",", "effects/PowerADSR.ck", ",", "effects/reverb.ck", ",", "effects/Spectacle.ck", ",", "effects/WinFuncEnv.ck", ",", "event/broadcast.ck", ",", "event/event-extend.ck", ",", "event/event-extend2.ck", ",", "event/event-x-bpm-1.ck", ",", "event/event-x-bpm-2.ck", ",", "event/signal.ck", ",", "event/signal4.ck", ",", "extend/chugen.ck", ",", "extend/chugraph.ck", ",", "fauck/clarinet.ck", ",", "fauck/compressor.ck", ",", "fauck/crybaby.ck", ",", "fauck/distortion.ck", ",", "fauck/fau.ck", ",", "fauck/fauReverb.ck", ",", "fauck/file-playback.ck", ",", "fauck/flanger.ck", ",", "fauck/flute.ck", ",", "fauck/mx.fauReverb.ck", ",", "fauck/noise.ck", ",", "fauck/phaser.ck", ",", "fauck/polyphony-simple.ck", ",", "fauck/polyphony.ck", ",", "fauck/reverb.ck", ",", "fauck/sidechain-compressor.ck", ",", "fauck/smcAdvancedExample.ck", ",", "fauck/string.ck", ",", "fauck/test-import.ck", ",", "fauck/test.ck", ",", "fauck/vocoder.ck", ",", "fauck/wah.ck", ",", "filter/bpf.ck", ",", "filter/brf.ck", ",", "filter/dcblocker.ck", ",", "filter/Elliptic-help.ck", ",", "filter/FIR-convolve-homer.ck", ",", "filter/FIR-greyhole-down-up-decimate.ck", ",", "filter/FIR-sinc-explicit.ck", ",", "filter/hpf.ck", ",", "filter/KasFilter.ck", ",", "filter/lpf.ck", ",", "filter/Multicomb.ck", ",", "filter/resonz.ck", ",", "filter/rhpf.ck", ",", "filter/rlpf.ck", ",", "filter/WPDiodeLadder.ck", ",", "filter/WPKorg35.ck", ",", "func/func_doozey.ck", ",", "func/func_obj_arg.ck", ",", "func/func_overload.ck", ",", "func/func_overload_member.ck", ",", "func/func_overload_multi.ck", ",", "func/func_recursion.ck", ",", "func/func_void.ck", ",", "hanoi/hanoi++.ck", ",", "hanoi/hanoi.ck", ",", "hanoi/hanoi2.ck", ",", "hanoi/hanoi3.ck", ",", "hid/gametra.ck", ",", "hid/joy-fm.ck", ",", "hid/joy-noise.ck", ",", "hid/joy-shake.ck", ",", "hid/joy.ck", ",", "hid/kb.ck", ",", "hid/kbhit/clix.ck", ",", "hid/kbhit/clix2.ck", ",", "hid/kbhit/clix3.ck", ",", "hid/kbhit/kbhit.ck", ",", "hid/kbhit/kbhit2.ck", ",", "hid/keyboard-organ.ck", ",", "hid/mouse-abs.ck", ",", "hid/mouse-fm.ck", ",", "hid/mouse.ck", ",", "hid/tilt.ck", ",", "io/chout.ck", ",", "io/read-byte.ck", ",", "io/read-float.ck", ",", "io/read-int.ck", ",", "io/read-line.ck", ",", "io/read-str.ck", ",", "io/read-tokens.ck", ",", "io/read-wav-raw.ck", ",", "io/seek.ck", ",", "io/write-auto.ck", ",", "io/write-byte.ck", ",", "io/write.ck", ",", "io/write2.ck", ",", "machine/crash.ck", ",", "machine/eval-global.ck", ",", "machine/eval.ck", ",", "machine/intsize.ck", ",", "machine/is-realtime.ck", ",", "machine/machine-help.ck", ",", "machine/machine-shred.ck", ",", "machine/test1.ck", ",", "machine/test2.ck", ",", "machine/version.ck", ",", "math/int-dist.ck", ",", "math/map.ck", ",", "math/math-help.ck", ",", "math/maybe.ck", ",", "math/randomize.ck", ",", "max/amen.ck", ",", "max/fmenv.ck", ",", "max/test.ck", ",", "midi/gomidi.ck", ",", "midi/gomidi2.ck", ",", "midi/midifile-play.ck", ",", "midi/midiout.ck", ",", "midi/polyfony.ck", ",", "midi/polyfony2.ck", ",", "multi/i.ck", ",", "multi/n.ck", ",", "multi/we-robot.ck", ",", "oper/oper_arith_chuck.ck", ",", "oper/oper_assign.ck", ",", "oper/oper_logic.ck", ",", "oper/oper_post_inc.ck", ",", "oper/oper_pre_inc.ck", ",", "oper/overload_class.ck", ",", "oper/overload_gru.ck", ",", "oper/overload_overview.ck", ",", "oper/overload_pre_post.ck", ",", "oper/overload_public.ck", ",", "osc/multi-msg/r-multi-msg.ck", ",", "osc/multi-msg/s-multi-msg.ck", ",", "osc/multicast/r.ck", ",", "osc/multicast/s-multicast.ck", ",", "osc/one2many/r.ck", ",", "osc/one2many/s-sender.ck", ",", "osc/osc-dump.ck", ",", "osc/r.ck", ",", "osc/s.ck", ",", "osc/wildcards/r-wildcards.ck", ",", "osc/wildcards/s-multi.ck", ",", "otf/otf_01.ck", ",", "otf/otf_02.ck", ",", "otf/otf_03.ck", ",", "otf/otf_04.ck", ",", "otf/otf_05.ck", ",", "otf/otf_06.ck", ",", "otf/otf_07.ck", ",", "serial/byte.ck", ",", "serial/bytes.ck", ",", "serial/ints-ascii.ck", ",", "serial/ints-bin.ck", ",", "serial/lines.ck", ",", "serial/list.ck", ",", "serial/write-bytes.ck", ",", "serial/write.ck", ",", "shred/ancestor.ck", ",", "shred/dir.ck", ",", "shred/machine.ck", ",", "shred/parent.ck", ",", "shred/powerup.ck", ",", "shred/spork.ck", ",", "shred/spork2-exit.ck", ",", "shred/spork2-remove.ck", ",", "shred/spork2.ck", ",", "spatial/AmbPan3-CCRMA.ck", ",", "spatial/Pan4.ck", ",", "spatial/Pan8.ck", ",", "special/Dyno-compress.ck", ",", "special/Dyno-duck.ck", ",", "special/Dyno-limit.ck", ",", "special/Gen10-test.ck", ",", "special/Gen17-test.ck", ",", "special/Gen5-test.ck", ",", "special/Gen7-test.ck", ",", "special/Gen9-test.ck", ",", "special/GenX-CurveTable-test.ck", ",", "special/GenX-WarpTable-test.ck", ",", "special/LiSa-load.ck", ",", "special/LiSa-munger1.ck", ",", "special/LiSa-munger2.ck", ",", "special/LiSa-munger3.ck", ",", "special/LiSa-simplelooping.ck", ",", "special/LiSa-stereo.ck", ",", "special/LiSa-track1.ck", ",", "special/LiSa-track2.ck", ",", "special/LiSa-track3.ck", ",", "special/LiSa-track4.ck", ",", "special/LiSa-track5.ck", ",", "special/LiSa-trigger.ck", ",", "special/Mesh2D.ck", ",", "special/readme-GenX.ck", ",", "special/readme-LiSa1.ck", ",", "special/readme-LiSa2.ck", ",", "special/scream-o-matic/scream-o-matic.ck", ",", "special/twilight/twilight-granular-kb-interp.ck", ",", "special/twilight/twilight-granular-kb.ck", ",", "stereo/array.ck", ",", "stereo/curly2.ck", ",", "stereo/larry2.ck", ",", "stereo/moe2.ck", ",", "stereo/powerup2.ck", ",", "stereo/stereo-noise.ck", ",", "stereo/ugen-array.ck", ",", "stk/band-o-matic.ck", ",", "stk/bandedwg.ck", ",", "stk/bandedwg2.ck", ",", "stk/blowbotl.ck", ",", "stk/blowbotl2.ck", ",", "stk/blowhole.ck", ",", "stk/blowhole2.ck", ",", "stk/bowed.ck", ",", "stk/bowed2.ck", ",", "stk/brass.ck", ",", "stk/brass2.ck", ",", "stk/clarinet.ck", ",", "stk/clarinet2.ck", ",", "stk/flute.ck", ",", "stk/frenchrn-algo2.ck", ",", "stk/hevymetl-algo3.ck", ",", "stk/hevymetl-dance-now.ck", ",", "stk/hevymetl-trumpet-algo3.ck", ",", "stk/honkeytonk-algo1.ck", ",", "stk/honkeytonk-algo3.ck", ",", "stk/krstlchr-algo7.ck", ",", "stk/mand-o-matic-simple.ck", ",", "stk/mand-o-matic.ck", ",", "stk/mandolin.ck", ",", "stk/modalbar.ck", ",", "stk/modalbar2.ck", ",", "stk/mode-o-matic.ck", ",", "stk/mode-o-test.ck", ",", "stk/modulate.ck", ",", "stk/moog.ck", ",", "stk/moog2.ck", ",", "stk/moogie.ck", ",", "stk/rhodey-song.ck", ",", "stk/rhodey.ck", ",", "stk/saxofony.ck", ",", "stk/shake-cycle.ck", ",", "stk/shake-o-matic.ck", ",", "stk/sitar.ck", ",", "stk/stif-o-karp.ck", ",", "stk/stifkarp.ck", ",", "stk/subnoise-audio.ck", ",", "stk/subnoise-control.ck", ",", "stk/voic-o-form.ck", ",", "stk/wurley.ck", ",", "stk/wurley2.ck", ",", "stk/wurley3.ck", ",", "stk/wvout-24bit.ck", ",", "string/escape.ck", ",", "string/readline.ck", ",", "string/replace.ck", ",", "string/strops.ck", ",", "string/strops2.ck", ",", "string/token.ck", ",", "string/tostr.ck", ",", "test/callback.ck", ",", "test/global-event.ck", ",", "test/local-event.ck", ",", "test/sine.ck", ",", "time/time_operations.ck", ",", "time/time_types.ck", ",", "type/polymorph.ck", ",", "type/type_analysis.ck", ",", "type/type_auto.ck", ",", "type/type_object.ck", ",", "type/type_primitives.ck", ",", "type/type_query.ck", ",", "type/type_type.ck", ",", "util/help.ck", ",", "util/status.ck", ",", "vector/interpolate.ck", ",", "vector/vec2.ck", ",", "vector/vec3.ck", ",", "vector/vec4.ck", ",", "vector/vecs-cast.ck", ",", "warpbuf/mx.warpbuf_basic.ck", ",", "warpbuf/warpbuf_advanced.ck", ",", "warpbuf/warpbuf_basic.ck", ",", "warpbuf/warpbuf_no_warp_file.ck", ",", "warpbuf/warpbuf_silent.ck" ],
					"maxclass" : "umenu",
					"numinlets" : 1,
					"numoutlets" : 3,
					"outlettype" : [ "int", "", "" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 23.5, 327.0, 125.0, 21.0 ],
					"prefix" : "~/Downloads/projects/chuck-max/examples/",
					"types" : ".ck"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-15",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 238.0, 222.0, 102.0, 21.0 ],
					"text" : "freq_multiplier $1"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-3",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 357.5, 200.0, 88.0, 21.0 ],
					"text" : "+ basic/echo.ck"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-6",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 212.0, 287.0, 34.0, 21.0 ],
					"text" : "reset"
				}

			}
, 			{
				"box" : 				{
					"format" : 6,
					"id" : "obj-10",
					"maxclass" : "flonum",
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "bang" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 238.0, 192.0, 50.0, 21.0 ]
				}

			}
, 			{
				"box" : 				{
					"floatoutput" : 1,
					"id" : "obj-2",
					"maxclass" : "slider",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 238.0, 96.0, 20.0, 85.0 ],
					"size" : 20.0
				}

			}
, 			{
				"box" : 				{
					"disabled" : [ 0, 0, 0 ],
					"id" : "obj-41",
					"itemtype" : 0,
					"maxclass" : "radiogroup",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 403.0, 261.5, 18.0, 50.0 ],
					"size" : 3,
					"value" : 0
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-36",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 2,
					"outlettype" : [ "signal", "signal" ],
					"patching_rect" : [ 403.0, 327.0, 68.0, 21.0 ],
					"text" : "gate~ 2"
				}

			}
, 			{
				"box" : 				{
					"args" : [ "@module", 3, "@vol", -99 ],
					"bgmode" : 0,
					"border" : 0,
					"clickthrough" : 0,
					"enablehscroll" : 0,
					"enablevscroll" : 0,
					"id" : "obj-26",
					"lockeddragscroll" : 0,
					"lockedsize" : 0,
					"maxclass" : "bpatcher",
					"name" : "demosound.maxpat",
					"numinlets" : 0,
					"numoutlets" : 1,
					"offset" : [ 0.0, 0.0 ],
					"outlettype" : [ "signal" ],
					"patching_rect" : [ 452.0, 222.0, 225.0, 95.0 ],
					"viewvisibility" : 1
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-11",
					"linecount" : 2,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 16.0, 49.0, 656.0, 43.0 ],
					"text" : "This external is intended to demonstrate chuck running in Max's audio thread. Note that you can run multiple 'shreds' concurrently. Just hit bang more than once and you can remove the second by [remove 2] or [remove last] or just [remove]\n"
				}

			}
, 			{
				"box" : 				{
					"fontface" : 1,
					"fontsize" : 12.0,
					"id" : "obj-8",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 16.0, 16.0, 359.0, 21.0 ],
					"text" : "chuck~ : embedding chuck in a max/msp external"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-7",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 118.25, 230.0, 27.5, 19.0 ],
					"text" : "run"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-9",
					"maxclass" : "button",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "bang" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 120.0, 255.0, 24.0, 24.0 ]
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-4",
					"maxclass" : "ezdac~",
					"numinlets" : 2,
					"numoutlets" : 0,
					"patching_rect" : [ 230.75, 517.0, 45.0, 45.0 ]
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Verdana",
					"fontsize" : 10.0,
					"id" : "obj-1",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 2,
					"outlettype" : [ "signal", "signal" ],
					"patching_rect" : [ 238.0, 397.0, 119.0, 21.0 ],
					"saved_object_attributes" : 					{
						"file" : "max/test.ck"
					}
,
					"text" : "chuck~ 2 max/test.ck"
				}

			}
 ],
		"lines" : [ 			{
				"patchline" : 				{
					"destination" : [ "obj-19", 1 ],
					"order" : 0,
					"source" : [ "obj-1", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-19", 0 ],
					"order" : 0,
					"source" : [ "obj-1", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-25", 1 ],
					"order" : 1,
					"source" : [ "obj-1", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-25", 0 ],
					"order" : 1,
					"source" : [ "obj-1", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-15", 0 ],
					"source" : [ "obj-10", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"source" : [ "obj-12", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"source" : [ "obj-14", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"source" : [ "obj-15", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"source" : [ "obj-16", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-14", 0 ],
					"source" : [ "obj-17", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-17", 0 ],
					"source" : [ "obj-18", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-10", 0 ],
					"source" : [ "obj-2", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"source" : [ "obj-20", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"source" : [ "obj-21", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"source" : [ "obj-22", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"source" : [ "obj-23", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4", 1 ],
					"source" : [ "obj-25", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4", 0 ],
					"source" : [ "obj-25", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-36", 1 ],
					"source" : [ "obj-26", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"source" : [ "obj-27", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"source" : [ "obj-28", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"source" : [ "obj-29", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"source" : [ "obj-3", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"source" : [ "obj-30", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"source" : [ "obj-31", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"source" : [ "obj-32", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"source" : [ "obj-34", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"source" : [ "obj-35", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 1 ],
					"order" : 0,
					"source" : [ "obj-36", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"order" : 1,
					"source" : [ "obj-36", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-19", 0 ],
					"order" : 0,
					"source" : [ "obj-36", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-25", 1 ],
					"order" : 1,
					"source" : [ "obj-36", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-25", 0 ],
					"order" : 2,
					"source" : [ "obj-36", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"source" : [ "obj-37", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"source" : [ "obj-39", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-62", 0 ],
					"source" : [ "obj-40", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-36", 0 ],
					"source" : [ "obj-41", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-40", 0 ],
					"hidden" : 1,
					"midpoints" : [ 13.5, 321.0, 33.0, 321.0 ],
					"source" : [ "obj-5", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-40", 0 ],
					"source" : [ "obj-55", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-55", 0 ],
					"source" : [ "obj-58", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"source" : [ "obj-6", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"source" : [ "obj-62", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"source" : [ "obj-9", 0 ]
				}

			}
 ],
		"parameters" : 		{
			"obj-25" : [ "live.gain~", "live.gain~", 0 ],
			"obj-26::obj-21::obj-6" : [ "live.tab[3]", "live.tab[1]", 0 ],
			"obj-26::obj-35" : [ "[5]", "Level", 0 ],
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
, 			{
				"name" : "demosound.maxpat",
				"bootpath" : "C74:/help/msp",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "interfacecolor.js",
				"bootpath" : "C74:/interfaces",
				"type" : "TEXT",
				"implicit" : 1
			}
, 			{
				"name" : "midi_player.maxpat",
				"bootpath" : "~/Downloads/projects/chuck-max/patchers/tests",
				"patcherrelativepath" : "../patchers/tests",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "random.svg",
				"bootpath" : "C74:/media/max/picts/m4l-picts",
				"type" : "svg",
				"implicit" : 1
			}
, 			{
				"name" : "saw.svg",
				"bootpath" : "C74:/media/max/picts/m4l-picts",
				"type" : "svg",
				"implicit" : 1
			}
, 			{
				"name" : "sine.svg",
				"bootpath" : "C74:/media/max/picts/m4l-picts",
				"type" : "svg",
				"implicit" : 1
			}
, 			{
				"name" : "square.svg",
				"bootpath" : "C74:/media/max/picts/m4l-picts",
				"type" : "svg",
				"implicit" : 1
			}
 ],
		"autosave" : 0
	}

}
