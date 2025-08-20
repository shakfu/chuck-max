/*
Integration test for AZADAY fixing a bug on March 10 2025:
Pushing sufficient data to the internal audio_frame_arena would trigger
it a memory reallocation during the autoUpdateScenegraph function, invalidating
the pointer which was being used as a stack base pointer to stop iteration.

Here UI.plotLines(...) uses the audio_frame_arena as a temporary buffer to
read data from the CK_FloatArray, upping its curr size pointer in the process.
It pushes 64kb of data, which happens to be exactly the initial allocation size
of audio_frame_arena.

autoUpdateScenegraph then pushes more SG_IDs, being the "needle which breaks the camels back",
and triggering a memory reallocation internally to get more space, possibly invalidating 
the original base pointer if the new memory is in a different heap location.

Under normal circumstances, this test will NOT segfault/crash.
*/

GG.autoUpdate(true);

16 * 1024  => int WINDOW_SIZE; 
float samples[WINDOW_SIZE];

class Test extends GGen {
    GCube cube --> this;

    fun void update(float dt) {
        <<< GG.fc() >>>;
        this.rotateY(dt);
    }
}

Test t --> GG.scene();

repeat (5) {
    GG.nextFrame() => now;
    {
        UI.begin("");
        UI.plotLines("", samples);
        UI.end();
    }
}
