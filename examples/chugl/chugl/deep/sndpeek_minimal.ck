//-----------------------------------------------------------------------------
// name: sndpeek-minimal.ck
// desc: sndpeek in ChuGL (minimal version; no waterfall)
// 
// author: Ge Wang (https://ccrma.stanford.edu/~ge/)
//         Andrew Zhu Aday (https://ccrma.stanford.edu/~azaday/)
//         (https://https://kunwookim.com/)
// date: Fall 2024
//-----------------------------------------------------------------------------

// window size
1024 => int WINDOW_SIZE;
// y position of waveform
2 => float WAVEFORM_Y;
// width of waveform and spectrum display
10 => float DISPLAY_WIDTH;

// window title
GWindow.title( "sndpeek (minimal version)" );
// uncomment to fullscreen
GWindow.fullscreen();
// position camera
GG.scene().camera().posZ(8.0);

// waveform renderer
GLines waveform --> GG.scene(); waveform.width(.01);
// translate up
waveform.posY(WAVEFORM_Y);
// color0
waveform.color( @(.4, .4, 1) );

// spectrum renderer
GLines spectrum --> GG.scene(); spectrum.width(.01);
// translate down
spectrum.posY(-WAVEFORM_Y);
// color0
spectrum.color( @(.4, 1, .4) );

// accumulate samples from mic
adc => Flip accum => blackhole;
// take the FFT
adc => PoleZero dcbloke => FFT fft => blackhole;
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
vec2 positions[WINDOW_SIZE];

// map audio buffer to 3D positions
fun void map2waveform( float in[], vec2 out[] )
{
    if( in.size() != out.size() )
    {
        <<< "size mismatch in map2waveform()", "" >>>;
        return;
    }
    
    // mapping to xyz coordinate
    DISPLAY_WIDTH => float width;
    for (int i; i < in.size(); i++)
    {
        // space evenly in X
        -width/2 + width/WINDOW_SIZE*i => out[i].x;
        // map y, using window function to taper the ends
        in[i] * 2 * window[i] => out[i].y;
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
    DISPLAY_WIDTH => float width;
    for (int i; i < in.size(); i++)
    {
        // space evenly in X
        -width/2 + width/WINDOW_SIZE*i => out[i].x;
        // map frequency bin magnitide in Y
        25 * Math.sqrt( (in[i]$polar).mag ) => out[i].y;
    }
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
        // upchuck to take FFT, get magnitude response
        fft.upchuck();
        // get spectrum (as complex values)
        fft.spectrum( response );
        // jump by samples
        WINDOW_SIZE::samp/2 => now;
    }
}
spork ~ doAudio();

// graphics render loop
while( true )
{
    // next graphics frame
    GG.nextFrame() => now;
    // map to interleaved format
    map2waveform( samples, positions );
    // set the mesh position
    waveform.positions( positions ); // chugl
    // map to spectrum display
    map2spectrum( response, positions );
    // set the mesh position
    spectrum.positions( positions ); // chugl
}