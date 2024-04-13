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
		"rect" : [ 96.0, 106.0, 640.0, 480.0 ],
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
					"fontname" : "Verdana",
					"fontsize" : 11.0,
					"id" : "obj-8",
					"linecount" : 2,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 97.0, 38.0, 104.0, 33.0 ],
					"text" : "drop a folder with folders here"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Verdana",
					"fontsize" : 11.0,
					"hidden" : 1,
					"id" : "obj-6",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 75.0, 186.0, 91.0, 22.0 ],
					"text" : "prepend prefix"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Verdana",
					"fontsize" : 11.0,
					"hidden" : 1,
					"id" : "obj-4",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 153.0, 126.0, 91.0, 22.0 ],
					"text" : "prepend prefix"
				}

			}
, 			{
				"box" : 				{
					"allowdrag" : 0,
					"autopopulate" : 1,
					"fontname" : "Verdana",
					"fontsize" : 11.0,
					"id" : "obj-2",
					"items" : [ "band-o-matic.ck", ",", "bandedwg.ck", ",", "bandedwg2.ck", ",", "blowbotl.ck", ",", "blowbotl2.ck", ",", "blowhole.ck", ",", "blowhole2.ck", ",", "bowed.ck", ",", "bowed2.ck", ",", "brass.ck", ",", "brass2.ck", ",", "clarinet.ck", ",", "clarinet2.ck", ",", "flute.ck", ",", "frenchrn-algo2.ck", ",", "hevymetl-algo3.ck", ",", "hevymetl-dance-now.ck", ",", "hevymetl-trumpet-algo3.ck", ",", "honkeytonk-algo1.ck", ",", "honkeytonk-algo3.ck", ",", "krstlchr-algo7.ck", ",", "mand-o-matic-simple.ck", ",", "mand-o-matic.ck", ",", "mandolin.ck", ",", "modalbar.ck", ",", "modalbar2.ck", ",", "mode-o-matic.ck", ",", "mode-o-test.ck", ",", "modulate.ck", ",", "moog.ck", ",", "moog2.ck", ",", "moogie.ck", ",", "rhodey-song.ck", ",", "rhodey.ck", ",", "saxofony.ck", ",", "shake-cycle.ck", ",", "shake-o-matic.ck", ",", "sitar.ck", ",", "stif-o-karp.ck", ",", "stifkarp.ck", ",", "subnoise-audio.ck", ",", "subnoise-control.ck", ",", "voic-o-form.ck", ",", "wurley.ck", ",", "wurley2.ck", ",", "wurley3.ck", ",", "wvout-24bit.ck" ],
					"maxclass" : "umenu",
					"numinlets" : 1,
					"numoutlets" : 3,
					"outlettype" : [ "int", "", "" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 217.0, 100.0, 100.0, 22.0 ],
					"prefix" : "~/Downloads/projects/chuck/examples/stk/"
				}

			}
, 			{
				"box" : 				{
					"allowdrag" : 0,
					"autopopulate" : 1,
					"fontname" : "Verdana",
					"fontsize" : 11.0,
					"id" : "obj-1",
					"items" : [ "ai", ",", "analysis", ",", "array", ",", "basic", ",", "book", ",", "class", ",", "ctrl", ",", "data", ",", "deep", ",", "effects", ",", "event", ",", "extend", ",", "filter", ",", "func", ",", "hid", ",", "io", ",", "machine", ",", "math", ",", "midi", ",", "multi", ",", "oper", ",", "osc", ",", "serial", ",", "shred", ",", "spatial", ",", "special", ",", "stereo", ",", "stk", ",", "string", ",", "time", ",", "type", ",", "vector" ],
					"maxclass" : "umenu",
					"numinlets" : 1,
					"numoutlets" : 3,
					"outlettype" : [ "int", "", "" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 112.0, 100.0, 100.0, 22.0 ],
					"prefix" : "~/Downloads/projects/chuck/examples/",
					"types" : "fold"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-5",
					"maxclass" : "dropfile",
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "" ],
					"patching_rect" : [ 75.0, 17.0, 140.0, 157.0 ]
				}

			}
 ],
		"lines" : [ 			{
				"patchline" : 				{
					"destination" : [ "obj-4", 0 ],
					"hidden" : 1,
					"source" : [ "obj-1", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-2", 0 ],
					"hidden" : 1,
					"midpoints" : [ 162.5, 150.0, 87.0, 150.0, 87.0, 85.0, 226.5, 85.0 ],
					"source" : [ "obj-4", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-6", 0 ],
					"hidden" : 1,
					"source" : [ "obj-5", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"hidden" : 1,
					"midpoints" : [ 84.5, 215.0, 62.5, 215.0, 62.5, 90.0, 121.5, 90.0 ],
					"source" : [ "obj-6", 0 ]
				}

			}
 ],
		"dependency_cache" : [  ],
		"autosave" : 0
	}

}
