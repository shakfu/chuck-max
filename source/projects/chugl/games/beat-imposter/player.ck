@import "TimeOfDay.chug"

// Broadcast myID to network
OscOut xmit;
"Andrew" => string myID;

fun void sendID()
{
    while(true)
    {
        <<< "sending ID: ", myID >>>;
        xmit.dest("224.0.0.1", 8888);
        xmit.start("/ID");
        xmit.add(myID);
        xmit.send();
        second => now;
    }
}

spork ~ sendID();

// Receive OSC messages
OscIn oin;
OscMsg msg;
8888 => oin.port;

oin.addAddress( "/beat");

SndBuf metro => dac;
metro.read("./woodblock.wav");
metro.pos(metro.samples());

int isPlaying;

fun void playMetro(float bpm)
{
    true => isPlaying;
    while(true)
    {
        <<< "beat" >>>;
        metro.pos(0);
        (60.0/bpm)::second => now;
    }
}

// infinite event loop
while( true )
{
    // wait for event to arrive
    oin => now;

    // grab the next message from the queue. 
    while( oin.recv(msg) )
    {
        if(msg.address == "/beat")
        {
            msg.getString(0) => string targetID;
            if(targetID == myID)
            {
                msg.getFloat(1) => float bpm;
                msg.getInt(2) => int LH;
                msg.getFloat(3) => float RH;
                LH + RH => float startTime;

                <<< "received bpm: ", bpm, " and startTime: ", startTime >>>;
                <<< "current tod: ", TimeOfDay.inSeconds() >>>;
                <<< "waiting for: ", (startTime - TimeOfDay.inSeconds()), " seconds" >>>;
                if(!isPlaying)
                {
                    (startTime - TimeOfDay.inSeconds())::second => now;
                    spork ~ playMetro(bpm);
                }
            }
        }
    }
}
