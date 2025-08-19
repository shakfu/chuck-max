//--------------------------------------------------------------------
// name: glines.ck
// desc: drawing lines with GLines
// 
// author: Andrew Zhu Aday
//   date: Fall 2024
//--------------------------------------------------------------------
// add GLines GGen to scene
GLines lines --> GG.scene();

[@(-0.5, -0.5), @(0.0, 0.0), @(0.5, -0.5)] @=> vec2 line_positions[];
[Color.RED] @=> vec3 line_colors[];
UI_Float2 ui_line_positions[0];
UI_Float3 ui_line_colors[0];

// populate line_positions and line_colors
for (int i; i < line_positions.size(); ++i) {
    ui_line_positions << new UI_Float2(line_positions[i]);
}
for (int i; i < line_colors.size(); ++i) {
    ui_line_colors << new UI_Float3(line_colors[i]);
}

// set line_positions and line_colors on geometry
line_positions => lines.positions;
line_colors => lines.colors;

// UI variables for line material params
UI_Float line_width(lines.width());
UI_Float3 line_color(lines.color());

fun void ui() {
    while (true) {
        GG.nextFrame() => now;
        if (UI.begin("Lines Example")) {
            if (UI.slider("line width", line_width, 0.01, 1)) {
                line_width.val() => lines.width;
            }

            if (UI.colorEdit("line color", line_color, 0)) {
                line_color.val() => lines.color;
            }

            // if (UI.slider("line extrusion", line_extrusion, 0, 1)) {
            //     line_extrusion.val() => lines.extrusion;
            // }

            // if (UI.checkbox("line loop", line_loop)) {
            //     line_loop.val() => lines.loop;   
            // }

            UI.separatorText("vertex positions");
            for (int i; i < line_positions.size(); i++) {
                UI.pushID(i);

                if (UI.drag("##point_pos", ui_line_positions[i], .01)) {
                    ui_line_positions[i].val() => line_positions[i];
                    line_positions => lines.positions;
                }

                UI.sameLine();
                if (UI.button("Remove##point_pos")) {
                    line_positions.erase(i);
                    ui_line_positions.erase(i);
                    line_positions => lines.positions;
                }

                UI.popID();
            }
            if (UI.button("Add Position")) {
                line_positions << @(0,0);
                ui_line_positions << new UI_Float2;
                line_positions => lines.positions;
            }


            UI.separatorText("vertex colors");

            for (int i; i < line_colors.size(); ++i) {
                UI.pushID(i);

                if (UI.colorEdit("##point_color", ui_line_colors[i], 0)) {
                    ui_line_colors[i].val() => line_colors[i];
                    line_colors => lines.colors;
                }

                UI.sameLine();
                if (UI.button("Remove##point_color")) {
                    line_colors.erase(i);
                    ui_line_colors.erase(i);
                    line_colors => lines.colors;
                }

                UI.popID();
            }

            if (UI.button("Add Color")) {
                line_colors << Color.BLACK;
                ui_line_colors << new UI_Float3(Color.BLACK);
                line_colors => lines.colors;
            }
        }
        UI.end();
    }
} spork ~ ui();

while (true) { GG.nextFrame() => now; }
