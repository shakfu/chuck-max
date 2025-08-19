/*
2D Immediate-mode vector graphics library

TODO 
- modify outputpass gamma so that we can undo the srgb texture
	- basically just make sure (.5, 0, 0) shows up as (127, 0, 0)
- default camera orthographic and z position of 10
- add depth/layer option
- catmull-rom splines
- GPolygon triangulation
- thick lines (can they all be batch drawn?)
- disable/enable srgb view (currently chugl defaults to srgb backbuffer)
- camera shake (?)

*/

public class G2D extends GGen
{
	this --> GG.scene();

    // initialize batch drawers
    G2D_Circles circles;
    G2D_Lines lines;
    G2D_SolidPolygon polygons;
    G2D_Sprite sprites; 
    G2D_Capsule capsules;
	G2D_Text texts;

    // connect to scene
    circles.mesh --> GG.scene();
    lines.mesh --> GG.scene();
    polygons.mesh --> GG.scene();
    sprites.mesh --> GG.scene();
	capsules.mesh --> GG.scene();

	texts.mesh --> GG.scene();
	texts.mesh.posZ(1); // default on top

	// init camera
	GG.camera().orthographic();
	GG.camera().viewSize(10);
	GG.camera().posZ(10);

	TextureSampler nn_output_sampler;
	TextureSampler.Filter_Nearest => nn_output_sampler.filterMin;  
	TextureSampler.Filter_Nearest => nn_output_sampler.filterMag;  
	TextureSampler.Filter_Nearest => nn_output_sampler.filterMip;  
	TextureSampler bfilt_output_sampler;

	// disable tonemapping / HDR
	GG.outputPass().tonemap(OutputPass.ToneMap_None);
	// TODO: disable the srgb view on swapchain screen buffer

	// ------------------- state stacks --------------------------
	// note: these config stacks are cleared at the end of every frame to prevent accidental leaks
	// you *don't* have to call popXXX() for every pushXXX
	[null] @=> string font_stack[];
	[1.0] @=> float font_size_stack[];
	[Color.WHITE] @=> vec3 color_stack[];
	[0.0] @=> float layer_stack[]; // z depth

	fun void pushFont(string s) { font_stack << s; }
	fun void popFont() { font_stack.popBack(); }
	fun void pushFontSize(float s) { font_size_stack << s; }
	fun void popFontSize() { font_size_stack.popBack(); }
	fun void pushColor(vec3 c) { color_stack << c; }
	fun void popColor() { color_stack.popBack(); }
	fun void pushLayer(float layer) { layer_stack << layer; }
	fun void popLayer() { layer_stack.popBack(); }

	// ----------- look & feel (aka antialias and resolution) ----------
	fun void antialias(int bool) {
		circles.antialias(bool);
		capsules.antialias(bool);

		if (!bool) {
			GG.renderPass().msaa(1);
			GG.outputPass().sampler(nn_output_sampler);
		} else {
			GG.renderPass().msaa(4);
			GG.outputPass().sampler(bfilt_output_sampler);
		}
	}

	fun void resolution(int w, int h) {
		GG.renderPass().resolution(w, h);
		GG.hudPass().resolution(w, h);
	}

	fun void backgroundColor(vec3 color) {
		GG.scene().backgroundColor(color);
	}

	// get the screen dimensions in worldspace units
	fun vec2 screenSize() {
		GWindow.windowSize() => vec2 size;
		size.x / size.y => float aspect;
		GG.camera().viewSize() => float height;
		return @(
			height * aspect,
			height
		);
	}

	// in NDC (normalized device coordinates) the screen boundaries go from [-1, 1] in x and y
	fun vec2 NDCToWorldPos(float x, float y) {
		GG.camera().NDCToWorldPos(@(x, y, 0)) => vec3 world_pos;
		return world_pos $ vec2;
	}

