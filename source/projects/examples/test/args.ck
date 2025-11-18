
// print number of args
<<< "number of arguments:", me.args() >>>;

// print each
for( int i; i < me.args(); i++ )
{
    <<< "   ", me.arg(i) >>>;
}

0 => int start;

me.dir() + "../data/kick.wav" => string filename;

if( me.args() > 0 ) {
	me.dir() + "../data/" + me.arg(0) => filename;
	if (me.args() > 1) {
		Std.atoi(me.arg(1)) => start;
	}
} 

// the patch 
SndBuf buf => dac;
// load the file
filename => buf.read;

// time loop
while( true )
{
    start => buf.pos;
    3::second => now;
}
