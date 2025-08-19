//--------------------------------------------------------------------
// name: gtext.ck
// desc: drawing text with GText!
//
// author: Andrew Zhu Aday (https://ccrma.stanford.edu/~azaday/)
//   date: Fall 2024
//--------------------------------------------------------------------

// Scene Setup =======================================================
GOrbitCamera cam --> GG.scene();
GG.scene().camera(cam);

GText text --> GG.scene();
text.sca(.1);
text.text("    PLANETFALL: INTERLOGIC Science Fiction
    Copyright (c) 1983 by Infocom, Inc. All rights reserved.
    PLANETFALL and INTERLOGIC are trademarks of Infocom, Inc.
    Release 29 / Serial number 840118

    ------------------------------------------------------------------------

    Another routine day of drugery aboard the Stellar Patrol Ship Feinstein.
    This morning's assignment for a certain lowly Ensign Seventh Class:
    scrubbing the filthy metal deck at the port end of Level Nine. With your
    Patrol-issue self-contained multi-purpose all-weather scrub-brush you 
    shine the floor with a diligence born of the knowledge that at any 
    moment dreaded Ensign First CLass Blather, the bane of your shipboard
    existence, could appear.");

UI_String text_input(text.text());

UI_Int font_index;
[ 
    "chugl:cousine-regular",
    "chugl:karla-regular",
    "chugl:proggy-tiny",
    "chugl:proggy-clean",
] @=> string builtin_fonts[];

UI_Float4 text_color;
@(1, 1, 1, 1) => text_color.val;

[0.5, 0.5] @=> float control_points[];

UI_Float line_spacing(1.0);
UI_Float text_scale(text.sca().x);
UI_Bool text_rotate;
UI_Float antialias(text.antialias());

// main loop
while (true)
{
    GG.nextFrame() => now;

    UI.setNextWindowSize(@(400, 600), UI_Cond.Once);
    if (UI.begin("GText Example", null, 0))
    {
        UI.setNextItemWidth(-1);
        if (UI.inputTextMultiline("##GText Input", text_input, 10000, @(0,0), 0)) {
            text.text(text_input.val());
        }

        UI.dummy(@(0.0f, 20.0f)); // vertical spacing
        if (UI.listBox("Builtin Fonts", font_index, builtin_fonts, -1)) {
            text.font(builtin_fonts[font_index.val()]);
        }

        if (UI.colorPicker("Text Color", text_color, UI_ColorEditFlags.AlphaPreview | UI_ColorEditFlags.AlphaBar)) {
            text.color(text_color.val());
        }

        if (UI.slider("Control Points", control_points, 0.0, 1.0)) {
            text.controlPoints(@(control_points[0], control_points[1]));
        }

        if (UI.slider("Line Spacing", line_spacing, 0.01, 3.0)) {
            text.spacing(line_spacing.val());
        }

        if (UI.slider("Text Scale", text_scale, 0.01, 1.0)) {
            text.sca(text_scale.val());
        }

        if (UI.slider("Antialias", antialias, 0, 50)) {
            text.antialias(antialias.val());
        }

        UI.checkbox("Rotate Text", text_rotate);
    }
    UI.end();

    if (text_rotate.val()) GG.dt() => text.rotateY;
}
