// List all available AudioUnits on the system
// This is useful for discovering what AudioUnits are available

@import "AudioUnit";

// Create Link instance
AudioUnit au => blackhole;

<<< "=== Listing all available AudioUnits on your system ===" >>>;

// This will print all available AudioUnits with their:
// - Type (Effect, MusicEffect, Instrument, etc.)
// - Name
// - Four-character codes (type:subtype:manufacturer)
au.list();

<<< "=== End of AudioUnit list ===" >>>;
<<< "You can use any of these AudioUnits in your ChucK programs!" >>>;
<<< "Load by name: au.load(\"AUDelay\")" >>>;
<<< "Load by code: au.open(\"aufx\", \"dely\", \"appl\")" >>>;