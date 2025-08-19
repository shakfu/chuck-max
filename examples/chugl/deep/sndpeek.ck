//-----------------------------------------------------------------------------
// name: sndpeek.ck
// desc: sndpeek in ChuGL!
// 
// author: Ge Wang (https://ccrma.stanford.edu/~ge/)
//         Andrew Zhu Aday (https://ccrma.stanford.edu/~azaday/)
//         Kunwoo Kim (https://https://kunwookim.com/)
// date: Fall 2023
//-----------------------------------------------------------------------------

// window size
1024 => int WINDOW_SIZE;
// y position of waveform
2 => float WAVEFORM_Y;
// y position of spectrum
-2.5 => float SPECTRUM_Y;
// width of waveform and spectrum display
10 => float DISPLAY_WIDTH;
// waterfall depth
64 => int WATERFALL_DEPTH;

// window title
GWindow.title( "sndpeek" );
// uncomment to fullscreen
GWindow.fullscreen();
// position camera
GG.scene().camera().posZ(8.0);

// waveform renderer
GLines waveform --> GG.scene(); waveform.width(.01);
// translate up
waveform.posY( WAVEFORM_Y );
// color0
waveform.color( @(.4, .4, 1)*1.5 );

// make a waterfall
Waterfall waterfall --> GG.scene();
// translate down
waterfall.posY( SPECTRUM_Y );

// which input?
adc => Gain input;
// SinOsc sine => Gain input => dac; .15 => sine.gain;
// accumulate samples from mic
input => Flip accum => blackhole;
// take the FFT
input => PoleZero dcbloke => FFT fft => blackhole;
// set DC blocker
.95 => dcbloke.blockZero;
// set size of flip
WINDOW_SIZE => accum.size;
// set window type and size
Windowing.hann(WINDOW_SIZE) => fft.window;
// set FFT size (will automatically zero pad)
WINDOW_SIZE*2 => fft.size;
// get a reference for our window for visual tapering of the waveform
Windowing.hann(WINDOW_SIZE) @=> float window[];

// sample array
float samples[0];
// FFT response
complex response[0];
// a vector to hold positions
vec2 positions[WINDOW_SIZE];

// custom GGen to render waterfall
class Waterfall extends GGen
{
    // waterfall playhead
    0 => int playhead;
    // lines
    GLines wfl[WATERFALL_DEPTH];
    // color
    @(.4, 1, .4) => vec3 color;

    // iterate over line GGens
    for( GLines w : wfl )
    {
        // aww yea, connect as a child of this GGen
        w --> this;
        // line width
        w.width(.01);
        // color
        w.color( @(.4, 1, .4) );
    }

    // copy
    fun void latest( vec2 positions[] )
    {
        // set into
        positions => wfl[playhead].positions;
        // advance playhead
        playhead++;
        // wrap it
        WATERFALL_DEPTH %=> playhead;
    }

    // update
    fun void update( float dt )
    {
        // position
        playhead => int pos;
        // so good
        for( int i; i < wfl.size(); i++ )
        {
            // start with playhead-1 and go backwards
            pos--; if( pos < 0 ) WATERFALL_DEPTH-1 => pos;
            // offset Z
            wfl[pos].posZ( -i );
            // set fade
            wfl[pos].color( color * Math.pow(1.0 - (i$float / WATERFALL_DEPTH), 4) );
        }
    }
}

// keyboard controls and getting audio from dac
fun void kbListener()
{
    SndBuf buf => dac;
    .0 => buf.gain;
    "special:dope" => buf.read;
    while (true) {
        GG.nextFrame() => now;
        if (UI.isKeyPressed(UI_Key.Space, false)) {
            .3 => buf.gain;
            0 => buf.pos;
        }
    }
} 
spork ~ kbListener();

// map audio buffer to 3D positions
fun void map2waveform( float in[], vec2 out[] )
{
    if( in.size() != out.size() )
    {
        <<< "size mismatch in map2waveform()", "" >>>;
        return;
    }
    
    // mapping to xyz coordinate
    int i;
    DISPLAY_WIDTH => float width;
    for( auto s : in )
    {
        // space evenly in X
        -width/2 + width/WINDOW_SIZE*i => out[i].x;
        // map y, using window function to taper the ends
        s*2 * window[i] => out[i].y;
        // increment
        i++;
    }
}

// map FFT output to 3D positions
fun void map2spectrum( complex in[], vec2 out[] )
{
    if( in.size() != out.size() )
    {
        <<< "size mismatch in map2spectrum()", "" >>>;
        return;
    }
    
    // mapping to xyz coordinate
    int i;
    DISPLAY_WIDTH => float width;
    for( auto s : in )
    {
        // space evenly in X
        -width/2 + width/WINDOW_SIZE*i => out[i].x;
        // map frequency bin magnitide in Y        
        5 * Math.sqrt( (s$polar).mag * 25 ) => out[i].y;
        // increment
        i++;
    }

    waterfall.latest( out );
}

// do audio stuff
fun void doAudio()
{
    while( true )
    {
        // upchuck to process accum
        accum.upchuck();
        // get the last window size samples (waveform)
        accum.output( samples );
        // upchuck to take FFT, get magnitude reposne
        fft.upchuck();
        // get spectrum (as complex values)
        fft.spectrum( response );
        // jump by samples
        WINDOW_SIZE::samp/2 => now;
    }
}
spork ~ doAudio();

// fps printer
fun void printFPS( dur howOften )
{
    while( true )
    {
        <<< "fps:", GG.fps() >>>;
        howOften => now;
    }
}
spork ~ printFPS(.25::second);

fun void controlSine( Osc s )
{
    while( true )
    {
        100 + (Math.sin(now/second*5)+1)/2*20000 => s.freq;
        10::ms => now;
    }
}
// spork ~ controlSine( sine );

// graphics render loop
while( true )
{
    // map to interleaved format
    map2waveform( samples, positions );
    // set the mesh position
    waveform.positions( positions );
    // map to spectrum display
    map2spectrum( response, positions );
    // next graphics frame
    GG.nextFrame() => now;
}
