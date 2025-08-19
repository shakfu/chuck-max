//-----------------------------------------------------------------------------
// name: soundbulb.ck
// desc: SoundBulb (Kunwoo's first 256a design in 2017) in ChuGL!
// 
// authors: Kunwoo Kim (https://ccrma.stanford.edu/~kunwoo/)
//         Andrew Zhu Aday (https://ccrma.stanford.edu/~azaday/)
// adapted from: sndpeek.ck by Ge Wang, Andrew, and Kunwoo
//         https://chuck.stanford.edu/chugl/examples/deep/sndpeek.ck
// date: Fall 2024
//-----------------------------------------------------------------------------

// ************** MAGIC NUMBERS ************** //
// ---- WAVEFROM ---- //
// window size (DEFAULT: 2048)
2048 => int WINDOW_SIZE;
// y position of waveform (DEFAULT: 0)
0 => float WAVEFORM_Y;
// width of waveform and spectrum display (DEFAULT: 1.35)
1.35 => float DISPLAY_WIDTH;
// Amplitude Pole
0.98 => float AMP_POLE;

// ---- SPECTRUM ---- //
// spectrum size (DEFAULT: = WINDOW_SIZE)
WINDOW_SIZE => int SPECTRUM_SIZE;
// spectrum frequency color map until (DEFAULT: 0.1 - 0Hz to 2kHz)
0.3 => float SPECTRUM_LIMIT;
// y position of spectrum (DEFAULT: 0)
0 => float SPECTRUM_Y;
// spectrum radius (DEFAULT: 1.6)
1.6 => float SPECTRUM_RADIUS;
// spectrum loop (DEFAULT: 67)
67 => float SPECTRUM_LOOP;

// ---- WATERFALL ---- //
// waterfall depth (DEFAULT: 128)
128 => int WATERFALL_DEPTH; 
// waveform interpolation (DEFAULT: 0.08)
0.08 => float WAVEFORM_LERP;
// spectrum width (DEFAULT: 0.009)
0.009 => float WATERFALL_INCREMENT;
// waterfall definition (DEFAULT: 2.28)
2.28 => float WATERFALL_CRISP;
// waterfall width (DEFAULT: 0.03)
0.03 => float WATERFALL_WIDTH;

// ---- ETC ---- //
// COLOR PALETTES
0 => int COLOR_MODE;
// mode switch lerp
0.01 => float MODE_LERP;

// ************** CHUGL SETTINGS ************** //
// ---- GENERAL ---- //
// window title
GWindow.title( "soundbulb" );
// camera stuff
GCamera camera --> GG.scene();
GG.scene().camera(camera);
camera.orthographic();
GG.scene().camera().posZ( 10 );

// ------ BLOOM ------ //
GG.renderPass() --> BloomPass bloom_pass --> GG.outputPass();
bloom_pass.input( GG.renderPass().colorOutput() );
GG.outputPass().input( bloom_pass.colorOutput() );

bloom_pass.intensity(0.71);
bloom_pass.radius(0.68);
bloom_pass.levels(9);

// ---- WAVEFORM ---- //
// waveform renderer
GLines waveform --> GG.scene(); waveform.width(0.005);
// translate up
waveform.posY( WAVEFORM_Y );
// color0
waveform.color( @(1, 1, 1) );

// ---- WATERFALL ---- //
// make a waterfall
Waterfall waterfall --> GG.scene();
// translate down
waterfall.posY( SPECTRUM_Y );

// ************** AUDIO SETTINGS ************** //
// node that will go into the audio analysis
Gain input;
// do we want to hear the input?
input => dac;

// --------------------------------------------------
// audio source to visualize (uncomment at least one)
// --------------------------------------------------
// *** microphone ***
// adc => input;
// *** sine sweep ***
spork ~ playSine();
// *** play a sound file ***
// spork ~ playBuf();
// ----------------------------------------------

fun void playSine()
{
    // sine
    SinOsc foo => input;
    // gain
    .25 => foo.gain;
    // time loop
    while( true )
    {
        // sweep the frequency
        2550 + Math.sin(now/second) * 2500 => foo.freq;
        // advance time
        1::ms => now;
    }
}