	// get the bounds in world space of the screen
	// fun vec4 screenBounds() {
	// 	GG.camera().NDCToWorldPos(@(-1.0, -1.0, 0)) => vec3 bottom_left;
	// 	GG.camera().NDCToWorldPos(@(1.0, 1.0, 0)) => vec3 top_right;
	// 	return (Math.fabs(pos_ndc.x) > threshold || Math.fabs(pos_ndc.y) > threshold);
	// }

	// ----------- mouse and camera --------------
	// returns world-coordinates of mouse pos
	fun vec2 mousePos() {
		GG.camera().screenCoordToWorldPos(GWindow.mousePos(), 1.0) => vec3 world_pos;
		return @(world_pos.x, world_pos.y);
	}

	fun int mouseLeftDown() {
		return GWindow.mouseLeftDown();
	}


    // ----------- circle ----------
	fun void circle(vec2 center, float radius, float thickness, vec3 color) {
        circles.circle(center, radius, thickness, @(color.r, color.g, color.b, 1.0));
	}

	fun void circleFilled(vec2 center, float radius, vec3 color) {
        circles.circle(center, radius, 1.0, @(color.r, color.g, color.b, 1.0));
	}

	fun void circleFilled(vec2 center, float radius) {
        circles.circle(center, radius, 1.0, color_stack[-1]);
	}

    // ----------- line -----------
    // draws a dashed line from p1 to p2, each dash is length `segment_length`
	fun void dashed(vec2 p1, vec2 p2, vec3 color, float segment_length) {
		p2 - p1 => vec2 d;
		Math.euclidean(p1, p2) => float dist;
		d / dist => vec2 dir;
		dist / segment_length => float N;

		for (int i; i < N; 2 +=> i) {
			lines.segment(
				(p1 + i * segment_length * dir),
				(p1 + Math.min((i + 1), N) * segment_length * dir),
				color
			);
		}
	}

	fun void line(vec2 p1, vec2 p2, vec3 color) {
		lines.segment(p1, p2, color);
	}

	fun void line(vec2 p1, vec2 p2) {
		lines.segment(p1, p2, color_stack[-1]);
	}

	// assumes arr is circular buff and we draw from start_idx --> start_idx - 1 with wraparound
	fun void line(vec2 arr[], int start_idx, int count) {
		while (start_idx < 0) arr.size() +=> start_idx;
		arr.size() => int size;
		Math.min(size, count) => count;
		for (int i; i < count-1; i++) {
			lines.segment(
				arr[(start_idx + i) % size],
				arr[(start_idx + i + 1) % size],
				color_stack[-1]
			);
		}
	}

	// draws a polygon outline at given position and rotation
	fun void polygon(vec2 pos, float rot_radians, vec2 vertices[], vec2 scale, vec3 color) {
		if (vertices.size() < 2) return;

        Math.cos(rot_radians) => float cos_a;
        Math.sin(rot_radians) => float sin_a;

		// just draw as individual line segments for now
		for (int i; i < vertices.size(); i++) {
			vertices[i] => vec2 v1;
			vertices[(i + 1) % vertices.size()] => vec2 v2;

			// scale
			scale.x *=> v1.x; scale.y *=> v1.y;
			scale.x *=> v2.x; scale.y *=> v2.y;

			lines.segment(
				pos + @(
					cos_a * v1.x - sin_a * v1.y,
					sin_a * v1.x + cos_a * v1.y
				),
				pos + @(
					cos_a * v2.x - sin_a * v2.y,
					sin_a * v2.x + cos_a * v2.y
				),
                color
			);
		}

		// // to close the loop
		// vertices[-1] => vec2 v1;
		// vertices[0] => vec2 v2;

		// // scale
		// scale.x *=> v1.x; scale.y *=> v1.y;
		// scale.x *=> v2.x; scale.y *=> v2.y;

		// drawlines.segment(
		// 	pos + @(
		// 		cos_a * v1.x - sin_a * v1.y,
		// 		sin_a * v1.x + cos_a * v1.y
		// 	),
		// 	pos + @(
		// 		cos_a * v2.x - sin_a * v2.y,
		// 		sin_a * v2.x + cos_a * v2.y
		// 	)
		// );
	}

