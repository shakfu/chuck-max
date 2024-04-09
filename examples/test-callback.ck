// example from https://github.com/DBraun/ChucKDesigner
global Event demo;
global Event hit;


me.dir() + "/wavs/amen.wav" => string filename;

fun void playDrums() {
    SndBuf buf => dac;
    filename => buf.read;

    // chuck enough time so that the buf plays
    2::second => now;

    demo.signal();
}

while( true ) {
    hit => now;
    spork ~ playDrums();
}
