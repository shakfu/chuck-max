//--------------------------------------------------------------------
// name: jello.ck
// desc: using microphone input to modulate cube size and rotation
// requires: ChuGL + chuck-1.5.3.0 or higher
// 
// author: Andrew Zhu Aday
//   date: Fall 2024
//--------------------------------------------------------------------

// audio setup (envelope follower) =====================
adc => Gain g => OnePole p => blackhole;
// square the input
adc => g;
// multiply
3 => g.op;
// filter pole position
0.9999 => p.pole;

// scenegraph setup ====================================
GCube cube --> GG.scene();

// Game Loop ===========================================
while (true)
{
    // render
    GG.nextFrame() => now;
    // map audio output to cube scale
    20*p.last() + 1 => cube.sca;
    // map audio output to rotation
    10*p.last() => cube.rotateZ;
    // rotate
    GG.dt() => cube.rotateX;
}
