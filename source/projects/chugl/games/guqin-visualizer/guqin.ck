@import "../lib/g2d/ChuGL.chug"
@import "../lib/g2d/g2d.ck"

G2D g;

// OSC in
OscIn oin;
9527 => oin.port;

// print
cherr <= "listening for OSC messages over port: " <= oin.port()
      <= "..." <= IO.newline();
// listen to everything coming
// oin.listenAll();
oin.addAddress("/hands/arr");

// something to shuttle data
OscMsg msg;

// UI elements
UI_Int2 frame_dims;
UI_Int num_hands;
UI_Float score;

vec3 hand1[21]; // x y confidence

// infinite time loop
fun void oscListener() 
{
    while( true ) {
        // wait for OSC input
        oin => now;
        
        // drain the message queue
        while( oin.recv(msg) )
        {
            (msg.getInt(0), msg.getInt(1)) => frame_dims.val;
            msg.getInt(2) => num_hands.val;

            for (int i; i < num_hands.val(); i++) {
                msg.getFloat(3 + 64 * i) => score.val;
                for (int landmark_idx; landmark_idx < 21; landmark_idx++) {
                    msg.getFloat(4 + 64 * i + 3 * landmark_idx + 0) => hand1[landmark_idx].x;
                    msg.getFloat(4 + 64 * i + 3 * landmark_idx + 1) => hand1[landmark_idx].y;
                    msg.getFloat(4 + 64 * i + 3 * landmark_idx + 2) => hand1[landmark_idx].z;
                }
            }
            
            // print address and typetag
            // cherr <= msg.address <= " " <= msg.typetag <= " ";
            // // loop over number of arguments by type
            // for(int n; n < msg.numArgs(); n++)
            // {
            //     // print by type
            //     if(msg.typetag.charAt(n) == 'i')
            //         cherr <= msg.getInt(n) <= " ";
            //     else if(msg.typetag.charAt(n) == 'f')
            //         cherr <= msg.getFloat(n) <= " ";
            //     else if(msg.typetag.charAt(n) == 's')
            //         cherr <= msg.getString(n) <= " ";
            // }
            // // print newline

            // cherr <= IO.nl();

    // The first value (int) is width of the frame.
    // The second value (int) is height of the frame.
    // The third value (int) is the number of hands.
    // The next 64 values are data for the first hand, and the 64 values after that are data for the second hand (if there is), and so on...
    // For each hand, the first value (float) is the score for that hand, the rest 63 values (floats) can be divided into 21 groups of 3, with each group being (x,y,score) of a keypoint. For the ordering of the keypoints, see handpose spec

            
        }
    }
} spork ~ oscListener();

while (true) {
    GG.nextFrame() => now;

	g.pushFontSize(.2);
    for (int i; i < hand1.size(); i++) {
        hand1[i] => vec3 v;
        -((v.x / frame_dims.x()) * 2 - 1) => float ndc_x;
        -((v.y / frame_dims.y()) * 2 - 1) => float ndc_y;
        g.NDCToWorldPos(ndc_x, ndc_y) => vec2 p;
        // g.circleFilled(p, .1);
        g.text(i + "", p);
    }
    g.popFontSize();

    if (UI.begin("HandPose OSC")) {
        UI.drag("Frame Dims", frame_dims);
        UI.slider("Score", score, 0, 100);
    }
    UI.end();
}