	fun void polygon(vec2 pos, float rot_radians, vec2 vertices[], vec2 scale) {
		polygon(pos, rot_radians, vertices, scale, color_stack[-1]);
	}

	fun void polygon(vec2 pos, float rot_radians, vec2 vertices[], vec3 color) {
		polygon(pos, rot_radians, vertices, @(1,1), color);
	}

	fun void polygon(vec2 pos, float rot_radians, vec2 vertices[]) {
		polygon(pos, rot_radians, vertices, @(1,1), color_stack[-1]);
	}

	[@(0.5, 0.5), @(-0.5, 0.5), @(-0.5, -0.5), @(0.5, -0.5)] @=> vec2 square_vertices[];
	fun void box(vec2 center, float width, float height, vec3 color) {
		polygon(center, 0, square_vertices, @(width, height), color);
	}

	fun void box(vec2 center, float width, float height) {
		polygon(center, 0, square_vertices, @(width, height));
	}

	fun void box(vec2 bottom_left, vec2 top_right) {
		line(bottom_left, @(top_right.x, bottom_left.y));
		line(@(top_right.x, bottom_left.y), top_right);
		line(top_right, @(bottom_left.x, top_right.y));
		line(@(bottom_left.x, top_right.y), bottom_left);
	}

	fun void square(vec2 pos, float rot_radians, float size, vec3 color) {
		polygon(pos, rot_radians, square_vertices, @(size, size), color);
	}

	32 => int circle_segments;
	vec2 circle_vertices[circle_segments];
	for (int i; i < circle_segments; i++) {
		Math.two_pi * i / circle_segments => float theta;
		@( Math.cos(theta), Math.sin(theta)) => circle_vertices[i];
	}
	fun void circle(vec2 pos, float radians, vec3 color) {
		for (int i; i < circle_vertices.size() - 1; i++) {
			lines.segment(pos + radians * circle_vertices[i], pos + radians * circle_vertices[i+1], color);
		}
		lines.segment(pos + radians * circle_vertices[-1], pos + radians * circle_vertices[0], color);
	}

	vec2 dotted_circle_vertices[0];
	fun void circleDotted(vec2 pos, float radius, float start_theta, vec3 color) {
		dotted_circle_vertices.clear();
		// draw every other segment
		for (int i; i < 32; i++) {
			(Math.two_pi * i / 32) + start_theta => float theta;
			pos + radius * @(Math.cos(theta), Math.sin(theta)) => vec2 vertex;
			dotted_circle_vertices << vertex;
		}

		for (int i; i < 32; 2 +=> i) {
			lines.segment(dotted_circle_vertices[i], dotted_circle_vertices[i+1], color);
		}
	}

    // ---------- filled polygons ----------

	fun void polygonFilled(vec2 pos, float rot_radians, vec2 vertices[], float radius) {
		polygons.polygonFilled(pos, rot_radians, vertices, radius, color_stack[-1]);
	}

	fun void boxFilled(
		vec2 position,
		float rotation_radians,
		float width,
		float height,
		vec3 color
	) {
		.5 * width => float hw;
		.5 * height => float hh;
		polygons.polygonFilled(
			position,
			rotation_radians,
			[@(-hw, hh), @(-hw, -hh), @(hw, -hh), @(hw, hh)], 
			0,
			color	
		);
	}

	fun void boxFilled(vec2 top_left, vec2 bottom_right, vec3 color) {
		boxFilled(
			(top_left + bottom_right) * .5, // pos
			0, // rot
			Math.fabs(top_left.x - bottom_right.x), // w
			Math.fabs(top_left.y - bottom_right.y), // h
			color
		);
	}

	fun void squareFilled(
		vec2 position,
		float rotation_radians,
		float l,
		vec3 color
	) {
		boxFilled(position, rotation_radians, l, l, color);
	}

