global float gfloat;     // 0.0 - 1.0
global int gint;         // 0 - 100
global int gints[6];     // array of 6 ints 
global float gfloats[6]; // array of 6 floats 


while( true )
{
	<<< "gint (value):", gint >>>;

	<<< "gfloat (value):", gfloat >>>;

	if (gints.size() > 0) {
		<<< "gints:", gints >>>;
        for( int a : gints )
        {
            <<< a >>>;
        }
	}

	if (gfloats.size() > 0) {
		<<< "gfloats:", gints >>>;
        for( float b : gfloats )
        {
            <<< b >>>;
        }
	}
	
	// advance time
	100::ms => now;
}