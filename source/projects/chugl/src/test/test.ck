GG.rootPass() --> ScenePass sp(GG.scene());
false => sp.msaa;

FlatMaterial mat;
PlaneGeometry geo;

TextureLoadDesc tex_load_desc;
false => tex_load_desc.gen_mips; 
Texture.load(me.dir() + "./chugl-tests/flower_petals.png", tex_load_desc) @=> Texture@ sprite_tex;
mat.colorMap(sprite_tex);

GMesh mesh(geo, mat) --> GG.scene();

// GG.camera().posZ(1);

while (1) {
    GG.nextFrame() => now;
}

// alternate way ================================

// ScenePass sp;
// rp.msaa(true);
// internally track another 4xMSAA texture that will will resolve to colorOutput



