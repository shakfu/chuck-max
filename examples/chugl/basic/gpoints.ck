//-----------------------------------------------------------------------------
// name: gpoints.ck
// desc: stuff you can do with GPoints!
// requires: ChuGL + chuck-1.5.3.0 or higher
//
// author: Andrew Zhu Aday (https://ccrma.stanford.edu/~azaday/)
//         Ge Wang (https://ccrma.stanford.edu/~ge/)
// date: Fall 2023
//-----------------------------------------------------------------------------
GPoints points --> GG.scene();

[@(-1, -1, 0), @(1, -1, 0), @(0, 1, 0)] @=> vec3 point_positions[];
[Color.WHITE] @=> vec3 point_colors[];
[1.0] @=> float point_sizes[];

UI_Float3 ui_point_positions[0];
UI_Float3 ui_point_colors[0];
UI_Float ui_point_sizes[0];

// populate per-point attributes
for (int i; i < point_positions.size(); ++i) {
    ui_point_positions << new UI_Float3(point_positions[i]);
}
for (int i; i < point_colors.size(); ++i) {
    ui_point_colors << new UI_Float3(point_colors[i]);
}
for (int i; i < point_sizes.size(); ++i) {
    ui_point_sizes << new UI_Float(point_sizes[i]);
}


points.positions(point_positions);

// UI variables for line material params
UI_Float point_size(points.size());
UI_Float3 point_color(points.color());

while (true) {
    GG.nextFrame() => now;

    // UI
    if (UI.begin("GPoints")) {
        if (UI.slider("point size", point_size, 0.01, 1)) {
            point_size.val() => points.size;
        }

        if (UI.colorEdit("point color", point_color, 0)) {
            point_color.val() => points.color;
        }

        UI.separatorText("point positions");

        for (int i; i < point_positions.size(); i++) {
            UI.pushID(i);

            if (UI.drag("##point_pos", ui_point_positions[i], .01)) {
                ui_point_positions[i].val() => point_positions[i];
                point_positions => points.positions;
            }

            UI.sameLine();
            if (UI.button("Remove##point_pos")) {
                point_positions.erase(i);
                ui_point_positions.erase(i);
                point_positions => points.positions;
            }

            UI.popID();
        }

        if (UI.button("Add Position")) {
            point_positions << @(0,0, 0);
            ui_point_positions << new UI_Float3;
            point_positions => points.positions;
        }

        UI.separatorText("point colors");

        for (int i; i < point_colors.size(); i++) {
            UI.pushID(i);

            if (UI.colorEdit("##point_color", ui_point_colors[i], 0)) {
                ui_point_colors[i].val() => point_colors[i];
                point_colors => points.colors;
            }

            UI.sameLine();
            if (UI.button("Remove##point_color")) {
                point_colors.erase(i);
                ui_point_colors.erase(i);
                point_colors => points.colors;
            }

            UI.popID();
        }

        if (UI.button("Add Color")) {
            point_colors << @(1, 1, 1);
            ui_point_colors << new UI_Float3(@(1, 1, 1));
            point_colors => points.colors;
        }

        UI.separatorText("point sizes");

        for (int i; i < point_sizes.size(); i++) {
            UI.pushID(i);

            if (UI.slider("##point_sizes", ui_point_sizes[i], 0.01, 1.0)) {
                ui_point_sizes[i].val() => point_sizes[i];
                point_sizes => points.sizes;
            }

            UI.sameLine();

            if (UI.button("Remove##point_size")) {
                point_sizes.erase(i);
                ui_point_sizes.erase(i);
                point_sizes => points.sizes;
            }

            UI.popID();
        }

        if (UI.button("Add Size")) {
            point_sizes << 1;
            ui_point_sizes << new UI_Float(1);
            point_sizes => points.sizes;
        }
    }
    UI.end();
}