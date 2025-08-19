/*
Ideas
- synchronized clock visual that is some large multiple of the bpm
    - delineates the length of each player's turn, i.e. when the clock strikes 12, the next player begins 
    - 12:00 also corresponds with a beat, giving the imposter phase info
- 2 full rounds, then host tells players to stop their metronomes
- timed voting round (~30secs?) players vote on their computers for the imposter
- if imposter is correctly voted for, they have to tap the bpm
    - implement: BPM tracker
    - imposter does not need to get the phase correct
- BPM range: 80 - 159, avoids multiples

Explore
- different accents (4/4 vs 3/4 etc)
- different sounds/drum beats
    - apply warpbuf to drum loops
- kuramoto oscillator levels
*/

@import "TimeOfDay.chug"
@import "HashMap.chug" 
@import "../lib/g2d/ChuGL.chug"

SndBuf buf => dac;
"./woodblock.wav" => buf.read;
buf.samples() => buf.pos;

// multicast address sends to all machines on local network
"224.0.0.1" => string hostname;
// destination port number
8888 => int port;
HashMap player_names;
120.0 => float bpm;
60 => float round_time_sec;
0 => float remaining_time_sec;

class GameState {
    string imposter;
}
GameState gs;

fun void playerNameListener() {
    OscIn oin;
    OscMsg msg;
    port => oin.port;
    oin.addAddress( "/ID" );
    while (true) {
        oin => now;
        while (oin.recv(msg)) {
            player_names.set(msg.getString(0), true);

            <<< "players in lobby: " >>>;
            player_names.strKeys() @=> string names[];
            for (auto name: names) {
                <<< name >>>;
            }
        }
    }
} spork ~ playerNameListener();

fun void startGame() {
    Math.random2f(40, 220) => bpm;
    TimeOfDay.inSeconds() + 2.0 => float start_time;
    start_time $ int => int start_time_int;
    (start_time - start_time_int) => float start_time_dec;

    player_names.strKeys() @=> string players[];
    // pick random player as imposter
    players[Math.random2(0, players.size() - 1)] => gs.imposter;

    while (true) {
        for (auto name : players) {
            if (name == gs.imposter) continue;

            <<< "sending start_time", start_time >>>;
            <<< "parsed into: " + start_time_int + "." + start_time_dec>>>;

            // sender object
            OscOut xmit;

            // aim the transmitter at destination
            xmit.dest( hostname, port );

            // start the message...
            xmit.start( "/beat" );

            name => xmit.add;
            bpm => xmit.add;
            start_time_int => xmit.add; 
            start_time_dec => xmit.add; 

            // send it
            xmit.send();
        }
        100::ms => now;
    }
}


// start playing at start_time
fun void playBeat(float start_time) {
    <<< "time diff", start_time - TimeOfDay.inSeconds() >>>;
    (start_time - TimeOfDay.inSeconds())::second => now;

    while (true) {
        0 => buf.pos;
        (60.0/bpm)::second => now;
    }
}

UI_Int dummy;
while (true) {
    GG.nextFrame() => now;
    GG.dt() => float dt;

    { // update
        Math.max(0, remaining_time_sec - dt) => remaining_time_sec;
    }

    if (UI.begin("Host")) {
        UI.listBox("Players", dummy, player_names.strKeys());
        if (UI.button("start game")) {
            spork ~ startGame();
            round_time_sec => remaining_time_sec;
        }

        UI.progressBar(remaining_time_sec / round_time_sec, @(0,0), "round time");

        // UI.labelText("Imposter", gs.imposter);
    }
    UI.end();
}


