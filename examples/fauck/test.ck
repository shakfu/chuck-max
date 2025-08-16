// name: test.ck
@import "Faust";

Faust sine => dac;

sine.eval(`
  import("stdfaust.lib");
  f = hslider("freq",440,50,2000,0.01);
  phasor(freq) = (+(freq/ma.SR) ~ ma.frac);
  osc(freq) = sin(phasor(freq)*2*ma.PI);
  process = osc(f);
`);

sine.dump();

// time loop
while( true )
{
  1::samp => now;
}
