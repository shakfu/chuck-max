//-----------------------------------------------------------------------------
// name: texture_read.ck
// desc: Example on how to read back texture data from the GPU to chuck
//
// authors: Andrew Zhu Aday (https://ccrma.stanford.edu/~azaday/)
//
// date: Fall 2024
//-----------------------------------------------------------------------------

Webcam webcam;

// create a texture to write the webcam data we're reading
TextureDesc desc;
webcam.width() => desc.width;
webcam.height() => desc.height;
1 => desc.mips;
Texture write_texture(desc);

// original webcam texture
GPlane plane --> GG.scene();
plane.colorMap(webcam.texture());
plane.translateX(-1);

// texture after reading webcam texture back from GPU to CPU,
// and writing it back to the GPU
GPlane write_plane --> GG.scene();
write_plane.colorMap(write_texture);
write_plane.translateX(1);

fun void read() {
    while (true) {
        // issue a read from the GPU to the CPU
        webcam.texture().read() => now;
        // write the newly read data into the duplicate webcam texture
        write_texture.write(webcam.texture().data());
    }
} spork ~ read();

while (true) { GG.nextFrame() => now; }
