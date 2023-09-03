//-----------------------------------------------------------------------------
// name: chuck-embed
// desc: hello sine wave, real-time
//       (example of how to embed chuck core)
//
// author: Jack Atherton (lja@ccrma.stanford.edu)
//         Ge Wang (ge@ccrma.stanford.edu)
//   date: 2014-2020
//   uses: RtAudio by Gary Scavone
//-----------------------------------------------------------------------------
#include "RtAudio/RtAudio.h"
#include "chuck.h"
#include <math.h>
#include <stdlib.h>
#include <unistd.h> // for usleep
#include <iostream>
using namespace std;


// our datetype
#define SAMPLE float
// corresponding format for RtAudio
#define MY_FORMAT RTAUDIO_FLOAT32
// sample rate
#define MY_SRATE 44100
// number of channels
#define MY_CHANNELS 2

// the chuck
ChucK * the_chuck = NULL;




//-----------------------------------------------------------------------------
// name: callme()
// desc: audio callback
//-----------------------------------------------------------------------------
int callme( void * outputBuffer, void * inputBuffer, unsigned int numFrames,
            double streamTime, RtAudioStreamStatus status, void * data )
{
    // cast!
    SAMPLE * input = (SAMPLE *)inputBuffer;
    SAMPLE * output = (SAMPLE *)outputBuffer;
    
    // --------------------------- ChucK -------------------------------- //
    // compute chuck!
    the_chuck->run( input, output, numFrames );
    // ------------------------------------------------------------------- //
    
    return 0;
}




//-----------------------------------------------------------------------------
// name: main()
// desc: entry point
//-----------------------------------------------------------------------------
int main( int argc, char ** argv )
{
    // instantiate RtAudio object
    RtAudio audio;
    // variables
    unsigned int bufferBytes = 0;
    // frame size
    unsigned int bufferFrames = 1024;
    
    // check for audio devices
    if( audio.getDeviceCount() < 1 )
    {
        // nopes
        cout << "no audio devices found!" << endl;
        exit( 1 );
    }
    
    // let RtAudio print messages to stderr.
    audio.showWarnings( true );
    
    // set input and output parameters
    RtAudio::StreamParameters iParams, oParams;
    iParams.deviceId = audio.getDefaultInputDevice();
    iParams.nChannels = MY_CHANNELS;
    iParams.firstChannel = 0;
    oParams.deviceId = audio.getDefaultOutputDevice();
    oParams.nChannels = MY_CHANNELS;
    oParams.firstChannel = 0;
    
    // create stream options
    RtAudio::StreamOptions options;
    
    // go for it
    try {
        // open a stream
        audio.openStream( &oParams, &iParams, MY_FORMAT, MY_SRATE, &bufferFrames, &callme, (void *)&bufferBytes, &options );
    }
    catch( RtError& e )
    {
        // error!
        cout << e.getMessage() << endl;
        exit( 1 );
    }
    
    
    // --------------------------- ChucK -------------------------------- //
    // set up chuck
    the_chuck = new ChucK();
    // set sample rate and number of in/out channels on our chuck
    the_chuck->setParam( CHUCK_PARAM_SAMPLE_RATE, MY_SRATE );
    the_chuck->setParam( CHUCK_PARAM_INPUT_CHANNELS, MY_CHANNELS );
    the_chuck->setParam( CHUCK_PARAM_OUTPUT_CHANNELS, MY_CHANNELS );
    the_chuck->setParam( CHUCK_PARAM_WORKING_DIRECTORY, "." );
    
    // initialize our chuck
    the_chuck->init();

    // run chuck code!
    if( argc > 1 )
    {
        // print message
        cerr << "chuck-embed example generating code from command line arg '" << argv[1] << "'..." << endl;

        // construct a program using the first command line argument
        std::stringstream program;
        program << "SinOsc foo => dac; \n";
        program << ".5 => foo.gain; \n";
        program << "\"" << argv[1] << "\" @=> string arg; \n";
        program << "while( true ) \n";
        program << "{ \n";
        program << "    for( int i; i < arg.length(); i++ ) \n";
        program << "    { \n";
        program << "        Std.scalef( arg.charAt(i), 0, 128, 50, 80 ) => Std.mtof => foo.freq; \n";
        program << "        200::ms => now; \n";
        program << "    } \n";
        program << "} \n";
        
        // print the program
        cerr << "------------------" << endl;
        cerr << program.str();
        cerr << "------------------" << endl;

        // compile the code from string
        the_chuck->compileCode( program.str(), "" );
    }
    else
    {
        // print message
        cerr << "chuck-embed example running 'sine.ck'..." << endl;
        // run a chuck program from file
        the_chuck->compileFile( "sine.ck", "", 1 );
    }
    // ------------------------------------------------------------------- //
    
    // go for it
    try {
        // start stream
        audio.startStream();

        // idle
        while( true )
        {
            usleep( 10000 );
        }

        // stop the stream.
        audio.stopStream();
    }
    catch( RtError& e )
    {
        // print error message
        cout << e.getMessage() << endl;
        // close if open
        if( audio.isStreamOpen() )
            audio.closeStream();
    }

    return 0;
}

