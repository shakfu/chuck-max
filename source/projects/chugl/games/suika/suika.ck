/*
=== b2 overview ===
Body: transform, body type (static, kinematic, dynamic)

ShapeDef: collision parameters (friction, density, filter)

Polygon/circle/capsule: geometry data

ShapeDef is analgous to a physics "Material", only instead of data for
rendering, it contains data for physics simulation.

a (shapeDef, geometry) pair is bound to a body via createPolygonShape(),
which actually creates a shape_id and shape object.

b2World > b2Body > b2shape
            |         |
         body_def     |
                  shape_def and geometry

==optimization== 
I believe b2 copies all the data you pass to its construction fns,
so should only need 1 body_def, shape_def, and geometry for every *kind*
of entity, rather than a new definition structs/geometries for every single entity

Fruit Diameters
watermelon: 224  +34  x8
melon: 190       +37  x6.7
pineapple: 153   +17  x5.5
peach: 136       +24  x4.9
cante: 112       +17  x4
apple: 95        +19  x3.4
tomato: 76       +16  x2.7
orange: 60       +14  x2.14
grap: 46         +10  x1.64
strawberry: 36   +8   x1.29
cherry: 28

Ae^(kt) = 8

https://github.com/a327ex/emoji-merge?tab=readme-ov-file#arenamerge_emojis
value_to_emoji_data = {
    [1] = {emoji = 'slight_smile', rs = 9, score = 1, mass_multiplier = 8, stars = 2, spawner_offset = vec2(0, 18)},
    [2] = {emoji = 'blush', rs = 11.5, score = 3, mass_multiplier = 6, stars = 2, spawner_offset = vec2(0, 20)},
    [3] = {emoji = 'thinking', rs = 16.5, score = 6, mass_multiplier = 4, stars = 3, spawner_offset = vec2(0, 25)},
    [4] = {emoji = 'devil', rs = 18.5, score = 10, mass_multiplier = 2, stars = 3, spawner_offset = vec2(0, 27)},
    [5] = {emoji = 'angry', rs = 23, score = 15, mass_multiplier = 1, stars = 4, spawner_offset = vec2(0, 32)},
    [6] = {emoji = 'relieved', rs = 29.5, score = 21, mass_multiplier = 1, stars = 4},
    [7] = {emoji = 'yum', rs = 35, score = 28, mass_multiplier = 1, stars = 5},
    [8] = {emoji = 'joy', rs = 41.5, score = 36, mass_multiplier = 1, stars = 6},
    [9] = {emoji = 'sob', rs = 47.5, score = 45, mass_multiplier = 0.5, stars = 8},
    [10] = {emoji = 'smirk', rs = 59, score = 56, mass_multiplier = 0.5, stars = 12},
    [11] = {emoji = 'sunglasses', rs = 70, score = 66, mass_multiplier = 0.25, stars = 24},
}
the factor of density between smallest and largest is 8 / .25 = 32,
whereas difference in area is 8^2 = 64

*/

@import "ChuGL.chug"
@import "T.ck"
@import "b2/b2.ck"
@import "g2d/g2d.ck"
@import "HashMap"

G2D g2d;
GText.defaultFont(me.dir() + "./assets/m5x7.ttf");

GG.outputPass().tonemap(OutputPass.ToneMap_None);
GG.camera().orthographic();
GG.camera().viewSize(10); // viewport height is 10 meters
GG.scene().backgroundColor(.03 * Color.WHITE);
GWindow.mouseMode(GWindow.MouseMode_Disabled);

// game state enum
0 => int GameState_Start;
1 => int GameState_GameOver;

// Math helper
class M {

    // ---------- Tweens ----------
    fun static float easeOutQuad(float x) {
        return 1 - (1 - x) * (1 - x);
    }

    fun static float easeInOutCubic(float x) {
        if (x < 0.5) 
            return 4 * x * x * x;
        else 
            return 1 - Math.pow(-2 * x + 2, 3) / 2;
    }


    fun static float mag(vec2 n) {
        return  Math.hypot(n.x, n.y); 
    }

    fun static int choose(float pdf[]) {
        0.0 => float total;
        0.0 => float accum;

        for (auto p : pdf) p +=> total;
        Math.random2f(0.0, total) => float pick;

        for (int i; i < pdf.size(); i++) {
            pdf[i] +=> accum;
            if (accum >= pick) return i;
        }
        return pdf.size() - 1;
    }
}

// config constants
class Config {
    // bounding container
    13.5 / 11.5 => float CONTAINER_RATIO; // ratio of height : width
    3 => float CONTAINER_HW;
    CONTAINER_RATIO * CONTAINER_HW => float CONTAINER_HH;
    .1 => float CONTAINER_THICKNESS;
    CONTAINER_HH => float GAME_OVER_HEIGHT;
    GAME_OVER_HEIGHT - 1.0 => float GAME_OVER_WARNING_HEIGHT;