	fun void capsuleFilled(
		vec2 p1, vec2 p2, float radius, vec3 color
	) {
		capsules.capsule(p1, p2, radius, @(color.r, color.g, color.b, 1.0));
	}

    // ---------- text ----------

	fun void textLayer(float z) { texts.mesh.posZ(z); } // sets the starting Z position for all text

	fun void text(string s) { texts.text(s, font_stack[-1], @(0,0), @(font_size_stack[-1], font_size_stack[-1]), 0, color_stack[-1]); }
	fun void text(string s, vec2 pos) { texts.text(s, font_stack[-1], pos, @(font_size_stack[-1],font_size_stack[-1]), 0, color_stack[-1]); }
	fun void text(string s, vec2 pos, float sca) { texts.text(s, font_stack[-1], pos, @(sca,sca), 0, color_stack[-1]); }
	fun void text(string s, vec2 pos, float sca, float rot) { texts.text(s, font_stack[-1], pos, @(sca, sca), rot, color_stack[-1]); }
	fun void text(string s, vec2 pos, vec2 sca, float rot) { texts.text(s, font_stack[-1], pos, sca, rot, color_stack[-1]); }
	fun void text(string s, vec3 pos) { texts.text(s, font_stack[-1], pos, @(font_size_stack[-1],font_size_stack[-1]), 0, color_stack[-1]); }
	fun void text(string s, vec3 pos, float sca) { texts.text(s, font_stack[-1], pos, @(sca,sca), 0, color_stack[-1]); }
	fun void text(string s, vec3 pos, float sca, float rot) { texts.text(s, font_stack[-1], pos, @(sca, sca), rot, color_stack[-1]); }
	fun void text(string s, vec3 pos, vec2 sca, float rot) { texts.text(s, font_stack[-1], pos, sca, rot, color_stack[-1]); }

    // ---------- sprites ----------
	fun void sprite(Texture tex, vec2 pos) {
		sprites.sprite(tex, @(pos.x, pos.y, layer_stack[-1]), @(1,1), 0, color_stack[-1]);
	}

	fun void sprite(Texture tex, vec2 pos, float sca, float rot) {
		sprites.sprite(tex, @(pos.x, pos.y, layer_stack[-1]), @(sca,sca), rot, color_stack[-1]);
	}
	
	fun void sprite(Texture tex, vec2 pos, vec2 sca, float rot) {
		sprites.sprite(tex, @(pos.x, pos.y, layer_stack[-1]), sca, rot, color_stack[-1]);
	}

	fun void sprite(Texture tex, vec2 sprite_sheet_frame_dim, vec2 offset, vec2 pos, vec2 sca, float rot, vec3 color) {
		sprites.sprite(
			tex, sprite_sheet_frame_dim, offset, 
			@(pos.x, pos.y, layer_stack[-1]), sca, rot, color
		);
	}

    // ---------- internal --------------------------------------
	fun static void assert(int t, string error) {
		if (!t) <<< error >>>;
	}

    fun void update(float dt) {
        circles.update();
        lines.update();
        polygons.update();
        sprites.update();
		capsules.update();
		texts.update();

		{ // clear state stacks (prevents accidental leak)
			font_stack[0] => string default_font; 
			font_stack.clear(); 
			font_stack << default_font;

			font_size_stack[0] => float default_font_size;
			font_size_stack.clear();
			font_size_stack << default_font_size;

			color_stack[0] => vec3 default_text_color; 
			color_stack.clear(); 
			color_stack << default_text_color;

			layer_stack[0] => float default_layer; 
			layer_stack.clear(); 
			layer_stack << default_layer;
		}
    }
}

// text pool
public class G2D_Text
{
	GText text_pool[0];
	int text_count;
	GGen mesh; // parent of all GTexts

	fun void text(string s, string font, vec3 pos, vec2 sca, float rot, vec3 color) {
		if (text_count == text_pool.size()) {
			text_pool << new GText;
		}
		text_pool[text_count] @=> GText@ gtext;

		gtext --> mesh;
		gtext.pos(pos);
		gtext.sca(sca);
		gtext.rotZ(rot);
		gtext.color(color);
		gtext.text(s);
		if (font != null) gtext.font(font);

		text_count++;
	}

