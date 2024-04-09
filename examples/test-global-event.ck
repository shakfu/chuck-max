// example from https://github.com/DBraun/ChucKDesigner
global Event pulse;

me.dir() + "/wavs/amen.wav" => string filename;

fun void playImpact() {
    SndBuf buf => dac;
    filename => buf.read;

    // chuck enough time so that the buf plays
    1::second => now; 
}

while( true ) {
    pulse => now;
    spork ~ playImpact();
}