fun void playBuf()
{
    // audio
    SndBuf buffy => input;
    // load file
    "special:doh" => buffy.read;
    // time loop
    while( true )
    {
        // randomize playback rate
        Math.random2f(.25,4) => buffy.rate;
        // set playhead to beginning
        0 => buffy.pos;
        // advance time
        600::ms => now;
    }
}

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

// audio graph
input => Gain g => OnePole p => blackhole;
// square the input
input => g;
// multiply
3 => g.op;
AMP_POLE => p.pole;


// ************** VARIABLES ************** //
// spectrum color maps
vec3 spectrumColorMap[6];
vec3 newColorMap[6];
// sample array
float samples[0];
// FFT response
complex response[0];
// a vector to hold positions
vec2 positions[WINDOW_SIZE];
vec2 specPositions[SPECTRUM_SIZE];

// previous waveform array
float prev_samples[WINDOW_SIZE];
// current waveform array
float curr_samples[WINDOW_SIZE];

// ************** FUNCTIONS ************** //
// map audio buffer to 3D positions
fun void map2waveform( float in[], vec2 out[] )
{
    if( in.size() != out.size() )
    {
        <<< "size mismatch in map2waveform()", "" >>>;
        return;
    }

    // get volume and scale
    0.1 + Math.pow(p.last(), .5) => float env;
    waveform.color( @(1, 1, 1) * Math.pow(env, 2) * 15.0 );
    
    DISPLAY_WIDTH => float width;
    for( 0 => int i; i < in.size(); i++)
    {
        prev_samples[i] + (in[i] - prev_samples[i]) * WAVEFORM_LERP => curr_samples[i];
        
        // space evenly in X
        -width/2 + width/WINDOW_SIZE*i => out[i].x;
        // map y, using window function to taper the ends
        curr_samples[i] * Math.sqrt(window[i] * 2.5) => out[i].y;
        
        curr_samples[i] => prev_samples[i];
    }
}

// map FFT output to 3D positions
fun void map2spectrum( complex in[], vec2 out[] )
{
    vec2 currentPos[SPECTRUM_SIZE];
    float magnitude[SPECTRUM_SIZE];
    
    if( in.size() != out.size() )
    {
        <<< "size mismatch in map2spectrum()", "" >>>;
        return;
    }
    
    // mapping to xyz coordinate
    DISPLAY_WIDTH => float width;
    for( 0 => int i; i < SPECTRUM_SIZE; i++ )
    {
        // space evenly in X
        -width/2 + width/SPECTRUM_SIZE*i => currentPos[i].x;
        // map frequency bin magnitide in Y        
        Math.sqrt( (in[i]$polar).mag * 0.2 ) => currentPos[i].y;
        
        // change to polar
        SPECTRUM_RADIUS * Math.cos(SPECTRUM_LOOP * Math.PI * i / (SPECTRUM_SIZE - 1) - Math.PI/2) => out[i].x;
        (SPECTRUM_RADIUS + currentPos[i].y) * Math.sin(SPECTRUM_LOOP * Math.PI * i / (SPECTRUM_SIZE - 1) - Math.PI/2) => out[i].y;
        
        (in[i]$polar).mag => magnitude[i];
    }

    waterfall.latest( out, magnitude );
}

// custom GGen to render waterfall
class Waterfall extends GGen
{
    // waterfall playhead
    0 => int playhead;
    // spectrum color mapping
    vec3 spectrumColor[SPECTRUM_SIZE];
    // lines
    GLines wfl[WATERFALL_DEPTH];
    // iterate over line GGens
    for( GLines w : wfl )
    {
        // aww yea, connect each GLine as a child of this GGen
        w --> this;
        // color
        spectrumColor => w.colors;
        // width
        w.width( WATERFALL_WIDTH );
    }