	fun void text(string s, string font, vec2 pos, vec2 sca, float rot, vec3 color) {
		text(s, font, @(pos.x, pos.y, 0), sca, rot, color);
	}

	fun void update() {
		// detach remaining GText pool from the scene
		for (text_count => int i; i < text_pool.size(); i++) {
			// if parent is null, all subsequent GTexts are already detached
			if (text_pool[i].parent() == null) break;
			text_pool[i].detachParent();
		}

		0 => text_count;
	}
}

public class G2D_Circles
{
	me.dir() + "./g2d_circle_shader.wgsl" @=> string shader_path;

	// set drawing shader
	ShaderDesc shader_desc;
	shader_path => shader_desc.vertexPath;
	shader_path => shader_desc.fragmentPath;
	null @=> shader_desc.vertexLayout; 

	// material shader (draws all line segments in 1 draw call)
	Shader shader(shader_desc);
	Material material;
	material.shader(shader);
	antialias(true); // default antialiasing to true

	// default bindings
	float empty_float_arr[4];
	initStorageBuffers();

	// vertex buffers
	vec4 u_center_radius_thickness[0];
	vec4 u_colors[0];

	Geometry geo;
	geo.vertexCount(0);
	GMesh mesh(geo, material);

	fun void initStorageBuffers() {
		material.storageBuffer(0, empty_float_arr);
		material.storageBuffer(1, empty_float_arr);
	}

	// set whether to antialias circles
	fun void antialias(int value) {
		material.uniformInt(2, value);
	}

	fun void circle(vec2 center, float radius, float thickness, vec4 color) {
		u_center_radius_thickness << @(center.x, center.y, radius, thickness);
		u_colors << color;
	}

	fun void circle(vec2 center, float radius, float thickness, vec3 color) {
		u_center_radius_thickness << @(center.x, center.y, radius, thickness);
		u_colors << @(color.r, color.g, color.b, 1.0);
	}

	fun void update() {
		if (u_center_radius_thickness.size() == 0) {
			initStorageBuffers(); // needed because empty storage buffers cause WGPU to crash on bindgroup creation
			return;
		}

		// update GPU vertex buffers
		material.storageBuffer(0, u_center_radius_thickness);
		material.storageBuffer(1, u_colors);
		geo.vertexCount(6 * u_center_radius_thickness.size());

		// reset
		u_center_radius_thickness.clear();
		u_colors.clear();
	}
}

// batch draws simple line segments (no width)
public class G2D_Lines
{
	me.dir() + "./g2d_lines_shader.wgsl" @=> string shader_path;

	// set drawing shader
	ShaderDesc shader_desc;
	shader_path => shader_desc.vertexPath;
	shader_path => shader_desc.fragmentPath;
	[VertexFormat.Float2, VertexFormat.Float3] @=> shader_desc.vertexLayout;

	// material shader (draws all line segments in 1 draw call)
	Shader shader(shader_desc);
	Material material;
	material.shader(shader);

	// ==optimize== use lineStrip topology + index reset? but then requires using additional index buffer
	material.topology(Material.Topology_LineList); // list not strip!

	// vertex buffers
	vec2 u_positions[0];
	vec3 u_colors[0];

	Geometry geo; // just used to set vertex count
	geo.vertexCount(0);
	GMesh mesh(geo, material);

	fun void segment(vec2 p1, vec2 p2, vec3 color) {
		u_positions << p1 << p2;
		u_colors << color << color;
	}

	fun void update()
	{
		// update GPU vertex buffers
		geo.vertexAttribute(0, u_positions);
		geo.vertexCount(u_positions.size());
		geo.vertexAttribute(1, u_colors);

		// reset
		u_positions.clear();
		u_colors.clear();
	}
}

public class G2D_SolidPolygon
{
	me.dir() + "./g2d_solid_polygon_shader.wgsl" @=> string shader_path;

