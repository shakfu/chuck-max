@import "g2d"

G2D g2d;

TextureLoadDesc tex_load_desc;
true => tex_load_desc.flip_y;
false => tex_load_desc.gen_mips; 
Texture.load(me.dir() + "./assets/flower_petals.png", tex_load_desc) @=> Texture@ sprite_tex;

vec2 random_positions[50];
vec3 random_colors[50];
for (int i; i < random_positions.size(); i++) {
	@(
		Math.random2f(-3, 3),
		Math.random2f(-3, 3)
	) => random_positions[i];
	Color.random() => random_colors[i];
}

// 1920x1080 / 4 = 480x270
// 8 => int scale;
// g2d.resolution(1920 / scale, 1080 / scale );
// g2d.antialias(false);

while (1) {
    GG.nextFrame() => now;

    g2d.backgroundColor(Color.BLACK);

    // circle outline
    // g2d.circle(@(0,0), 1.0, Color.GREEN);

    // g2d.circleFilled(@(0, 0), 1.0);
    // g2d.circleDotted(@(0, 0), 1.0, now/second, Color.WHITE);


    // circle w/ thickness
    g2d.circle(
        @(0,0), 
        1.0, Math.fabs(Math.sin(now/second)), Color.GREEN);

    // dashed line
    g2d.dashed(@(-1,1), @(1,-1), Color.WHITE, .2);

    g2d.boxFilled(@(0,0), @(1, -1), Color.BLUE);

    g2d.capsuleFilled(@(-1,1), @(1,-1), Math.fabs(Math.sin(now/second)), Color.WHITE);

    // g2d.pushFont("chugl:proggy-tiny");
    // g2d.pushColor(Color.GOLD);
    // g2d.text("PILLBUGS ATTACK", @(0,0,2), Math.sin(now/second), Math.sin(now/second));
    // g2d.pushColor(Color.GREEN);
    // g2d.text("DEFENSE", @(0,0,1), -Math.sin(now/second), -Math.sin(now/second));
    // g2d.popFont();

    8 => int num_petals;
    for (int j; j < random_positions.size(); j++) {
        g2d.pushColor(random_colors[j]);
        for (int i; i < num_petals; i++) {
            g2d.sprite(sprite_tex, random_positions[j], @(4.0/num_petals, 1.0), (now/second) + i * Math.two_pi/num_petals);
        }
        g2d.popColor();
    }
}