    // copy
    fun void latest( vec2 positions[], float mag[] )
    {
        // set into
        positions => wfl[playhead].positions;
        // set color dynamic
        Math.ceil(1.0 * SPECTRUM_SIZE * SPECTRUM_LIMIT / (spectrumColorMap.size() - 1))$int => int spectrumInterval;
        for( 0 => int i; i < spectrumColorMap.size() - 1; i++ )
        {   
            1.0 * (spectrumColorMap[i + 1] - spectrumColorMap[i])/spectrumInterval => vec3 colorLerp;
            for( int j; j < spectrumInterval; j++ )
            {
                if( (i * spectrumInterval + j) < SPECTRUM_SIZE )
                    spectrumColorMap[i] + colorLerp * j => spectrumColor[i * spectrumInterval + j];
            }
        }
        
        for( (SPECTRUM_SIZE * SPECTRUM_LIMIT)$int - 1 => int i; i < SPECTRUM_SIZE; i++)
        {
            spectrumColorMap[spectrumColorMap.size() - 1] => spectrumColor[i];
        }
        
        vec3 specColors[SPECTRUM_SIZE];
        for (int i; i < SPECTRUM_SIZE; i++)
        {
            100000 * Math.pow(mag[i], WATERFALL_CRISP) * spectrumColor[i] => specColors[i];
        }
        specColors => wfl[playhead].colors;
        
        // advance playhead
        playhead++;
        // wrap it
        WATERFALL_DEPTH %=> playhead;
    }

    fun void update( float dt )
    {
        // position
        playhead => int pos;
        // for color
        WATERFALL_DEPTH/4 => float thresh;
        // depth
        WATERFALL_DEPTH - thresh => float fadeChunk;
        // so good
        for( int i; i < wfl.size(); i++ )
        {
            // start with playhead-1 and go backwards
            pos--; if( pos < 0 ) WATERFALL_DEPTH-1 => pos;
            // offset X and Y
            wfl[pos].scaX(1.0 + i * WATERFALL_INCREMENT);
            wfl[pos].scaY(1.0 + i * WATERFALL_INCREMENT);
            // width dynamic
            wfl[pos].width( WATERFALL_WIDTH );
            
            if( i > thresh )
            {
                Color.WHITE * Math.pow((fadeChunk - (i - thresh))/ fadeChunk, 3) => wfl[pos].color;
            }
            else
            {
                Color.WHITE * Math.pow( 1.0 - (thresh - i) / thresh, 2) => wfl[pos].color;
            }
        }
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
        // upchuck to take FFT, get magnitude reposne
        fft.upchuck();
        // get spectrum (as complex values)
        fft.spectrum( response );
        // jump by samples
        WINDOW_SIZE::samp/2 => now;
    }
}
spork ~ doAudio();

// **************** GRAPHICAL THINGS **************** //

// segments per line
200 => int numSegments;
// ------- LightBulb Glass Outline ------- //
3 => int numOutline;
1.0 => float outlineRadius;
GLines glassOutline[numOutline];
vec2 glassOutlinePos[numSegments];
vec3 glassOutlineColor[numSegments];

for( auto s : glassOutline )
{
    s --> GG.scene(); s.width(0.005);
}

// color mapping
// [@(1,0,0), @(1,0.5,0), @(1,1,0), @(0,1,0), @(0,0,1), @(1,0,1)] @=> vec3 glassOutlineColorMap[];
vec3 glassOutlineColorMap[spectrumColorMap.size()];

fun void createOutline()
{
    for (0 => int j; j < numOutline; j++)
    {
        for (0 => int i; i < numSegments; i++)
        {
            1.65 * Math.PI * ((1.0 * i) / (numSegments - 1)) + 0.675 * Math.PI => float angle;
            outlineRadius * Math.cos(angle - Math.PI) + (0.01 * (1 - j)) => glassOutlinePos[i].x;
            1.1 * outlineRadius * Math.sin(angle - Math.PI + (0.02 * (1 - j))) => glassOutlinePos[i].y;
        }
        glassOutline[j].positions(glassOutlinePos);
        glassOutlineColor => glassOutline[j].colors;
    }
}
createOutline();