	// set drawing shader
	ShaderDesc shader_desc;
	shader_path => shader_desc.vertexPath;
	shader_path => shader_desc.fragmentPath;
	null => shader_desc.vertexLayout; // no vertex layout

	// material shader (draws all solid polygons in 1 draw call)
	Shader shader(shader_desc);
	Material solid_polygon_material;
	solid_polygon_material.shader(shader);

	// storage buffers
	int u_polygon_vertex_counts[0];
	vec2 u_polygon_vertices[0];
	vec4 u_polygon_transforms[0];
	vec4 u_polygon_colors[0];
	vec4 u_polygon_aabb[0];
	float u_polygon_radius[0];
	int num_solid_polygons;

	// initialize material uniforms
	// TODO: binding empty storage buffer crashes wgpu
	int empty_int_arr[1];
	float empty_float_arr[4];
	initStorageBuffers();

	Geometry solid_polygon_geo; // just used to set vertex count
	GMesh mesh(solid_polygon_geo, solid_polygon_material);

	@(Math.FLOAT_MAX, Math.FLOAT_MAX, -Math.FLOAT_MAX, -Math.FLOAT_MAX) => vec4 init_aabb;

	fun void initStorageBuffers() {
		solid_polygon_material.storageBuffer(0, empty_int_arr);
		solid_polygon_material.storageBuffer(1, empty_float_arr);
		solid_polygon_material.storageBuffer(2, empty_float_arr);
		solid_polygon_material.storageBuffer(3, empty_float_arr);
		solid_polygon_material.storageBuffer(4, empty_float_arr);
		solid_polygon_material.storageBuffer(5, empty_float_arr);
	}

	fun void polygonFilled(
		vec2 position,
		float rotation_radians,
		vec2 vertices[], 
		float radius,
		vec3 color
	) {
		u_polygon_vertex_counts << u_polygon_vertices.size(); // offset
		u_polygon_vertex_counts << vertices.size(); // count

		init_aabb => vec4 aabb;
		for (auto v : vertices) {
			u_polygon_vertices << v;

			// update aabb
			Math.min(aabb.x, v.x) => aabb.x;
			Math.min(aabb.y, v.y) => aabb.y;
			Math.max(aabb.z, v.x) => aabb.z;
			Math.max(aabb.w, v.y) => aabb.w;
		}

		u_polygon_aabb << @(aabb.x, aabb.y, aabb.z, aabb.w);

		u_polygon_transforms << @(
            position.x, position.y, // position
            Math.cos(rotation_radians), Math.sin(rotation_radians) // rotation
        );

		u_polygon_radius << radius;

		u_polygon_colors << @(color.r, color.g, color.b, 1.0);
	}

	fun void update() {
		if (u_polygon_aabb.size() == 0) {
			initStorageBuffers(); // needed because empty storage buffers cause WGPU to crash on bindgroup creation
			return;
		}

		// upload
		{ // b2 solid polygon
			solid_polygon_material.storageBuffer(0, u_polygon_vertex_counts);
			solid_polygon_material.storageBuffer(1, u_polygon_vertices);
			solid_polygon_material.storageBuffer(2, u_polygon_transforms);
			solid_polygon_material.storageBuffer(3, u_polygon_colors);
			solid_polygon_material.storageBuffer(4, u_polygon_aabb);
			solid_polygon_material.storageBuffer(5, u_polygon_radius);

			// update geometry vertices (6 vertices per polygon plane)
			solid_polygon_geo.vertexCount(6 * u_polygon_radius.size());
		}

		// zero
		{ // b2 solid polygon
			u_polygon_vertex_counts.clear();
			u_polygon_vertices.clear();
			u_polygon_transforms.clear();
			u_polygon_colors.clear();
			u_polygon_aabb.clear();
			u_polygon_radius.clear();
			0 => num_solid_polygons;
		}
	}
}

public class G2D_Sprite
{
	int sprite_count;

