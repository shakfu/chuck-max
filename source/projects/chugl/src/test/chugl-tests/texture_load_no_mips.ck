/*
3/17/25 azaday
Bug: when loading a texture (via Texture.load(...)) and 
passing TextureLoadDesc.gen_mips = FALSE, ChuGL would 
incorrectly create a WGPU_Texture with a full mip chain, but 
the renderer thread would *not* actually create the render passes
to generate these mips, resulting in textures either not appearing
or appearing much darker than expected, as a result of the texture sampler
sampling (0,0,0,0) pixels at mip levels that were never populated.

Fix: Texture.load() now takes into account TextureLoadDesc.gen_mips option

Test: load a texture without mips, assert that the mip level is correctly 1.
*/

TextureLoadDesc tex_load_desc;
false => tex_load_desc.gen_mips; 
Texture.load(me.dir() + "./flower_petals.png", tex_load_desc) @=> Texture@ sprite_tex;
<<< "texture has full mip chain? ", sprite_tex.mips() >>>; // should report 0 for no mip chain