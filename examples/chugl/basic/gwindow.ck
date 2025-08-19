//--------------------------------------------------------------------
// name: gwindow.ck
// desc: window management demo
//       (the screen is blank; see below for interactions)
// requires: ChuGL + chuck-1.5.3.0 or higher
//
// press '1' to go fullscreen.
// press '2' to go windowed.
// press '3' to maximize (windowed fullscreen).
// press '4' to iconify / restore
// press '5' to toggle window opacity
// press '6' to toggle mouse mode (normal - hidden - disabled)
//
// listens for keyboard input on space key
// 
// author: Andrew Zhu Aday
//   date: June 2024
//--------------------------------------------------------------------

// disable <esc> and close button; allows you to manually handle close
// events and perform any necessary cleanup, e.g. saving game state
GWindow.closeable( false ); 

// uncomment to set size limits
// GWindow.sizeLimits(100, 100, 1920, 1080, @(16, 9));

GWindow.windowed(1200, 675);
GWindow.center(); // center window on screen
GWindow.title("GWindow Demo");

// window configuration (must be called BEFORE first GG.nextFrame())
true => GWindow.transparent;
true => GWindow.floating;
// false => GWindow.decorated;
// false => GWindow.resizable;

fun void mouseListener() {
    while (true) {
        GG.nextFrame() => now;

        if (GWindow.mouseLeft()) {
            <<< "left mouse button held" >>>;
        }

        if (GWindow.mouseRight()) {
            <<< "right mouse button held" >>>;
        }

        if (GWindow.mouseLeftDown()) {
            <<< "left mouse button pressed" >>>;
        }
        if (GWindow.mouseRightDown()) {
            <<< "right mouse button pressed" >>>;
        }
        if (GWindow.mouseLeftUp()) {
            <<< "left mouse button released" >>>;
        }
        if (GWindow.mouseRightUp()) {
            <<< "right mouse button released" >>>;
        }
    }
} spork ~ mouseListener();

fun void kbListener() {
    while (true) {
        GG.nextFrame() => now;
        if (GWindow.key(GWindow.Key_Space)) {
            <<< "space key held" >>>;
        }

        if (GWindow.keyDown(GWindow.Key_Space)) {
            <<< "space key pressed" >>>;
        }

        if (GWindow.keyUp(GWindow.Key_Space)) {
            <<< "space key released" >>>;
        }
    }
} spork ~ kbListener();

fun void closeCallback() {
    while (1) {
        GWindow.closeEvent() => now;
        <<< "user tried to close window" >>>;
        GWindow.close(); // allow close 
    }
} spork ~ closeCallback();

fun void resizeCallback() {
    while (1) {
        GWindow.resizeEvent() => now;
        <<< "user resized window | window size: ", GWindow.windowSize(), " | framebuffer size: ", GWindow.framebufferSize() >>>;
    }
} spork ~ resizeCallback();

fun void contentScaleCallback() {
    while (1) {
        GWindow.contentScaleEvent() => now;
        <<< "content scale changed | content scale: ", GWindow.contentScale() >>>;
    }
} spork ~ contentScaleCallback();

int fc;
true => int opaque;
false => int floating;
0 => int mouse_mode;

while (1) { 
    GG.nextFrame() => now;
    if (UI.isKeyPressed(UI_Key.Num1)) GWindow.fullscreen();
    if (UI.isKeyPressed(UI_Key.Num2)) {
        GWindow.windowed(800, 600);
        GWindow.center();
    }
    if (UI.isKeyPressed(UI_Key.Num3)) {
        // disable size limits
        GWindow.sizeLimits(0, 0, 0, 0, @(0, 0));
        GWindow.maximize();
    }
    if (UI.isKeyPressed(UI_Key.Num4)) {
        GWindow.iconify();
        repeat (60) {
            GG.nextFrame() => now;
        }
        GWindow.restore();
    }
    if (UI.isKeyPressed(UI_Key.Num5)) {
        if (opaque) {
            GWindow.opacity(0.4);
            false => opaque;
        } else {
            GWindow.opacity(1.0);
            true => opaque;
        }
    }
    if (UI.isKeyPressed(UI_Key.Num6, false)) {
        (mouse_mode + 1) % 3 => mouse_mode;
        if (mouse_mode == 0) {
            <<< "mouse mode: normal" >>>;
        } else if (mouse_mode == 1) {
            <<< "mouse mode: hidden" >>>;
        } else {
            <<< "mouse mode: disabled" >>>;
        }
        GWindow.mouseMode(mouse_mode);
    }

    GWindow.title("GWindow Demo | Frame: " + (++fc));
}
