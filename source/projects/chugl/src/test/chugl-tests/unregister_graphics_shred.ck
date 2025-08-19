/*

Tests GG.unregisterShred().
A shred can stop calling GG.nextFrame() and prevent the window from hanging 
by callimng GG.unregisterShred()

*/

Event e1, e2;

fun void B() {
    repeat (5) {
        GG.unregisterShred(); // no-op
        1::samp => now;
    }
}
spork ~ B();

fun void A() {
    repeat (3) GG.nextFrame() => now;

    // if commented, will hang the window
    GG.unregisterShred();

    e1 => now;
    1::samp => now;
    e2.broadcast();
}
spork ~ A();

for (int i; i < 5; i++) {
    GG.nextFrame() => now;
    <<< i >>>;
}
e1.broadcast();
e2 => now;
