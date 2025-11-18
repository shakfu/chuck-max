// CLAP Multiple Effects Example
// Demonstrates chaining multiple CLAP plugins in series

// Create a chain of CLAP plugins
adc => CLAP clap1 => CLAP clap2 => dac;

<<< "=== CLAP Multiple Effects Chain Example ===" >>>;

// Replace these paths with valid CLAP plugins on your system
"/Library/Audio/Plug-Ins/CLAP/Filter.clap" => string filterPath;
"/Library/Audio/Plug-Ins/CLAP/Reverb.clap" => string reverbPath;

// Load first effect
if (clap1.load(filterPath)) {
    <<< "Successfully loaded first CLAP plugin (filter)!" >>>;

    // Load second effect
    if (clap2.load(reverbPath)) {
        <<< "Successfully loaded second CLAP plugin (reverb)!" >>>;
        <<< "Processing audio through plugin chain for 8 seconds..." >>>;

        // Process audio through both effects
        8::second => now;

        <<< "Closing plugins..." >>>;
        clap1.close();
        clap2.close();

    } else {
        <<< "Failed to load second CLAP plugin at:", reverbPath >>>;
        <<< "Processing with just the first plugin..." >>>;
        5::second => now;
        clap1.close();
    }

} else {
    <<< "Failed to load first CLAP plugin at:", filterPath >>>;
    <<< "Run examples/00-list-clap.ck to see available plugins" >>>;
}

<<< "=== Example complete ===" >>>;
