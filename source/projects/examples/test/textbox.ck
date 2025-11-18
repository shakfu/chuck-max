global float length;

Noise imp => OneZero lowpass => dac;
lowpass => Delay delay => lowpass;
.99999 => float R;
length => float L;
L::samp => delay.delay;
Math.pow( R, L ) => delay.gain;
-1 => lowpass.zero;
1 => imp.gain;
L::samp => now;
0 => imp.gain;
(Math.log(.0001) / Math.log(R))::samp => now;

