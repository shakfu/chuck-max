// List all available VST3 plugins on the system
// This is useful for discovering what VST3 plugins are available
// Run with: chuck --chugin:../VST3.chug 00-list-vst3.ck

// Create VST3 instance
VST3 vst => blackhole;

<<< "=== Listing all available VST3 plugins on your system ===" >>>;

// This will print all available VST3 plugins with their paths
vst.list();

<<< "=== End of VST3 plugin list ===" >>>;
<<< "You can use any of these VST3 plugins in your ChucK programs!" >>>;
<<< "Load by path: vst.load(\"/path/to/plugin.vst3\")" >>>;
