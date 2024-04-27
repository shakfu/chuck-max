
// name: mx.fauReverb.ck
// desc: demo of Faust chugin running in chuck~ within Max in action!

global float freq_low; // default 200
global float freq_high; // default 800

// instantiate and connect faust => ck
Faust fck => Faust reverb => dac;

// evaluate Faust synth
fck.eval(`
	freq=nentry("freq",440,50,2000,0.01);
	process = os.sawtooth(freq);
`);

// evaluate Faust reverb
reverb.eval(`
	process = _ <: dm.zita_light;
`);


// time loop
while( true )
{
	// set
	fck.v("freq", Math.random2f(freq_low, freq_high) );
	// advance time
	100::ms => now;
}
