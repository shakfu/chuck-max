// sound file
// me.dir() gets the directory of the chuck script!
me.dir() + "../data/amen.wav" => string filename;
if( me.args() > 0 ) me.arg(0) => filename;

// the patch 
SndBuf buf => dac;
// load the file
filename => buf.read;

// time loop
while( true )
{
    0 => buf.pos;
    3::second => now;
}
