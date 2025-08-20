// test various combination of null/valid skybox and envmap

Texture.load(
    "",
    "",
    "",
    "",
    "",
    ""
) @=> Texture error_cubemap;

GG.scene().skybox() @=> Material default_skybox_material;

GG.nextFrame() => now;

GG.scene().envMap(error_cubemap);

GG.nextFrame() => now;

GG.scene().envMap(null);

GG.nextFrame() => now;

GG.scene().skybox(null);
GG.scene().envMap(null);

GG.nextFrame() => now;

GG.scene().skybox(null);
GG.scene().envMap(error_cubemap);

GG.nextFrame() => now;

GG.scene().skybox(default_skybox_material);
GG.scene().envMap(null);

GG.nextFrame() => now;

GG.scene().skybox(default_skybox_material);
GG.scene().envMap(error_cubemap);

GG.nextFrame() => now;