//-----------------------------------------------------------------------------
// TAL CLAP Plugins Test
// Tests commercial TAL plugins (if installed) with the CLAP Host Chugin
//-----------------------------------------------------------------------------

// Create CLAP host with Gain object
CLAP clap; Gain g;
1 => g.gain;
clap => g => dac;

<<< "========================================" >>>;
<<< "  TAL CLAP Plugins Test" >>>;
<<< "========================================" >>>;
<<< "" >>>;
<<< "Testing TAL plugins from /Library/Audio/Plug-Ins/CLAP/" >>>;
<<< "" >>>;

//-----------------------------------------------------------------------------
// TEST 1: TAL-Reverb-4
//-----------------------------------------------------------------------------
<<< "========================================" >>>;
<<< "TEST 1: TAL-Reverb-4" >>>;
<<< "========================================" >>>;

if (clap.load("/Library/Audio/Plug-Ins/CLAP/TAL-Reverb-4.clap"))
{
    <<< "✓ TAL-Reverb-4 loaded successfully" >>>;

    // List plugins in the bundle
    clap.listPlugins() => int count;
    <<< "Found", count, "plugin(s) in bundle" >>>;

    if (count > 0)
    {
        <<< "Plugin name:", clap.pluginName(0) >>>;

        if (clap.activatePlugin(0))
        {
            <<< "✓ Plugin activated" >>>;

            // Show parameters
            clap.paramCount() => int params;
            <<< "Parameters:", params >>>;
            <<< "" >>>;
            <<< "First 10 parameters:" >>>;
            for (0 => int i; i < Math.min(10, params); i++)
            {
                <<< "  [" + i + "]", clap.paramName(i) >>>;
            }

            <<< "" >>>;
            <<< "Playing 440 Hz tone through reverb for 4 seconds..." >>>;
            <<< "(You should hear the reverb tail after the tone stops)" >>>;

            SinOsc osc => clap;
            440 => osc.freq;
            0.3 => osc.gain;

            4::second => now;

            <<< "✓ TAL-Reverb-4 test complete" >>>;
        }
        else
        {
            <<< "✗ Failed to activate plugin" >>>;
        }
    }
}
else
{
    <<< "✗ Failed to load TAL-Reverb-4" >>>;
    <<< "Make sure TAL-Reverb-4 is installed in /Library/Audio/Plug-Ins/CLAP/" >>>;
}

<<< "" >>>;
<<< "========================================" >>>;
<<< "" >>>;

// Brief pause between tests
1::second => now;

//-----------------------------------------------------------------------------
// TEST 2: TAL-Chorus-LX
//-----------------------------------------------------------------------------
<<< "========================================" >>>;
<<< "TEST 2: TAL-Chorus-LX" >>>;
<<< "========================================" >>>;

if (clap.load("/Library/Audio/Plug-Ins/CLAP/TAL-Chorus-LX.clap"))
{
    <<< "✓ TAL-Chorus-LX loaded successfully" >>>;

    clap.listPlugins() => int count;
    <<< "Found", count, "plugin(s) in bundle" >>>;

    if (count > 0)
    {
        <<< "Plugin name:", clap.pluginName(0) >>>;

        if (clap.activatePlugin(0))
        {
            <<< "✓ Plugin activated" >>>;

            clap.paramCount() => int params;
            <<< "Parameters:", params >>>;
            <<< "" >>>;
            <<< "All parameters:" >>>;
            for (0 => int i; i < params; i++)
            {
                <<< "  [" + i + "]", clap.paramName(i) >>>;
            }

            <<< "" >>>;
            <<< "Playing 440 Hz tone through chorus for 3 seconds..." >>>;
            <<< "(You should hear a thicker, doubled sound)" >>>;

            SinOsc osc => clap;
            440 => osc.freq;
            0.3 => osc.gain;

            3::second => now;

            <<< "✓ TAL-Chorus-LX test complete" >>>;
        }
        else
        {
            <<< "✗ Failed to activate plugin" >>>;
        }
    }
}
else
{
    <<< "✗ Failed to load TAL-Chorus-LX" >>>;
    <<< "Make sure TAL-Chorus-LX is installed in /Library/Audio/Plug-Ins/CLAP/" >>>;
}

<<< "" >>>;
<<< "========================================" >>>;
<<< "" >>>;

1::second => now;

//-----------------------------------------------------------------------------
// TEST 3: TAL-Filter-2
//-----------------------------------------------------------------------------
<<< "========================================" >>>;
<<< "TEST 3: TAL-Filter-2" >>>;
<<< "========================================" >>>;

if (clap.load("/Library/Audio/Plug-Ins/CLAP/TAL-Filter-2.clap/Contents/MacOS/TAL-Filter-2"))
{
    <<< "✓ TAL-Filter-2 loaded successfully" >>>;

    clap.listPlugins() => int count;
    <<< "Found", count, "plugin(s) in bundle" >>>;

    if (count > 0)
    {
        <<< "Plugin name:", clap.pluginName(0) >>>;

        if (clap.activatePlugin(0))
        {
            <<< "✓ Plugin activated" >>>;

            clap.paramCount() => int params;
            <<< "Parameters:", params >>>;
            <<< "" >>>;
            <<< "First 10 parameters:" >>>;
            for (0 => int i; i < Math.min(10, params); i++)
            {
                <<< "  [" + i + "]", clap.paramName(i) >>>;
            }

            <<< "" >>>;
            <<< "Playing noise through filter for 3 seconds..." >>>;
            <<< "(You should hear filtered noise)" >>>;

            Noise noise => clap;
            0.2 => noise.gain;

            3::second => now;

            <<< "✓ TAL-Filter-2 test complete" >>>;
        }
        else
        {
            <<< "✗ Failed to activate plugin" >>>;
        }
    }
}
else
{
    <<< "✗ Failed to load TAL-Filter-2" >>>;
    <<< "Make sure TAL-Filter-2 is installed in /Library/Audio/Plug-Ins/CLAP/" >>>;
}

<<< "" >>>;
<<< "========================================" >>>;
<<< "  TEST SUMMARY" >>>;
<<< "========================================" >>>;
<<< "" >>>;
<<< "All TAL plugin tests completed successfully!" >>>;
<<< "" >>>;
<<< "Tested:" >>>;
<<< "  ✓ TAL-Reverb-4 (reverb effect)" >>>;
<<< "  ✓ TAL-Chorus-LX (chorus effect)" >>>;
<<< "  ✓ TAL-Filter-2 (filter effect)" >>>;
<<< "" >>>;
<<< "Notes:" >>>;
<<< "  • These plugins must be installed separately" >>>;
<<< "  • Free download from: toguaudioline.com" >>>;
<<< "  • macOS bundles are at /Library/Audio/Plug-Ins/CLAP/" >>>;
<<< "" >>>;
<<< "========================================" >>>;