    // UI
    0.6 => float vspace;
    0.6 => float font_scale;

    // b2
    UI_Bool b2_debug_draw(false);

    // drop params
    UI_Float mouse_speed(0.33);
    UI_Float mouse_interp(0.38); 
    UI_Float key_speed(806); 

    // shared chugl resources
    // CircleGeometry circle_geo;
    PlaneGeometry sprite_geo;
    TextureSampler sprite_sampler;
    // TextureSampler.Filter_Nearest => sprite_sampler.filterMin;
    // TextureSampler.Filter_Nearest => sprite_sampler.filterMag;
    // TextureSampler.Filter_Nearest => sprite_sampler.filterMip;
    TextureSampler.Filter_Linear => sprite_sampler.filterMin;
    TextureSampler.Filter_Linear => sprite_sampler.filterMag;
    TextureSampler.Filter_Linear => sprite_sampler.filterMip;
    TextureLoadDesc texture_load_desc;
    true => texture_load_desc.flip_y;


    // drop selection pdf
    [30.0, 25, 20, 15, 10] @=> float drop_selection_pdf[];

    // mergeable ball sizes
    CONTAINER_HW / 15.0 => float BASE_BALL_RADIUS; // smallest ball is relative to container size
    1 => float BASE_BALL_DENSITY;

    1::second => dur CHECK_GAMEOVER_COOLDOWN; // only check for gameover state this much time after last merge

    // Sound
    64 => int LISA_MAX_VOICES;


} Config conf;

// layer enum
0 => int Layer_Base;
1 => int Layer_Sprite;
2 => int Layer_Text;

// global gamestate
class GS {
    GameState_Start => int gamestate;

    // container
    int left_wall_b2_body_id;
    int right_wall_b2_body_id;


    // ball state
    float highest_body_pos; // ignoring the one we just dropped

    // drop logic
    int curr_drop_type;
    int next_drop_type;
    FlatMaterial@ curr_drop_material, next_drop_material;
    GMesh@ curr_drop_mesh, next_drop_mesh;

    @(0, conf.CONTAINER_HH + .5) => vec2 drop_pos;
    drop_pos => vec2 drop_pos_target;
    int drop_waiting_on;  // the b2bodyid of the mergeable we last dropped and are waiting to fall
    FlatMaterial@ hand_material;
    GMesh@ hand_mesh;

    // event times
    time last_merge_time;

    int score;
    int high_score;
    GGen next_region --> GG.scene();
    GText score_text --> GG.scene();
    GText score_number_text --> GG.scene();
    GText high_score_text --> GG.scene();
    GText high_score_number_text --> GG.scene();
    GText next_text --> next_region;
    GPlane game_over_plane --> GG.scene();
    GText game_over_text --> game_over_plane;
    GText retry_text --> game_over_plane;

    // chain of evolution
    GGen chain_of_evolution --> GG.scene();
    GText chain_of_evolution_text --> chain_of_evolution;
    FlatMaterial chain_of_evolution_materials[11];
    GMesh@ chain_of_evolution_meshes[11];
    GText chain_of_evolution_texts[11];
    

    int b2_world_id;

    Entity entities[0];
    HashMap entity_map; // b2_body_id --> Entity

    // audio related
    int measure_beat_drop;

} GS gs;

fun void drop() {
    dropBody(gs.curr_drop_type);
    gs.next_drop_type => gs.curr_drop_type;
    M.choose(conf.drop_selection_pdf) => gs.next_drop_type;
}

// gs.entities.help();

MergeType merge_types[0];
class MergeType
{
    false => int unlocked;
    string name;
    float radius; // b2 collider radius
    int   score;
    float density; 
    float draw_radius_mod; // radius * draw_radius_mod is sca of sprite
    vec3 bg_col;
    Texture@ color_map;

    fun static void add(string name, float r, int s, float d, float draw_radius_mod, vec3 bg_col, string tex_path ) {
        MergeType m;
        name => m.name;
        conf.BASE_BALL_RADIUS * r => m.radius;
        s => m.score;
        d => m.density;
        draw_radius_mod => m.draw_radius_mod;
        bg_col => m.bg_col;
        merge_types << m;
        Texture.load(tex_path, conf.texture_load_desc) @=> m.color_map;
    }
}

