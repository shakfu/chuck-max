// Host.sendBang/Float/Symbol/List(receiver, ...) -> every [receive <receiver>],
// as [send] does; no patch cord to chuck~ needed.
// in Max: try [r tick], [r freq], [r range] and [r pair], each into [print]

SinOsc s => dac;
0.2 => s.gain;

while( true )
{
    Math.random2f( 200, 800 ) => float f => s.freq;
    Host.sendBang( "tick" );
    Host.sendFloat( "freq", f );
    Host.sendSymbol( "range", f < 500 ? "low" : "high" );
    Host.sendList( "pair", [f, s.gain()] );
    250::ms => now;
}