	FlatMaterial flat_materials[0];
	GMesh sprites[0];

    static PlaneGeometry plane_geo;
    static TextureSampler sprite_sampler;
    TextureSampler.Filter_Nearest => sprite_sampler.filterMin;
    TextureSampler.Filter_Nearest => sprite_sampler.filterMag;
    TextureSampler.Filter_Nearest => sprite_sampler.filterMip;

	GGen mesh;
	mesh.name("G2D_Sprite Mesh");

	fun void _resizeSpritePool() {
		if (sprite_count == sprites.size()) {
			flat_materials << new FlatMaterial;
			flat_materials[-1].sampler(sprite_sampler);
			sprites << new GMesh(plane_geo, flat_materials[-1]);
		}
	}

	// TODO: add instanced mode 
	fun void sprite(
		Texture sprite_sheet, vec2 sprite_sheet_frame_dim, vec2 offset, 
		vec3 pos, vec2 sca, float rot, vec3 color
	) {
		_resizeSpritePool();

		sprites[sprite_count] @=> GMesh@ sprite_mesh;
		flat_materials[sprite_count] @=> FlatMaterial@ sprite_material;
		sprite_material.color(color);
		sprite_material.colorMap(sprite_sheet);
		sprite_material.scale( // set UV sample area to 1 sprite frame
			@(1.0 / sprite_sheet_frame_dim.x, 1.0 / sprite_sheet_frame_dim.y)
		);
		sprite_material.offset(offset); // set UV sample offset

		sprite_mesh --> mesh;
		sprite_mesh.pos(pos);
		sprite_mesh.sca(sca);
		sprite_mesh.rotZ(rot);

		sprite_count++;
	}

	fun void sprite(Texture tex, vec3 pos, vec2 sca, float rot, vec3 color) {
		sprite(tex, @(1,1), @(0,0), pos, sca, rot, color);
	}

    fun void update() {
		// disconnect unused sprite pool items
		for (sprite_count => int i; i < sprites.size(); i++) {
			if (sprites[i].parent() == null) break;
			sprites[i].detachParent();
		}

		G2D.assert(sprites.size() == flat_materials.size(), "sprites GMesh && material size mismatch");

		0 => sprite_count;
    }
}


public class G2D_Capsule
{	
	me.dir() + "./g2d_capsule_shader.wgsl" @=> string shader_path;

	// set drawing shader
	ShaderDesc shader_desc;
	shader_path => shader_desc.vertexPath;
	shader_path => shader_desc.fragmentPath;
	null @=> shader_desc.vertexLayout; 

	// material shader (draws all line segments in 1 draw call)
	Shader shader(shader_desc);
	Material material;
	material.shader(shader);
	antialias(true);

	// default bindings
	float empty_float_arr[4];
	initStorageBuffers();

	// vertex buffers
	vec2 u_positions[0];
	float u_radius[0];
	vec4 u_colors[0];

	Geometry geo;
	geo.vertexCount(0);
	GMesh mesh(geo, material);

	fun void initStorageBuffers() {
		material.storageBuffer(0, empty_float_arr); // p1, p2
		material.storageBuffer(1, empty_float_arr); // radius
		material.storageBuffer(2, empty_float_arr); // color
	}

	fun void antialias(int a) {
		material.uniformInt(3, a);
	}

	fun void capsule(vec2 p1, vec2 p2, float radius, vec4 color) {
		u_positions << p1 << p2;
		u_radius << radius;
		u_colors << color;
	}

    fun void update() {
		if (u_radius.size() == 0) {
			initStorageBuffers(); // needed because empty storage buffers cause WGPU to crash on bindgroup creation
			return;
		}

		// update GPU vertex buffers
		material.storageBuffer(0, u_positions); // p1, p2
		material.storageBuffer(1, u_radius); // radius
		material.storageBuffer(2, u_colors); // color
		geo.vertexCount(6 * u_radius.size());

		// reset
        u_positions.clear();
        u_radius.clear();
        u_colors.clear();
    }
}