{ // init gamestate

    // load textures and merge types
    //            name                radius    score    density    draw_mod bg_color         path                                    
    MergeType.add("VCV Rack",         8.0/9.0,  1,       8,         1.0,     Color.WHITE,     me.dir() + "./assets/vcv.png");
    MergeType.add("Max MSP",          11.5/9.0, 3,       6,         1.0,     Color.WHITE,     me.dir() + "./assets/max_msp.png");
    MergeType.add("Ableton",          16.5/9.0, 6,       4,         1.08,    Color.WHITE,     me.dir() + "./assets/ableton.png");
    MergeType.add("Csound",           18.5/9.0, 10,      2,         1.0,     Color.WHITE,     me.dir() + "./assets/csound.png");
    MergeType.add("SuperCollider",    23.0/9.0, 15,      1,         1.0,     Color.WHITE,     me.dir() + "./assets/super_collider.png");
    MergeType.add("Sibelius",         29.5/9.0, 21,      1,         1.1,     Color.WHITE,     me.dir() + "./assets/sibelius.png");
    MergeType.add("MuseScore",        35.0/9.0, 28,      1,         1.0,     Color.WHITE,     me.dir() + "./assets/musescore.png");
    MergeType.add("Faust",            41.5/9.0, 36,      1,         1.0,     Color.WHITE,     me.dir() + "./assets/faust.png");
    MergeType.add("JUCE",             47.5/9.0, 45,      0.5,       1.0,     Color.WHITE,     me.dir() + "./assets/juce.png");
    MergeType.add("Pure Data",        59.0/9.0, 56,      0.5,       .66,     Color.WHITE,     me.dir() + "./assets/pure_data.png");
    MergeType.add("ChucK",            72.0/9.0, 66,      0.25,      .96,     Color.BLACK,     me.dir() + "./assets/chuck.png");


    // b2 world creation
    b2WorldDef world_def;
    // false => world_def.enableSleep;  // disable sleep on entire world so suika balls always trigger collisions
    // .05 => world_def.hitEventThreshold;
    b2.createWorld(world_def) => gs.b2_world_id;
    { // simulation config (eventually group these into single function/command/struct)
    // TODO: follow the lisa playback model. e.g. one-time set loop/rate/etc params on a voice and it auto plays.
    // same should be with physics: world = voice, playback rate = physics simulation rate, #concurrent threads, etc...
    // put params in struct so we can add more without breaking the API (e.g. #threads will be added after enkiTS task scheduler )
        b2.world(gs.b2_world_id);
        // b2.substeps(1);
    }

    b2CreateStaticBody([
        @(-conf.CONTAINER_HW, conf.CONTAINER_HH),
        @(-conf.CONTAINER_HW - conf.CONTAINER_THICKNESS, conf.CONTAINER_HH),
        @(-conf.CONTAINER_HW - conf.CONTAINER_THICKNESS, -conf.CONTAINER_HH),
        @(-conf.CONTAINER_HW, -conf.CONTAINER_HH),
    ]) => gs.left_wall_b2_body_id;

    b2CreateStaticBody([
        @(conf.CONTAINER_HW + conf.CONTAINER_THICKNESS, conf.CONTAINER_HH),
        @(conf.CONTAINER_HW, conf.CONTAINER_HH),
        @(conf.CONTAINER_HW, -conf.CONTAINER_HH),
        @(conf.CONTAINER_HW + conf.CONTAINER_THICKNESS, -conf.CONTAINER_HH),
    ]) => gs.right_wall_b2_body_id;

    b2CreateStaticBody([
        @(conf.CONTAINER_HW + conf.CONTAINER_THICKNESS, -conf.CONTAINER_HH),
        @(-conf.CONTAINER_HW - conf.CONTAINER_THICKNESS, -conf.CONTAINER_HH),
        @(-conf.CONTAINER_HW - conf.CONTAINER_THICKNESS, -conf.CONTAINER_HH - conf.CONTAINER_THICKNESS),
        @(conf.CONTAINER_HW + conf.CONTAINER_THICKNESS, -conf.CONTAINER_HH - conf.CONTAINER_THICKNESS),
    ]);


    { // drop mesh
        new FlatMaterial @=> gs.curr_drop_material;
        new FlatMaterial @=> gs.next_drop_material;
        new FlatMaterial @=> gs.hand_material;
        gs.curr_drop_material.sampler(conf.sprite_sampler);
        gs.next_drop_material.sampler(conf.sprite_sampler);
        gs.hand_material.sampler(conf.sprite_sampler);

        // merge_types[pl_type].color_map => this.sprite_material.colorMap;
        new GMesh(conf.sprite_geo, gs.curr_drop_material) @=> gs.curr_drop_mesh;
        new GMesh(conf.sprite_geo, gs.next_drop_material) @=> gs.next_drop_mesh;
        new GMesh(conf.sprite_geo, gs.hand_material) @=> gs.hand_mesh;
        gs.curr_drop_mesh --> GG.scene();
        gs.next_drop_mesh --> gs.next_region;
        // gs.hand_mesh --> GG.scene();
        gs.next_drop_mesh.translateY(-1.5 * conf.vspace);
        Layer_Sprite => gs.curr_drop_mesh.posZ;
        Layer_Sprite => gs.next_drop_mesh.posZ;
        Layer_Sprite => gs.hand_mesh.posZ;

        gs.next_region.translateX(1.67 * conf.CONTAINER_HW);
        gs.next_region.translateY(conf.CONTAINER_HH);

        // initialize dropper
        merge_types[-1].color_map => gs.hand_material.colorMap;
        -Math.pi / 2.0 => gs.hand_mesh.rotZ; 
        .5 => gs.hand_mesh.sca;
    }


    gs.score_text.controlPoints(@(0.5, 1.0));
    gs.score_text.sca(conf.font_scale);
    gs.score_text.pos(@(-1.67 * conf.CONTAINER_HW, conf.CONTAINER_HH));
    gs.score_text.text("Score");

    gs.score_number_text.sca(conf.font_scale);
    gs.score_number_text.pos(gs.score_text.pos() - @(0, conf.vspace, 0));
    gs.score_number_text.text(gs.score + "");

    gs.high_score_text.controlPoints(@(0.5, 1.0));
    gs.high_score_text.sca(conf.font_scale);
    gs.high_score_text.pos(gs.score_number_text.pos() - @(0, 2*conf.vspace, 0));
    gs.high_score_text.text("High Score");

    gs.high_score_number_text.sca(conf.font_scale);
    gs.high_score_number_text.pos(gs.high_score_text.pos() - @(0, conf.vspace, 0));
    gs.high_score_number_text.text(gs.high_score + "");

    gs.game_over_plane.color(Color.BLACK);
    gs.game_over_plane.scaX(6 * conf.CONTAINER_HW);
    gs.game_over_plane.scaY(2);
    gs.game_over_plane.posZ(Layer_Text);

    gs.game_over_text.scaWorld(conf.font_scale);
    gs.game_over_text.posWorld(@(0, 0.5, 0));
    gs.game_over_text.text("GAME OVER");
    Layer_Text => gs.game_over_text.posZ;

    gs.retry_text.scaWorld(conf.font_scale);
    gs.retry_text.posWorld(@(0, -0.5, 0));
    gs.retry_text.text("PRESS <space> TO RETRY");
    Layer_Text => gs.retry_text.posZ;

    gs.next_text.controlPoints(@(0.5, 1.0));
    gs.next_text.sca(conf.font_scale);
    gs.next_text.text("Next");
    Layer_Text => gs.next_text.posZ;

    gs.chain_of_evolution.pos(@(
        gs.next_region.pos().x,
        gs.high_score_text.pos().y
    ));

    gs.chain_of_evolution_text.controlPoints(@(0.5, 1.0));
    gs.chain_of_evolution_text.sca(conf.font_scale);
    gs.chain_of_evolution_text.text("Chain of\nEvolution");
    Layer_Text => gs.chain_of_evolution_text.posZ;

    { // chain of evolution
        for (int i; i < gs.chain_of_evolution_meshes.size(); i++) {
            GGen row --> gs.chain_of_evolution;
            row.posY(-(i) * conf.vspace * .5 - 2*conf.vspace);
            row.posX(-.3);
            row.sca(.22);

            0.6 => float hspace;

            new GMesh(conf.sprite_geo, gs.chain_of_evolution_materials[i]) @=> gs.chain_of_evolution_meshes[i];
            gs.chain_of_evolution_meshes[i] --> row;
            gs.chain_of_evolution_meshes[i].posX(-hspace);
            gs.chain_of_evolution_materials[i].color(Color.BLACK);

            gs.chain_of_evolution_texts[i].controlPoints(@(0.0, 0.5));
            gs.chain_of_evolution_texts[i] --> row;
            gs.chain_of_evolution_texts[i].posX(hspace);
            gs.chain_of_evolution_texts[i].text("???");
        }
    }
    

    enterGamestate(GameState_Start);
}