fun void outlineSwitchColor()
{
    while (true)
    {
        for (int i; i < spectrumColorMap.size(); i++)
        {
            spectrumColorMap[spectrumColorMap.size() - 1 - i] => glassOutlineColorMap[i];
        }

        Math.ceil(1.0 * numSegments/(glassOutlineColorMap.size() - 1))$int => int interval;

        for (0 => int i; i < glassOutlineColorMap.size() - 1; i++)
        {
            1.0 * (glassOutlineColorMap[i + 1] - glassOutlineColorMap[i])/interval => vec3 colorLerp;
            for (int j; j < interval; j++)
            {
                glassOutlineColorMap[i] + colorLerp * j => glassOutlineColor[i * interval + j];
            }
        }
        
        for (0 => int j; j < numOutline; j++)
        {
            glassOutlineColor => glassOutline[j].colors;
        }
        10::ms => now;
    }
}
spork ~ outlineSwitchColor();

// ------- LightBulb Filament Holder ------- //
2 => int numHolder;
0.62 => float FH_WIDTH_RATIO;
200 => int numFHSegments;
vec2 FH_startPos[numHolder];
vec2 FH_endPos[numHolder];

GLines filamentHolder[numHolder];
vec2 FH_Pos[numFHSegments];
for( auto s : filamentHolder )
{
    s --> GG.scene(); s.width(0.005);
}

fun void createHolder()
{   
    for (0 => int j; j < numHolder; j++)
    {
        -DISPLAY_WIDTH / 2.0 + DISPLAY_WIDTH * j => FH_startPos[j].x;
        WAVEFORM_Y => FH_startPos[j].y;
        
        FH_WIDTH_RATIO * FH_startPos[j].x => FH_endPos[j].x;
        WAVEFORM_Y - (DISPLAY_WIDTH) => FH_endPos[j].y;
        
        for (0 => int i; i < numFHSegments; i++)
        {
            FH_startPos[j].x + (FH_endPos[j].x - FH_startPos[j].x) * (1.0 * i / (numSegments - 1)) => FH_Pos[i].x;
            FH_startPos[j].y + (FH_endPos[j].y - FH_startPos[j].y) * (1.0 * i / (numSegments - 1)) => FH_Pos[i].y;
        }
        filamentHolder[j].positions(FH_Pos);
    }
}
createHolder();

fun void holderSwitchColor()
{
    while( true )
    {
         // color mapping
        [spectrumColorMap[0]] => filamentHolder[0].colors;
        [spectrumColorMap[spectrumColorMap.size()-1]] => filamentHolder[1].colors;
        10::ms => now;
    }
}
spork ~ holderSwitchColor();

// ------- LightBulb Base ------- //
GLines lightBulbBase --> GG.scene(); lightBulbBase.width(0.002);
100 => int numBaseSegments;
vec2 base_Pos[numBaseSegments];
vec2 base_startPos;
FH_WIDTH_RATIO * DISPLAY_WIDTH => float BASE_WIDTH;

100 => float baseFreq;
-0.004 => float baseYOffset;

// color mapping
vec3 baseColor[numBaseSegments];
[@(1,1,1), @(0.5,0.5,0.5), @(0,0,0)] @=> vec3 baseColorMap[];
Math.ceil(1.0 * numBaseSegments/(baseColorMap.size() - 1))$int => int baseInterval;

for (0 => int i; i < baseColorMap.size() - 1; i++)
{
    1.0 * (baseColorMap[i + 1] - baseColorMap[i])/baseInterval => vec3 colorLerp;
    for (int j; j < baseInterval; j++)
    {
        Math.pow(baseColorMap[i].x + colorLerp.x * j, 2) => baseColor[i * baseInterval + j].x;
        Math.pow(baseColorMap[i].y + colorLerp.y * j, 2) => baseColor[i * baseInterval + j].y;
        Math.pow(baseColorMap[i].z + colorLerp.z * j, 2) => baseColor[i * baseInterval + j].z;
    }
}

baseColor => lightBulbBase.colors;

