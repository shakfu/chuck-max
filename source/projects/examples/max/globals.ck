global string gstring;
global float gfloat;     // 0.0 - 1.0
global int gint;         // 0 - 100
global int gints[6];     // array of 6 ints 
global float gfloats[6]; // array of 6 floats 
global int imap[6];      // associate array of ints
global float fmap[6];    // associate array of floats


while( true )
{
	<<< "gstring (value):", gstring >>>;
	
	<<< "gint (value):", gint >>>;

	<<< "gfloat (value):", gfloat >>>;

	if (imap.size() > 0) {
		<<< "imap[\"abd\"] = ", imap["abc"] >>>;
	}

	if (fmap.size() > 0) {
		<<< "fmap[\"def\"] = ", fmap["def"] >>>;
	}

	if (gints.size() > 0) {
		<<< "gints:", gints >>>;
        for( int a : gints )
        {
            <<< a >>>;
        }
	}

	if (gfloats.size() > 0) {
		<<< "gfloats:", gfloats >>>;
        for( float b : gfloats )
        {
            <<< b >>>;
        }
	}
	
	// advance time
	5::second => now;
}