fun void enterGamestate(int state) {
    { // leaving logic
    }

    state => gs.gamestate;

    { // entering logic
        if (state == GameState_GameOver) {
            gs.game_over_plane --> GG.scene();
        }
        else if (state  == GameState_Start) {
            /*
            Clear container
            - balls pop 1 by 1
            - rotate and pour out
            - bottom trap door opens
            // first: create entity.destroy method.
            */

            // delete all existing entities
            Entity.destroyAll();

            // select next drops
            M.choose(conf.drop_selection_pdf) => gs.curr_drop_type;
            M.choose(conf.drop_selection_pdf) => gs.next_drop_type;

            // update high score
            Math.max(gs.score, gs.high_score) => gs.high_score;
            0 => gs.score;

            // clear LUT
            gs.entity_map.clear();

            // remove text
            gs.game_over_plane.detachParent();

            // clear waiting on
            0 => gs.drop_waiting_on;
        }
    }
}

class FX
{
    // slowly expanding ring
    fun void ripple(vec2 pos, float start_radius, float end_radius) {
        .5 => float end_radius;
        .5::second => dur effect_dur;

        dur elapsed_time;
        while (elapsed_time < effect_dur) {
            GG.nextFrame() => now;
            GG.dt()::second +=> elapsed_time;
            M.easeOutQuad(elapsed_time / effect_dur) => float t;

            g2d.circle(
                pos, 
                start_radius + (end_radius - start_radius) * t, 
                .2 * (1 - t), 
                Color.WHITE * (1 - t)
            );
        }
    }
} FX fx;


