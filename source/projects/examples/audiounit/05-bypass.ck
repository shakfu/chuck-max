// AudioUnit Bypass
// Demonstrates bypassing an AudioUnit to compare processed vs unprocessed audio

@import "AudioUnit";

adc => AudioUnit au => dac;

<<< "=== AudioUnit Bypass Example ===" >>>;


if (au.load("AUDistortion")) {
    <<< "Loaded AUDistortion" >>>;


    // Set some distortion parameters if available
    if (au.paramCount() > 0) {
        au.setParam(0, 0.7);  // Drive/amount
        <<< "Set distortion amount" >>>;
    }

    <<< "Playing with distortion active..." >>>;
    3::second => now;


    <<< "Bypassing AudioUnit (clean signal)..." >>>;
    au.bypass(1);
    3::second => now;


    <<< "Re-enabling AudioUnit (distorted signal)..." >>>;
    au.bypass(0);
    3::second => now;


    <<< "Toggling bypass rapidly..." >>>;
    for (0 => int i; i < 8; i++) {
        i % 2 => int bypass;
        au.bypass(bypass);
        if (bypass) {
            <<< "  Bypassed (clean)" >>>;
        } else {
            <<< "  Active (distorted)" >>>;
        }
        500::ms => now;
    }

    au.close();

    <<< "Closed AudioUnit" >>>;
} else {
    <<< "Failed to load AUDistortion" >>>;
    <<< "Try another effect available on your system" >>>;
}


<<< "=== Example complete ===" >>>;