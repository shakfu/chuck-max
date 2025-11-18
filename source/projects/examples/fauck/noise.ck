// name: noise.ck

@import "Faust";

Faust noise => dac;

noise.eval(`
  import("stdfaust.lib");
  process = no.noise;
`);

noise.dump();

// time loop
while( true )
{
  1::samp => now;
}