DebugDraw debug_draw;
true => debug_draw.drawShapes;
// true => debug_draw.drawAABBs; // calls drawPolygon, not drawSegment

// b2 static bodies
fun int b2CreateStaticBody(vec2 vertices[]) {
    // body def
    b2BodyDef static_body_def;
    b2BodyType.staticBody => static_body_def.type;
    @(0.0, 0.0) => static_body_def.position;
    b2.createBody(gs.b2_world_id, static_body_def) => int body_id;

    // shape def
    b2ShapeDef shape_def;

    // geometry
    b2.makePolygon(vertices, 0.0) @=> b2Polygon polygon;

    // shape
    b2.createPolygonShape(body_id, shape_def, polygon);

    return body_id;
}

class Entity
{
    // mergable fields
    int pl_type; // index into merge_types array
    MergeType@ merge_type; 
    int b2_body_id;
    float bounding_radius;
    FlatMaterial@ sprite_material;
    GMesh@ sprite_mesh;

    fun @construct(int pl_type, int b2_body_id, float radius, vec2 pos) {
        pl_type => this.pl_type;
        merge_types[pl_type] @=> this.merge_type;
        b2_body_id => this.b2_body_id;
        radius => this.bounding_radius;


        new FlatMaterial @=> this.sprite_material;
        merge_types[pl_type].color_map => this.sprite_material.colorMap;

        this.sprite_material.sampler(conf.sprite_sampler);
        new GMesh(conf.sprite_geo, this.sprite_material) @=> this.sprite_mesh;
        this.sprite_mesh.sca(radius * 2 * this.merge_type.draw_radius_mod );
        pos => this.sprite_mesh.pos;
        this.sprite_mesh --> GG.scene();
        Layer_Sprite => this.sprite_mesh.posZ;
    }

    fun static void destroyAll() {
        for (auto e : gs.entities) {
            b2.destroyBody(e.b2_body_id);
            e.sprite_mesh.detach();
        }
        gs.entity_map.clear();
        gs.entities.clear();
    }

    fun void destroy() {
        if (b2Body.isValid(b2_body_id)) {
            b2.destroyBody(b2_body_id);
            gs.entity_map.del(b2_body_id);
            sprite_mesh.detach();

            // == optimize ==
            // add iter to hashmap API and do away with gs.entities array completely
            for (int i; i < gs.entities.size(); i++) {
                if (gs.entities[i] == this) {
                    gs.entities.popOut(i);
                    break;
                }
            }
        }
    }
}



fun Entity addBody(int which, vec2 pos)
{
    if (which >= merge_types.size()) return null; // can't merge 2 watermelons

    merge_types[which] @=> MergeType mt;
    true => mt.unlocked;

    { // chain of evolution
        mt.color_map => gs.chain_of_evolution_materials[which].colorMap;
        gs.chain_of_evolution_materials[which].color(Color.WHITE);
        gs.chain_of_evolution_texts[which].text(mt.name);
    }


    // body def
    b2BodyDef dynamic_body_def;
    b2BodyType.dynamicBody => dynamic_body_def.type;
    pos => dynamic_body_def.position;
    .5 * Math.random2f(-Math.pi,Math.pi) => dynamic_body_def.angularVelocity; // initial roll
    0.5 => dynamic_body_def.angularDamping;


    // @(Math.random2f(-4.0, 4.0), Math.random2f(6.0, 12.0)) => dynamic_body_def.position;
    // Math.random2f(0.0,Math.two_pi) => float angle;
    // @(Math.cos(angle), Math.sin(angle)) => dynamic_body_def.rotation;
    b2.createBody(gs.b2_world_id, dynamic_body_def) => int dynamic_body_id;


    // geometry 

    { // old radius and density calculations (not table driven)
        // r1 ^ 2 / r0 ^ 2 = 2
        // conf.CONTAINER_HW / 1.86 => float max_radius; // largest fruit is 1
        // max_radius / 8.6 => float A; // radius of fruit 0 is 1/8.6 the radius of largest fruit
        // Math.log(max_radius / A) / 10.0 => float k;
        // A * Math.exp(k * which) => float radius;

        // meanwhile the density is inversely proportional
        // e.g. the smallest fruit is 1/N the radius of the largest, but Nx as dense 
        // Math.pow(Math.exp(k * (10 - which)), 1.6) => dynamic_shape_def.density;
    }

    // shape def
	b2ShapeDef dynamic_shape_def;
	true => dynamic_shape_def.enableHitEvents;
    true => dynamic_shape_def.enableContactEvents;
    conf.BASE_BALL_DENSITY * mt.density => dynamic_shape_def.density;
    0.2 => dynamic_shape_def.restitution;

    // shape
    mt.radius => float radius;
    b2Circle circle(radius);
    b2.createCircleShape(dynamic_body_id, dynamic_shape_def, circle);

    // <<< which, "density: ", dynamic_shape_def.density, "| radius: ", circle.radius >>>;

    // save
    gs.entities << new Entity(which, dynamic_body_id, radius, pos);
    gs.entity_map.set(dynamic_body_id, gs.entities[-1]);

	// if (which == 0) {
	// 	b2.makeBox(1.0f, 1.0f) @=> b2Polygon dynamic_box;
	// 	b2.createPolygonShape(dynamic_body_id, dynamic_shape_def, dynamic_box);
	// } else if (which == 1) {
	// 	b2.makePolygon(
	// 		[ // hexagon
	// 			@(-1, 0),
	// 			@(-.5, Math.sqrt(3)/2),
	// 			@(.5, Math.sqrt(3)/2),
	// 			@(1, 0),
	// 			@(.5, -Math.sqrt(3)/2),
	// 			@(-.5, -Math.sqrt(3)/2),
	// 		],
	// 		0.0
	// 	) @=> b2Polygon dynamic_polygon;
	// 	b2.createPolygonShape(dynamic_body_id, dynamic_shape_def, dynamic_polygon);
	// } else if (which == 2) {
	// 	b2Circle circle(Math.random2f(0.3, 0.7));
	// 	b2.createCircleShape(dynamic_body_id, dynamic_shape_def, circle);
	// } else if (which == 3) {
	// 	b2Capsule capsule(@(0.0, 0.0), @(0.0, 1.0), Math.random2f(0.2, .7));
	// 	b2.createCapsuleShape(dynamic_body_id, dynamic_shape_def, capsule);
	// }

    return gs.entities[-1];
}

