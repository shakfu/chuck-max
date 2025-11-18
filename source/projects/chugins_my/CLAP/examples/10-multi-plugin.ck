// Multi-plugin bundle support demo
// Shows how to enumerate and switch between multiple plugins in a bundle

// Create CLAP instance
CLAP clap => dac;

// Load a CLAP bundle (using TAL-Reverb-4 as example)
// Most CLAP files contain only one plugin, but some may contain multiple
// macOS bundle paths are automatically resolved
if(!clap.load("/Library/Audio/Plug-Ins/CLAP/TAL-Reverb-4.clap"))
{
    <<< "Failed to load CLAP plugin" >>>;
    me.exit();
}

// Check how many plugins are in the bundle
clap.pluginCount() => int count;
<<< "Plugins in bundle:", count >>>;

// List all plugins (prints to console)
if(count > 1)
{
    <<< "\n=== Available Plugins ===" >>>;
    clap.listPlugins();  // Prints plugin list with indices and descriptions

    // Get plugin names programmatically
    for(0 => int i; i < count; i++)
    {
        <<< "Plugin", i, ":", clap.pluginName(i) >>>;
    }
}
else
{
    <<< "Single plugin bundle -", clap.pluginName(0) >>>;
}

// The first plugin (index 0) is automatically activated by load()
<<< "\nCurrently active: Plugin 0 -", clap.pluginName(0) >>>;

// Test the default plugin
SinOsc osc => clap;
440 => osc.freq;
0.3 => osc.gain;

<<< "\nPlaying with default plugin for 2 seconds..." >>>;
2::second => now;

// Switch to another plugin if available
if(count > 1)
{
    <<< "\nSwitching to plugin 1..." >>>;
    if(clap.activatePlugin(1))
    {
        <<< "Activated:", clap.pluginName(1) >>>;
        <<< "Playing with plugin 1 for 2 seconds..." >>>;
        2::second => now;

        // Switch back to first plugin
        <<< "\nSwitching back to plugin 0..." >>>;
        if(clap.activatePlugin(0))
        {
            <<< "Activated:", clap.pluginName(0) >>>;
            <<< "Playing with plugin 0 for 2 seconds..." >>>;
            2::second => now;
        }
    }
}
else
{
    <<< "\nOnly one plugin in bundle, playing for 2 more seconds..." >>>;
    2::second => now;
}

<<< "\nDemo complete!" >>>;
