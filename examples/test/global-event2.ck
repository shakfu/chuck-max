// example of a custom global event

class MyEvent extends Event
{
    int value;
}

// example from https://github.com/DBraun/ChucKDesigner
global MyEvent pulse;

me.dir() + "../data/amen.wav" => string filename;

fun void playImpact(MyEvent p) {
    SndBuf buf => dac;
    filename => buf.read;

    <<<"length: ", p.value>>>;
    // chuck enough time so that the buf plays
    p.value::second => now; 
}

while( true ) {
    pulse => now;
    Math.random2( 0, 3 ) => pulse.value;
    spork ~ playImpact(pulse);
}