fun void createBase()
{
    (FH_endPos[0].x + FH_endPos[1].x) / 2.0 => base_startPos.x;
    FH_endPos[0].y => base_startPos.y;
    for (0 => int i; i < numBaseSegments; i++)
    {
        base_startPos.x + (BASE_WIDTH * (1.0 - i * 0.002)) / 2.0 * Math.cos(Math.PI / 180.0 * baseFreq * i) => base_Pos[i].x;
        FH_endPos[0].y + baseYOffset * i => base_Pos[i].y;
    }
    lightBulbBase.positions(base_Pos);
}
createBase();

// -------------- COLOR MODES ---------------- //
fun void switchColorMode()
{
    -1 => int current_mode;
    WATERFALL_WIDTH => float newWidth;
    
    while (true)
    {
        if (COLOR_MODE != current_mode)
        {
            COLOR_MODE => current_mode;
        }
        
        // various color modes
        if (COLOR_MODE == 0)
        {
            [@(1,0,1), @(0,0,1), @(0,1,0), @(1,1,0), @(1,0.5,0), @(1,0,0)] @=> newColorMap;
            Math.random2f(0.025, 0.035) => newWidth;
        }
        else if (COLOR_MODE == 1)
        {
            [@(232,54,101)/255.0 * 4.0, @(235,111,33)/255.0 * 4.0, @(255,253,122)/255.0 * 4.0, @(122,249,121)/255.0 * 4.0, @(232,42,232)/255.0 * 4.0, @(232,42,232)/255.0 * 4.0] @=> newColorMap;
            Math.random2f(0.1, 0.2) => newWidth;
        }
        else if (COLOR_MODE == 2)
        {
            [@(80,244,251)/255.0, @(160,200,43)/255.0, @(245,116,115)/255.0, @(253,148,185)/255.0, @(255,255,255)/255.0, @(37,38,68)/255.0] @=> newColorMap;
            Math.random2f(0.005, 0.015) => newWidth;
        }
        else if (COLOR_MODE == 3)
        {
            [@(254,248,58)/255.0 * 4.0, @(216,82,245)/255.0 * 4.0, @(82,239,254)/255.0 * 4.0, @(156,254,126)/255.0 * 4.0, @(249,78,67)/255.0 * 4.0, @(58,61,196)/255.0 * 4.0] @=> newColorMap;
            Math.random2f(0.15, 0.025) => newWidth;
        }
        else if (COLOR_MODE == 4)
        {
            [@(1,1,1), @(0.81,0.81,0.81), @(0.64,0.64,0.64), @(0.49,0.49,0.49), @(0.36,0.36,0.36), @(0.25,0.25,0.25)] @=> newColorMap;
            Math.random2f(0.0001, 0.2) => newWidth;
        }
        
        for (int i; i < spectrumColorMap.size(); i++)
        {
            (newColorMap[i] - spectrumColorMap[i]) * MODE_LERP +=> spectrumColorMap[i];
            (newWidth - WATERFALL_WIDTH) * MODE_LERP / 5.0 +=> WATERFALL_WIDTH;
        }
        10::ms => now;
    }
}
spork ~ switchColorMode();


// ----------------------- KEYBOARD INPUT ------------------------- //
false => int fullscreen;
fun void keyboardInput()
{
    if( GWindow.keyDown(GWindow.Key_1))
        0 => COLOR_MODE;
    else if( GWindow.keyDown(GWindow.Key_2))
        1 => COLOR_MODE;
    else if( GWindow.keyDown(GWindow.Key_3))
        2 => COLOR_MODE;
    else if( GWindow.keyDown(GWindow.Key_4))
        3 => COLOR_MODE;
    else if( GWindow.keyDown(GWindow.Key_5))
        4 => COLOR_MODE;
    else if( GWindow.keyDown(GWindow.Key_Space)) {
        if (fullscreen) {
            false => fullscreen;
            GG.windowed();
        }
        else {
            true => fullscreen;
            GG.fullscreen();
        }
    }
}

// ***************** graphics render loop ***************** //
while( true )
{
    // handle input
    keyboardInput();
    // map to interleaved format
    map2waveform( samples, positions );
    // set the mesh position
    waveform.positions( positions );
    // map to spectrum display
    map2spectrum( response, specPositions );
    // next graphics frame
    GG.nextFrame() => now;
}