fun void dropBody(int which) {
    addBody(which, gs.drop_pos) @=> Entity e;
    if (e != null) e.b2_body_id => gs.drop_waiting_on;
}

b2BodyMoveEvent move_events[0];

int begin_touch_events[0];
int end_touch_events[0];
b2ContactHitEvent hit_events[0];

// draws flashing red line as ball height approaches game over
fun void gameoverWarningLineDrawer() {
    int on;
    float time_since_toggle;

    while (1) {
        GG.nextFrame() => now;
        GG.dt() +=> time_since_toggle;

        // g2d.dashed(
        //     @(-conf.CONTAINER_HW, conf.GAME_OVER_WARNING_HEIGHT),
        //     @(conf.CONTAINER_HW, conf.GAME_OVER_WARNING_HEIGHT),
        //     Color.WHITE,
        //     .1
        // );

        if (gs.gamestate == GameState_Start) {
            if (gs.highest_body_pos > conf.GAME_OVER_WARNING_HEIGHT) {
                // compute flash cooldown
                conf.GAME_OVER_HEIGHT - conf.GAME_OVER_WARNING_HEIGHT => float margin;
                gs.highest_body_pos - conf.GAME_OVER_WARNING_HEIGHT => float d;
                Math.remap(
                    d,
                    0, margin,
                    0.5, 0
                ) => float cd;

                if (time_since_toggle > cd) {
                    0 => time_since_toggle;
                    1 - on => on;
                }

                if (on) {
                    g2d.dashed(
                        @(-conf.CONTAINER_HW, conf.GAME_OVER_HEIGHT),
                        @(conf.CONTAINER_HW, conf.GAME_OVER_HEIGHT),
                        Color.RED,
                        .1
                    );
                }
            }
        }

    }
} spork ~ gameoverWarningLineDrawer();

class Sound
{
    load("special:dope") @=> LiSa lisa;
    PoleZero blocker => NRev reverb => dac;
    // PoleZero blocker => Plateau reverb => dac;
    // connect
    lisa.chan(0) => blocker;
    // reverb mix
    .05 => reverb.mix;
    // .5 =>  reverb.wet;
    // pole location to block DC and ultra low frequencies
    .99 => blocker.blockZero;

    // scale (needs size = #balls)
    // C2 Bb     G      F      Eb     C    Cb      G      F       Eb     C 
    [2.0, 9.0/5, 3.0/2, 4.0/3, 6.0/5, 1.0, 9.0/10, 3.0/4, 4.0/6, 6.0/10, 1.0/2]
    @=> float rates[];
    .5 => float rate_mod;

    fun void play(int pl_type) {
        // get a voice to use
        lisa.getVoice() => int voice;

        // if available
        voice > -1 => int voice_available;
        if (voice_available) {
            // determine pitch from pl_type according to minor penta scale
            rate_mod * rates[pl_type] => float rate;

            // lisa.voiceGain(voice, 1.0);
            lisa.rate( voice, rate );
            lisa.playPos( voice, 0::samp);
            lisa.rampUp( voice, 0::samp);
            // (grainLen - rampUp) => now;
            // lisa.rampDown( voice, rampDown );
            // rampDown => now;
        } else {
            T.assert(false, "lisa out of voices");
        }
    }

