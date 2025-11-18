// Loading AudioUnit by Four-Character Codes
// Demonstrates the alternative method of loading AudioUnits using
// type, subtype, and manufacturer codes

@import "AudioUnit";

adc => AudioUnit au => dac;

<<< "=== Loading AudioUnit by Four-Character Codes ===" >>>;


// Load Apple's AULowpass filter using four-character codes
// Format: au.open(type, subtype, manufacturer)
// - type: "aufx" = effect
// - subtype: "lpas" = lowpass
// - manufacturer: "appl" = Apple
if (au.open("aufx", "lpas", "appl")) {
    <<< "Successfully loaded AULowpass using four-char codes!" >>>;
    <<< "  Type: 'aufx' (effect)" >>>;
    <<< "  SubType: 'lpas' (lowpass)" >>>;
    <<< "  Manufacturer: 'appl' (Apple)" >>>;


    <<< "Processing audio for 5 seconds..." >>>;
    5::second => now;

    au.close();
    <<< "Closed AudioUnit" >>>;
} else {
    <<< "Failed to load AULowpass" >>>;
}


<<< "=== Common Four-Character Codes ===" >>>;
<<< "Effects (aufx):" >>>;
<<< "  lpas - Lowpass filter" >>>;
<<< "  hpas - Highpass filter" >>>;
<<< "  bpas - Bandpass filter" >>>;
<<< "  dely - Delay" >>>;
<<< "  rvb2 - Reverb" >>>;
<<< "  dist - Distortion" >>>;

<<< "Use AudioUnit.list() to see all available codes" >>>;