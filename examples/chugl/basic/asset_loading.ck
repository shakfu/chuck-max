//--------------------------------------------------------------------
// name: asset_loading.ck
// desc: example of loading assets (e.g., 3D models) from file
//--------------------------------------------------------------------

// asset loader
AssLoader ass_loader;

// load from file, returns a GGen as the root of a model graph
ass_loader.loadObj( me.dir() + "../data/models/suzanne.obj" ) @=> GGen@ model;

// download this asset and unzip into ../data/obj/
// https://chuck.stanford.edu/chugl/examples/data/obj/
// ass_loader.loadObj( me.dir() + "../data/obj/backpack/backpack.obj" ) @=> GGen@ model;

// connect model to scene
model --> GG.scene();

// render loop
while( true )
{
    // sync with next graphics frame
    GG.nextFrame() => now;
    // rotate
    -GG.dt() => model.rotateY;
}