    // load file into a LiSa
    fun LiSa load(string filename) {
        SndBuf buffy;
        filename => buffy.read;
        
        LiSa lisa;
        buffy.samples()::samp => lisa.duration;
        
        // transfer values from SndBuf to LiSa
        for( 0 => int i; i < buffy.samples(); i++ ) {
            // args are sample value and sample index
            // (dur must be integral in samples)
            lisa.valueAt( buffy.valueAt(i), i::samp );        
        }
        
        // set LiSa parameters
        lisa.maxVoices( conf.LISA_MAX_VOICES );
        for (int i; i < lisa.maxVoices(); i++) {
            lisa.play(i, false);
            lisa.loop(i, false);
        }

        
        return lisa;
    }
} Sound s;

while (1) {
    GG.nextFrame() => now;
    GG.dt() => float dt;

    { // input 
        if (gs.gamestate == GameState_Start) {
            float mouse_dx;
            // check to ignore the initial large mouseDelta on window start
            if (M.mag(GWindow.mouseDeltaPos()) > 500) 0 => mouse_dx;
            else GWindow.mouseDeltaPos().x => mouse_dx;

            if (GWindow.key(GWindow.Key_Left)) -conf.key_speed.val() * dt +=> mouse_dx;
            if (GWindow.key(GWindow.Key_Right)) conf.key_speed.val() * dt +=> mouse_dx;
        
            // update drop pos
            mouse_dx * conf.mouse_speed.val() * dt +=> gs.drop_pos_target.x;
            Math.clampf(gs.drop_pos_target.x, .98 * -conf.CONTAINER_HW, .98 * conf.CONTAINER_HW) => gs.drop_pos_target.x;
            // zeno interpolate to target
            gs.drop_pos + dt * 60 * conf.mouse_interp.val() * (gs.drop_pos_target - gs.drop_pos) => gs.drop_pos;

            // toggle mouse mode
            if (GWindow.keyDown(GWindow.Key_Tab)) {
                if (GWindow.mouseMode() == GWindow.MouseMode_Disabled) GWindow.mouseMode(GWindow.MouseMode_Normal);
                else GWindow.mouseMode(GWindow.MouseMode_Disabled);

                <<< "current mouse mode", GWindow.mouseMode() >>>;
            }

            if (
                gs.measure_beat_drop 
                ||
                ((GWindow.keyDown(GWindow.Key_Space) || GWindow.mouseLeftDown())
                && gs.drop_waiting_on == 0)
            ) {
                // dropBody(gs.curr_drop_type);
                // gs.next_drop_type => gs.curr_drop_type;
                // M.choose(conf.drop_selection_pdf) => gs.next_drop_type;
                drop();
                false => gs.measure_beat_drop;
            }
        }

        else if (gs.gamestate == GameState_GameOver) {
            // space to retry
            if (GWindow.keyDown(GWindow.Key_Space)) {
                enterGamestate(GameState_Start);
            }
        }
    }

    { // update
        if (gs.gamestate == GameState_Start) {
            b2World.contactEvents(gs.b2_world_id, begin_touch_events, end_touch_events, hit_events);
            // for (int i; i < hit_events.size(); i++) {
            //     hit_events[i] @=> b2ContactHitEvent @ hit_event;
            //     <<< "hit:", hit_event.shapeIdA, hit_event.shapeIdB, hit_event.point, hit_event.normal, hit_event.approachSpeed >>>;
            // }
            for (int i; i < begin_touch_events.size(); 2 +=> i) {
                begin_touch_events[i] => int id0;
                begin_touch_events[i+1] => int id1;

                // ignore collisions with the side walls
                if (
                    id0 == gs.left_wall_b2_body_id || id0 == gs.right_wall_b2_body_id
                    ||
                    id1 == gs.left_wall_b2_body_id || id1 == gs.right_wall_b2_body_id
                ) continue;

                // reset drop cooldown after collision
                if (id0 == gs.drop_waiting_on || id1 == gs.drop_waiting_on) {
                    0 => gs.drop_waiting_on; // our last drop has collided
                }

                // early out on invalid
                if (!b2Body.isValid(id0) || !b2Body.isValid(id1)) continue;

                // shortcut so we don't have to use a LUT on body_id --> pl_type
                // assume if mass is the same, they are the same body type
                Math.equal(b2Body.mass(id0), b2Body.mass(id1)) => int same_type;
                if (same_type) {
                    // TODO improve merge 

                    // get entities
                    gs.entity_map.getObj(id0) $ Entity @=> Entity e0;
                    gs.entity_map.getObj(id1) $ Entity @=> Entity e1;
                    T.assert(e0 != null, "entity not found for b2_body_id: " + id0);
                    T.assert(e1 != null, "entity not found for b2_body_id: " + id1);
                    T.assert(e0.pl_type == e1.pl_type, "merge on different types");

                    e0.pl_type == merge_types.size() - 1 => int suika;

                    // for now, don't merge highest tier
                    if (!suika) {
                        // log merge time
                        now => gs.last_merge_time;

                        // get positions
                        b2Body.position(id0) => vec2 pos0;
                        b2Body.position(id1) => vec2 pos1;
                        0.5 * (pos0 + pos1) => vec2 merge_pos;


                        // spawn new 
                        addBody(e0.pl_type + 1, merge_pos);

                        // play sound effect!
                        // (pitch according to minor penta scale)
                        s.play(e0.pl_type);

                        // ripple
                        // spork ~ fx.ripple(
                        //     merge_pos, 
                        //     merge_types[e0.pl_type].radius,
                        //     merge_types[e0.pl_type].radius * 3
                        // );

                        // add score
                        merge_types[e0.pl_type].score +=> gs.score;

                        // destroy old 
                        // (Removed from gs.entities array in gameover check)
                        // TODO: add hashmap_iter, can do away with array (returns HashmapItem which is a key : value struct)
                        e0.destroy();
                        e1.destroy();
                    }
                }
            }
            
            // updates after all merges
            -Math.FLOAT_MAX => gs.highest_body_pos;
            for (auto e : gs.entities) {
                T.assert(b2Body.isValid(e.b2_body_id), "update check body invalid");

                // update highest dropped ball
                if (e.b2_body_id != gs.drop_waiting_on) {
                    Math.max(gs.highest_body_pos, b2Body.position(e.b2_body_id).y) => gs.highest_body_pos;
                }
            }

            
            // check gameover condition
            now - gs.last_merge_time > conf.CHECK_GAMEOVER_COOLDOWN => int no_recent_merges;
            if (
                no_recent_merges 
                &&
                gs.highest_body_pos > conf.GAME_OVER_HEIGHT
            ) {
                enterGamestate(GameState_GameOver);
            }
        }
    }

    { // draw

        { // static geo
            g2d.boxFilled(
                @(-conf.CONTAINER_HW - conf.CONTAINER_THICKNESS, conf.CONTAINER_HH),
                @(-conf.CONTAINER_HW, -conf.CONTAINER_HH),
                Color.WHITE
            );

            g2d.boxFilled(
                @(conf.CONTAINER_HW, conf.CONTAINER_HH),
                @(conf.CONTAINER_HW + conf.CONTAINER_THICKNESS, -conf.CONTAINER_HH),
                Color.WHITE
            );

            g2d.boxFilled(
                @(-conf.CONTAINER_HW - conf.CONTAINER_THICKNESS, -conf.CONTAINER_HH),
                @(conf.CONTAINER_HW + conf.CONTAINER_THICKNESS, -conf.CONTAINER_HH - conf.CONTAINER_THICKNESS),
                Color.WHITE
            );
        }


        merge_types[gs.curr_drop_type] @=> MergeType curr_mt;
        // dropper
        // gs.drop_pos + @(0, curr_mt.radius + 0.5) => gs.hand_mesh.pos;

        // curr drop mesh
        // draw curr at drop pos
        curr_mt.color_map => gs.curr_drop_material.colorMap;
        curr_mt.radius * 2 => gs.curr_drop_mesh.sca;
        gs.drop_pos  => gs.curr_drop_mesh.pos;

        // update next drop mesh
        merge_types[gs.next_drop_type].color_map => gs.next_drop_material.colorMap;
        merge_types[gs.next_drop_type].radius * 2 => gs.next_drop_mesh.sca;

        // score
        gs.score_number_text.text("" + gs.score);
        gs.high_score_number_text.text("" + gs.high_score);
        
        // balls
        for (int i; i < gs.entities.size(); i++) {
            gs.entities[i] @=> Entity e;

            T.assert(b2Body.isValid(e.b2_body_id), "draw body invalid");

            // track
            b2Body.position(e.b2_body_id) => vec2 pos;
            b2Body.angle(e.b2_body_id) => float angle;
            pos => e.sprite_mesh.pos;
            angle => e.sprite_mesh.rotZ;

            // draw background body color
            // outline
            if (e.merge_type.draw_radius_mod < 1.0) {
                g2d.circle(pos, e.bounding_radius, 1, e.merge_type.bg_col);
                // <<< "outline" >>>;
                // g2d.circle(pos, e.bounding_radius, Color.WHITE);
            }

        }

        // update drawers
        if (conf.b2_debug_draw.val()) {
            b2World.draw(gs.b2_world_id, debug_draw);
        }
        debug_draw.update(); // always do this to flush the draw queue
        g2d.update();
    }


    if (me.args()) { // developer console
        UI.begin("DEV CONSOLE");

        UI.separatorText("Developer Console");

        UI.checkbox("b2DebugDraw", conf.b2_debug_draw);

        for (int i; i < 11; i++) {
            if (UI.button(i + "") && gs.gamestate == GameState_Start) dropBody(i);
        }

        UI.slider("mouse speed", conf.mouse_speed, 0.0, 1.0);
        UI.slider("mouse interp", conf.mouse_interp, 0.0, 1.0);
        UI.slider("key speed", conf.key_speed, 0.0, 1000);

        UI.end();
    }
}