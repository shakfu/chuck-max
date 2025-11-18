//-----------------------------------------------------------------------------
// CLAP Host Chugin Test
// Demonstrates loading and using CLAP plugins in ChucK
//-----------------------------------------------------------------------------

// Create a CLAP host instance
CLAP clap; Gain g;
1 => g.gain;
clap => g => dac;

// Instructions
<<< "=== ChucK CLAP Host Test ===" >>>;
<<< "" >>>;
<<< "This chugin allows you to load and use CLAP format audio plugins in ChucK." >>>;
<<< "" >>>;
<<< "BASIC USAGE:" >>>;
<<< "1. Load a CLAP plugin library (.clap file)" >>>;
<<< "   clap.load(\"/path/to/plugin.clap\") => int success;" >>>;
<<< "" >>>;
<<< "2. List available plugins" >>>;
<<< "   clap.listPlugins() => int count;" >>>;
<<< "" >>>;
<<< "3. Activate a plugin" >>>;
<<< "   clap.activatePlugin(0) => int success;" >>>;
<<< "" >>>;
<<< "PARAMETER CONTROL:" >>>;
<<< "4. By parameter index:" >>>;
<<< "   clap.paramCount() => int numParams;" >>>;
<<< "   clap.paramName(0) => string name;" >>>;
<<< "   clap.paramInfo(0) => string info;" >>>;
<<< "" >>>;
<<< "5. By parameter ID:" >>>;
<<< "   clap.getParam(paramId) => float value;" >>>;
<<< "   clap.setParam(paramId, 0.5);" >>>;
<<< "" >>>;
<<< "6. By parameter name (convenient!):" >>>;
<<< "   clap.getParamByName(\"Volume\") => float vol;" >>>;
<<< "   clap.setParamByName(\"Volume\", 0.8);" >>>;
<<< "" >>>;
<<< "PRESETS:" >>>;
<<< "7. Load a preset file:" >>>;
<<< "   clap.loadPreset(\"/path/to/preset.clap\");" >>>;
<<< "" >>>;
<<< "OTHER:" >>>;
<<< "8. Bypass the plugin:" >>>;
<<< "   clap.bypass(1)  // bypass on" >>>;
<<< "   clap.bypass(0)  // bypass off" >>>;
<<< "" >>>;

// Example: Load a CLAP plugin (you'll need to provide a valid path)
// Uncomment and modify the path below to test with an actual CLAP plugin
/*
if (clap.load("/Library/Audio/Plug-Ins/CLAP/YourPlugin.clap"))
{
    <<< "Successfully loaded CLAP library!" >>>;

    // List available plugins
    <<< "Available plugins:" >>>;
    clap.listPlugins() => int pluginCount;

    if (pluginCount > 0)
    {
        // Get first plugin name
        <<< "Plugin 0:", clap.pluginName(0) >>>;

        // Activate the first plugin
        if (clap.activatePlugin(0))
        {
            <<< "Successfully activated plugin!" >>>;

            // PARAMETER EXPLORATION
            clap.paramCount() => int paramCount;
            <<< "Parameter count:", paramCount >>>;

            // List all parameters with their names
            <<< "\nParameter list:" >>>;
            for (0 => int i; i < Math.min(10, paramCount); i++)
            {
                clap.paramName(i) => string name;
                clap.paramInfo(i) => string info;
                <<< "Param", i, "- Name:", name >>>;
                <<< "  Info:", info >>>;
            }

            // PARAMETER CONTROL BY NAME EXAMPLE
            // Try to set a parameter by name (example: "Gain" or "Volume")
            <<< "\nTrying to control parameters by name..." >>>;

            // Get current value
            clap.getParamByName("Gain") => float gainValue;
            <<< "Current Gain value:", gainValue >>>;

            // Set new value
            if (clap.setParamByName("Gain", 0.7))
            {
                <<< "Successfully set Gain to 0.7" >>>;
                clap.getParamByName("Gain") => float newGain;
                <<< "New Gain value:", newGain >>>;
            }

            // PARAMETER CONTROL BY ID EXAMPLE
            // If the plugin has parameters, try the first one
            if (paramCount > 0)
            {
                clap.paramInfo(0) => string firstParamInfo;
                <<< "\nControlling first parameter by ID" >>>;
                <<< firstParamInfo >>>;

                // Get parameter ID from info (you'd parse this in real code)
                // For this example, assume ID matches index
                clap.getParam(0) => float value;
                <<< "Current value:", value >>>;

                clap.setParam(0, 0.5);
                <<< "Set to 0.5" >>>;

                clap.getParam(0) => float newValue;
                <<< "New value:", newValue >>>;
            }

            // PRESET LOADING EXAMPLE
            // if (clap.loadPreset("/path/to/preset.clap"))
            // {
            //     <<< "\nPreset loaded successfully!" >>>;
            // }
            // else
            // {
            //     <<< "\nPreset loading failed (plugin may not support presets)" >>>;
            // }

            // TEST AUDIO PROCESSING
            <<< "\n=== Audio Processing Test ===" >>>;
            SinOsc osc => clap => dac;
            440 => osc.freq;
            0.5 => osc.gain;

            <<< "Processing audio through plugin for 3 seconds..." >>>;
            3::second => now;

            // Test bypass
            <<< "Enabling bypass..." >>>;
            clap.bypass(1);
            1::second => now;

            <<< "Disabling bypass..." >>>;
            clap.bypass(0);
            1::second => now;

            <<< "\nTest complete!" >>>;
        }
        else
        {
            <<< "Failed to activate plugin" >>>;
        }
    }
}
else
{
    <<< "Failed to load CLAP library" >>>;
    <<< "Please edit this test file and provide a valid CLAP plugin path" >>>;
}
*/

// Without a plugin loaded, just pass through audio for demonstration
SinOsc osc => clap => dac;
440 => osc.freq;
0.3 => osc.gain;

<<< "\n=== Running without plugin (passthrough mode) ===" >>>;
<<< "Playing sine wave at 440 Hz" >>>;
<<< "Duration: 2 seconds" >>>;

2::second => now;

<<< "\nTest complete." >>>;
<<< "\nTo test with a real CLAP plugin:" >>>;
<<< "1. Uncomment the plugin loading code above" >>>;
<<< "2. Update the path to point to a .clap file" >>>;
<<< "3. Adjust parameter names to match your plugin" >>>;
