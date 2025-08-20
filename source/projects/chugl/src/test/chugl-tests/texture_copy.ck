

TextureDesc desc;
10 => desc.width;
10 => desc.height;
false => desc.mips;

Texture A(desc);

TextureDesc dst_small_desc;
1 => dst_small_desc.width;
1 => dst_small_desc.height;
Texture dst_small_tex(dst_small_desc);

TextureDesc no_copy_dst_desc;
Texture.Usage_TextureBinding => no_copy_dst_desc.usage;
Texture no_copy_tex(no_copy_dst_desc);

TextureDesc diff_format_desc;
Texture.Format_RGBA32Float => diff_format_desc.format;
Texture diff_format_tex(diff_format_desc);


// copying null textures is invalid
<<< "copying null textures" >>>;
Texture.copy(A, null);
Texture.copy(null, A);

// texture copy takes min of texture dimensions
<<< "copy larger to smaller ok" >>>;
Texture.copy(dst_small_tex, A);

<<< "copy smaller to larger ok" >>>;
Texture.copy(A, dst_small_tex);

<<< "cannot copy if copy_dst usage is not set" >>>;
Texture.copy(no_copy_tex, A);

<<< "cannot copy if copy_src usage is not set" >>>;
Texture.copy(A, no_copy_tex);

<<< "cannot copy if formats are different" >>>;
Texture.copy(diff_format_tex, A);
<<< "cannot copy if formats are different 2" >>>;
Texture.copy(A, diff_format_tex);

GG.nextFrame() => now;
GG.nextFrame() => now;

TextureLocation loc_a;
TextureLocation loc_b;
9999 => loc_a.x;

<<< "copying with invalid locations" >>>;
Texture.copy(A, loc_a, dst_small_tex, loc_b, 1, 1, 1);

<<< "copying with invalid range" >>>;
Texture.copy(A, loc_b, dst_small_tex, loc_b, 1000, 1000, 1000);

//
// Texture read tests
//

<<< "getting texture.data() before reading should throw warning" >>>;
A.data();
