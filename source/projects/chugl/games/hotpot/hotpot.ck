/*
Ideas
- pot temperature should be a factor, can upgrade heat source?
    - food takes N units to cook, where units is determined by power of pot
    - a fuller pot means everything in it cooks slower
- broth levels decreasing, how does this effect the game?
- sauce bar
- beverages (tea)
    - separate game idea: tea brewing
- restaurant stocking / operations

- after food is done, you have to click to take it out of pot?
    - food takes same amount of energy to burn as it does to cook. If burned it's removed and you get no flavor points (but still have to eat it and it takes up volume)
    - research: automated chopsticks to take food out for you (at certain rate)

Story
- Ge: "Can you add moral dilemmas like Papers Please?"
- passive clickers are almost by definition the most uncritical genre of game,
meaning that there is an accentuated *potential* for this game to impart a meaningful
story, a critical lens...
    - the game should begin at first firmly rooted in the genre. Mindless clicking
    and making a number--your "Flavor Points"--grow big. Easy enough to accept
    - gradually, the game unfurls and reveals a narrative 
- Important Qs
    - what are flavor points, really?
    - how do you have so much purchasing power?
    - What is the story / commentary we are trying to make?

TODO
- napa cabbage buff (boosts broth flavor somehow?)
- phone screen with texts to progress narratives / unlocks
- get the combo desc string in the hover of the ingredient menu

*/

// Window
// GWindow.windowed(1920 /2, 1080 /2);
GWindow.maximize();

// entity type enum
0 => int EntityType_Food;

// room type enum
0 => int RoomType_Eat;
1 => int RoomType_Upgrade;

[
    "Eat",
    "Upgrade"
] @=> string room_type_names[];

// ======================================================================
// Food Types
// ======================================================================

1 => int Tier_1;
2 => int Tier_2;
3 => int Tier_3;
4 => int Tier_4;
5 => int Tier_5;
6 => int Tier_6;

// Food Category bit flags
(1 << 1) => int FoodCategory_Vegetable;
(1 << 2) => int FoodCategory_Mushroom;
(1 << 3) => int FoodCategory_Meat;
(1 << 4) => int FoodCategory_Seafood;
(1 << 5) => int FoodCategory_Protein;
(1 << 6) => int FoodCategory_Carb;
(1 << 7) => int FoodCategory_Legendary;

class FoodType
{
    string name;
    float volume;        // amt of space the food takes up (in pot and stomach)
    float base_fp_rate;  // fp/sec that this food accumulates while cooking
    dur cook_time; // time to cook to completion
    dur overcook_dur; 
    int tier;
    int category; // FoodCategory flag
}

FoodType foods[0];
FoodType@ foods_tiered[7][0]; // 7 to prevent off by 1

fun FoodType FoodType_Add(string n, int category, float b, dur c, int tier) {
    FoodType food_type;
    // construct
    n => food_type.name;
    b => food_type.base_fp_rate;
    c => food_type.cook_time;
    .5 * food_type.cook_time => food_type.overcook_dur;
    1.0 => food_type.volume;  // for now lock all food to 1 unit of volume (simplification, like TFT teams with 6 slots)
    tier => food_type.tier;
    category => food_type.category;
    
    T.assert(category <= FoodCategory_Legendary << 1, "Invalid category " + category);
    

    // add to list
    foods << food_type;

    // add to tier list
    foods_tiered[tier] << food_type;

    return food_type;
}

// Vegetables
FoodType_Add("Spinach", FoodCategory_Vegetable, 1, 5::second, Tier_1) @=> FoodType FoodType_Spinach;
FoodType_Add("Cabbage", FoodCategory_Vegetable, 2, 12::second, Tier_1) @=> FoodType FoodType_Cabbage;
FoodType_Add("Potato", FoodCategory_Vegetable, 2, 20::second, Tier_1) @=> FoodType FoodType_Potato;
FoodType_Add("Crown Daisy (Tong Hao)", FoodCategory_Vegetable, 3, 15::second, Tier_2) @=> FoodType FoodType_CrownDaisy;
FoodType_Add("Bok Choy", FoodCategory_Vegetable, 2, 8::second, Tier_2) @=> FoodType FoodType_BokChoy;
FoodType_Add("Pumpkin", FoodCategory_Vegetable, 2, 16::second, Tier_2) @=> FoodType FoodType_Pumpkin;
FoodType_Add("Taro", FoodCategory_Vegetable, 2, 15::second, Tier_3) @=> FoodType FoodType_Taro;
FoodType_Add("Lotus Root", FoodCategory_Vegetable, 2, 16::second, Tier_4) @=> FoodType FoodType_LotusRoot;
// Mushrooms
FoodType_Add("Shiitake Mushroom", FoodCategory_Mushroom, 2, 10::second, Tier_3) @=> FoodType FoodType_Shiitake;
FoodType_Add("Wood Ear Mushroom", FoodCategory_Mushroom, 2, 7::second, Tier_4) @=> FoodType FoodType_WoodEar;
FoodType_Add("Enoki Mushroom", FoodCategory_Mushroom, 2, 9::second, Tier_5) @=> FoodType FoodType_Enoki;
// Meat

// Upgrade Type enum
// 0 => int UpgradeType_Workout; // maybe working out is *Training* not an Upgrade
// 0 => int UpgradeType_PotSize;
// class Upgrade
// {
// }


// ComboType enum
0 => int ComboType_Cabbage;
1 => int ComboType_Count;

string combo_descs[ComboType_Count];
"5x Cabbage: apply 1.2x flavor mult to pot" => combo_descs[ComboType_Cabbage];


class Entity
{
    // type system
    int entity_type;
    FoodType@ food_type;

    // cooking stats
    float accum_fp;           // the accumulated amount of FPs
    dur accum_cook_time;      // amount of time it's been cooked for
    int finished_cooking;   
    time finished_cooking_time;    // time that food reached 100% cooked
    float finished_cooking_fp;    // max FPs upon finishing cooking
    1.0 => float fp_mult; // the multiplier on the ingredient base FP rate


    fun static Entity makeFood(FoodType@ food_type) {
        Entity e;
        EntityType_Food => e.entity_type;
        food_type @=> e.food_type;
        return e;
    }
}

class GameState
{
    false => int game_state_initialized;
    
    // game progress + run metrics
    1 => int day_curr;
    21 => int day_max;

    // active room 
    UI_Int active_room_type;

    // player stats
    float flavor_points;
    UI_Int skill_points;
    32.0 => float stomach_max_cap;
    0.0 => float stomach_curr_cap;

    UI_Float player_vegetable_mult(1.0);
    UI_Float player_mushroom_mult(1.0);
    UI_Float player_meat_mult(1.0);
    UI_Float player_seafood_mult(1.0);
    UI_Float player_protein_mult(1.0);
    UI_Float player_carb_mult(1.0);

    // pot stats
    8.0 => float pot_max_cap;
    0.0 => float pot_curr_cap;

    // Shop Menu
    FoodType@ menu_items[8];

    // combos
    0.0 => float combo_multiplier;
    int active_combos[0];

    // currently cooking
    Entity cooking[0];

    // dev console
    UI_Float dev_flavor_points;
    UI_Float dev_time_factor(1.0);
    UI_Float font_global_scale(1.25);
    UI_Float dev_stomach_curr_cap(stomach_curr_cap);
    UI_Float dev_pot_curr_cap(pot_curr_cap);

    // Colors
    UI.convertHSVtoRGB(@(2.0/7, 0.7, 0.7)) => vec3 Color_ItemUnlocked;
    @(
        Color_ItemUnlocked.x,
        Color_ItemUnlocked.y,
        Color_ItemUnlocked.z,
        1.0
    ) => vec4 Color_ItemUnlockedVec4;
    UI.convertHSVtoRGB(@(0.0/7, 0.7, 0.7)) => vec3 Color_ItemTooExpensive;
    @(
        Color_ItemTooExpensive.x,
        Color_ItemTooExpensive.y,
        Color_ItemTooExpensive.z,
        1.0
    ) => vec4 Color_ItemTooExpensiveVec4;
    UI.convertHSVtoRGB(@(4.0/7, 0.7, 0.7)) => vec3 Color_ItemAffordable;


    Color.GRAY => vec3 Color_Tier1;
    UI.convertHSVtoRGB(@(4.0/7, 0.7, 0.7)) => vec3 Color_Tier2;
    UI.convertHSVtoRGB(@(2.0/7, 0.7, 0.7)) => vec3 Color_Tier3;
    UI.convertHSVtoRGB(@(5.0/7, 0.7, 0.7)) => vec3 Color_Tier4;
    UI.convertHSVtoRGB(@(0.0/7, 0.7, 0.7)) => vec3 Color_Tier5;
    UI.convertHSVtoRGB(@(1.0/7, 0.7, 0.7)) => vec3 Color_Tier6;
    [
        Color.BLACK,
        Color_Tier1,
        Color_Tier2,
        Color_Tier3,
        Color_Tier4,
        Color_Tier5,
        Color_Tier6,
    ] @=> vec3 Color_Tiers[];

    fun FoodType _chooseMenuItem() {
        // TODO base selection from tier list + day
        // for now just doing a fixed pdf 
        // Tier 1: 40%
        // Tier 2: 26%
        // Tier 3: 18%
        // Tier 4: 10%
        // Tier 5: 5%
        // Tier 6: 1%
        [.40, .26, .18, .10, .05, .01] @=> float pdf[];
        // corresponding CDF:
        float cdf[pdf.size()];
        pdf[0] => cdf[0];
        for (1 => int i; i < pdf.size(); i++) {
            pdf[i] + cdf[i - 1] => cdf[i];
        }

        // get random number, find which tier bucket it falls into
        Tier_6 => int tier; // default tier
        Math.randomf() => float rand;
        for (int i; i < cdf.size(); i++) {
            if (rand <= cdf[i]) {
                i + 1 => tier;
                break;
            }
        }
        T.assert(tier > 0 && tier <= Tier_6, "Invalid tier: " + tier);
        T.printArray(cdf);

        // pick random food from that tier
        foods_tiered[tier].size() => int num_foods_in_tier;
        if (num_foods_in_tier == 0) {
            // edge case: no foods in this tier, return random
            return foods[Math.random2(0, foods.size() - 1)];
        } else {
            return foods_tiered[tier][Math.random2(0, num_foods_in_tier - 1)];
        }
    }

    fun void rerollMenu() {
        for (int i; i < menu_items.size(); i++) {
            _chooseMenuItem() @=> menu_items[i];
        }
    }

    // selects a new menu item at index i
    fun void rerollMenu(int i) {
        // choose random
        _chooseMenuItem() @=> menu_items[i];
    }

    fun float clamp01(float f) {
        return Math.clampf(f, 0, 1);
    }

    fun vec3 lerp(vec3 a, vec3 b, float t) {
        return a * (1-t) + b * t;
    }

    fun void tooltip(FoodType category) {
        if (UI.beginItemTooltip()) {
            UI.text("Volume: " + category.volume);
            UI.text("Flavor Points: " + category.base_fp_rate + "/second");
            UI.text("Cook Time: " + (category.cook_time / second) + " seconds");
            UI.endTooltip();
        }
    }

    fun void playerStatUpgrade(string text, UI_Float stat) {
        if (UI.button("Upgrade##" + text) && skill_points.val() > 0) {
            skill_points.val() - 1 => skill_points.val;
            stat.val() + 1 => stat.val;
        }
        UI.sameLine();
        UI.text(text + player_vegetable_mult.val());
    }

    fun int coloredButton(string text, vec3 color) {
        return coloredButton(text, color, @(0, 0));
    }

    fun int coloredButton(string text, vec3 color, vec2 size) {
        UI.pushStyleColor(UI_Color.Button, color * .9);
        UI.pushStyleColor(UI_Color.ButtonHovered, color);
        UI.pushStyleColor(UI_Color.ButtonActive, color * 1.1);

        UI.button(text, size) => int clicked;
        UI.popStyleColor(3);
        return clicked;
    }

    fun int centeredButton(string text, vec2 size, vec3 color) { // TODO put in ulib_imgui
        UI.pushStyleColor(UI_Color.Button, color * .9);
        UI.pushStyleColor(UI_Color.ButtonHovered, color);
        UI.pushStyleColor(UI_Color.ButtonActive, color * 1.1);

        UI.getContentRegionAvail().x => float avail_width;
        UI.setCursorPosX(UI.getCursorPosX() + (avail_width - size.x) * .5);

        UI.button(text, size) => int clicked;

        UI.popStyleColor(3);
        return clicked;
    }

    fun void init() {
        if (game_state_initialized) return;
        GG.nextFrame() => now;

        true => game_state_initialized;
        
        // TODO: broken
        // me.dir() + "../../../assets/fonts/DroidSans.ttf" => string droid_sans_filepath;
        // UI.addFontFromFileTTF(droid_sans_filepath, 16) @=> UI_Font droid_sans_font;

        // set menu items
        rerollMenu();
    }

    fun void update(float dt_sec)
    {
        dev_time_factor.val() *=> dt_sec;

        if (active_room_type.val() == RoomType_Eat) {
            { // scan for combos
                // combo reset
                active_combos.clear();
                T.assert(active_combos.size() == 0, "active combos not reset");
                0.0 => combo_multiplier;

                int num_cabbage;
                for (int i; i < cooking.size(); i++) {
                    cooking[i] @=> Entity food;

                    // ComboType_Cabbage
                    if (food.food_type == FoodType_Cabbage) num_cabbage++;
                }
                // ComboType_Cabbage
                if (num_cabbage >= 5) {
                    active_combos << ComboType_Cabbage;
                    // apply the score mult
                    1.2 +=> combo_multiplier;
                }
            }

            // process foods
            // ==optimize== double buffer or swap with last instead of deletion
            for (int i; i < cooking.size(); i++) {
                cooking[i] @=> Entity food;
                food.food_type @=> FoodType food_type;

                // clamp to max time
                Math.clampf((food_type.cook_time - food.accum_cook_time) / second, 0.0, dt_sec) => float remaining_sec;
                <<< remaining_sec >>>;
                remaining_sec::second => dur remaining_dur;

                // add cook time
                dt_sec::second +=> food.accum_cook_time;

                // calculate final combo multiplier
                0 => float player_stat_mult;
                { // add player stats
                    if (food_type.category & FoodCategory_Vegetable) { player_vegetable_mult.val() +=> player_stat_mult; }
                    if (food_type.category & FoodCategory_Mushroom) {  player_mushroom_mult.val() +=> player_stat_mult; }
                    if (food_type.category & FoodCategory_Meat) {  player_meat_mult.val() +=> player_stat_mult; }
                    if (food_type.category & FoodCategory_Seafood) {  player_seafood_mult.val() +=> player_stat_mult; }
                    if (food_type.category & FoodCategory_Protein) {  player_protein_mult.val() +=> player_stat_mult; }
                    if (food_type.category & FoodCategory_Carb) {  player_carb_mult.val() +=> player_stat_mult; }
                }
                combo_multiplier + player_stat_mult => food.fp_mult;

                // accumulate points from cooking
                food.fp_mult * remaining_sec * food_type.base_fp_rate +=> food.accum_fp; 

                // mark if done cooking
                if (!food.finished_cooking && food.accum_cook_time > food_type.cook_time) {
                    true => food.finished_cooking;
                    now - dt_sec::second + remaining_dur => food.finished_cooking_time;
                    food.accum_fp => food.finished_cooking_fp;
                }

                // lose points from overcooking
                if (food.finished_cooking) {
                    food.finished_cooking_fp * clamp01(1.0 - ((now - food.finished_cooking_time) / food_type.overcook_dur)) => food.accum_fp;
                }
            }
        }
    }

    // returns true if successfully added to pot
    fun int addToPot(FoodType@ food_type) {
        // volume check
        if (pot_curr_cap + food_type.volume > pot_max_cap) return false;

        // fullness check
        if (stomach_curr_cap + food_type.volume > stomach_max_cap) return false;

        // else add
        food_type.volume +=> pot_curr_cap;
        cooking << Entity.makeFood(food_type);

        return true;
    }

    // for now drawing also handles user input...
    fun void draw()
    {
        UI.getStyle() @=> UI_Style ui_style;

        // fullscreen
        UI.getMainViewport() @=> UI_Viewport @ viewport;
        UI.setNextWindowPos(viewport.workPos(), UI_Cond.Always);
        UI.setNextWindowSize(viewport.workSize(), UI_Cond.Always);
        UI.pushStyleVar(UI_StyleVar.WindowRounding, 0.0);
        if (UI.begin("HOTPOT HERO", null, UI_WindowFlags.NoDecoration )) {
            UI.getWindowSize() => vec2 window_size;

            { // left pane
                UI.getContentRegionAvail() => vec2 pane_size;

                false => int ui_disabled;
                if (active_room_type.val() != RoomType_Eat) {
                    UI.beginDisabled();
                    true => ui_disabled;
                }

                UI.beginChild(
                    "EAT",
                    @(window_size.x * .5, window_size.y * .75),
                    // @(0,0),
                    UI_ChildFlags.Border,
                    0
                );

                "Day " + day_curr + "/" + day_max => string day_string;
                UI.text(day_string);
                UI.text("Flavor Points: " + (flavor_points));
                UI.text("Skill Points: " + skill_points.val());

                // stomach capacity
                (stomach_curr_cap $ int) + "/" + (stomach_max_cap $ int) => string stomach_cap_str;
                UI.progressBar(stomach_curr_cap / stomach_max_cap, @(0,0), null);
                UI.sameLine();
                if (active_room_type.val() != RoomType_Eat && stomach_curr_cap > 0) {
                    UI.textColored(Color_ItemUnlockedVec4, "DIGESTING " + stomach_cap_str);
                } else {
                    UI.text("Stomach " + stomach_cap_str);
                }

                // pot capacity
                UI.progressBar(pot_curr_cap / pot_max_cap, @(0,0), null);
                UI.sameLine();
                UI.text("Pot Capacity " + (pot_curr_cap $ int) + "/" + (pot_max_cap $ int));

                UI.separatorText("Menu");  // -------------MENU--------------
                if (UI.button("Reroll")) {
                    rerollMenu();
                } 

                // Menu Shop
                UI.getWindowWidth() => float curr_window_width;
                for (int menu_idx; menu_idx < menu_items.size(); menu_idx++) {
                    menu_items[menu_idx] @=> FoodType food;

                    UI.pushID(menu_idx);

                    // USER INPUT
                    if (coloredButton(food.name, Color_Tiers[food.tier], @(curr_window_width / 4.15, 80))) {
                        if (addToPot(food)) {
                            // reselect that menu item
                            rerollMenu(menu_idx);
                        } else {
                            // TODO: display reason for the add failing

                        }
                    }

                    UI.popID();

                    if (menu_idx != 3 && menu_idx != 7) UI.sameLine();
                }


                { // -------------Cooking--------------
                    UI.beginChild(
                        "COOKING",
                        // @(window_size.x * .5,0),
                        // @(-1,window_size.y * .75),
                        @(pane_size.x * .25, 0),
                        // @(size.x * .2, 0), 
                        UI_ChildFlags.Border,
                        0
                    );

                    UI.separatorText("COOKING"); 
                    for (int i; i < cooking.size(); i++) {
                        cooking[i] @=> Entity food;
                        food.food_type @=> FoodType category;

                        // check finished
                        if (food.accum_cook_time >= category.cook_time) {
                            // overcook meter
                            clamp01(1.0 - (now - food.finished_cooking_time) / category.overcook_dur) => float overcook_progress;
                                UI.progressBar(overcook_progress, @(0,0), (food.accum_fp $ int) + " flavor");
                            UI.sameLine();


                            // lerp color between affordable and item too expensive
                            lerp(Color_ItemTooExpensive, Color_ItemAffordable, overcook_progress) => vec3 eat_button_col;
                            if (coloredButton("EAT " + category.name + "##" + i, eat_button_col)) {
                                // get the ratio of this you can it
                                Math.clampf((stomach_max_cap - stomach_curr_cap)  / category.volume, 0.0, 1.0) => float ratio;

                                // add points
                                (ratio * food.accum_fp) +=> flavor_points;
                                // add to stomach
                                (ratio * category.volume) +=> stomach_curr_cap;
                                // remove from pot
                                category.volume -=> pot_curr_cap;

                                cooking.popOut(i); // SLOW!!!
                                i--;

                                // sanity checks
                                T.assert(pot_curr_cap >= 0 && pot_curr_cap <= pot_max_cap, "pot capacity error: curr/max" + pot_curr_cap + "/" + pot_max_cap);
                            }
                        } else {
                            UI.progressBar(
                                food.accum_cook_time / category.cook_time, @(0,0), (food.accum_fp $ int) + " flavor X " + food.fp_mult + " mult"
                            );
                            UI.sameLine();
                            UI.text(category.name);
                        }
                    }

                    UI.endChild();
                }

                UI.sameLine();

                { // -------------Combos--------------
                    UI.beginChild(
                        "COMBOS",
                        // @(window_size.x * .5,0),
                        // @(-1,window_size.y * .75),
                        @(pane_size.x * 0.5,0),
                        // @(size.x * .2, 0), 
                        UI_ChildFlags.Border,
                        0
                    );

                    UI.separatorText("COMBOS"); 

                    for (int i; i < active_combos.size(); i++) {
                        UI.bulletText(combo_descs[active_combos[i]]);
                    }

                    UI.endChild();
                }



                // check end game (stomach full)
                Math.fabs(stomach_curr_cap - stomach_max_cap) < .0001 => int stomach_full;
                if (stomach_full) {
                    if (centeredButton("MAI DAN", @(pane_size.x * .2, 40), Color.GOLD)) {
                        // transition out to upgrade screen
                        RoomType_Upgrade => active_room_type.val;
                    }
                }

                UI.endChild(); // "Left Generator"

                if (ui_disabled) {
                    UI.endDisabled();
                }
            }

            UI.sameLine();

            { // right pane
                UI.getContentRegionAvail() => vec2 pane_size;

                // room check
                false => int ui_disabled;
                if (active_room_type.val() != RoomType_Upgrade) {
                    true => ui_disabled;
                    UI.beginDisabled();
                }

                UI.beginChild(
                    "UPGRADE",
                    // @(window_size.x * .5,0),
                    @(-1,window_size.y * .75),
                    // @(size.x * .2, 0), 
                    UI_ChildFlags.Border,
                    0
                );

                UI.separatorText("Body Upgrades");

                playerStatUpgrade("Vegetable Mult: ", player_vegetable_mult);
                playerStatUpgrade("Mushroom Mult: ", player_mushroom_mult);
                playerStatUpgrade("Meat Mult: ", player_meat_mult);
                playerStatUpgrade("Seafood Mult: ", player_seafood_mult);
                playerStatUpgrade("Protein Mult: ", player_protein_mult);
                playerStatUpgrade("Carbs Mult: ", player_carb_mult);


                // TODO: should there be progress bars / time requirements for these too?
                (flavor_points >= 2000) => int work_out_affordable;
                if (work_out_affordable) {
                    if (coloredButton("Work Out (-2000 Flavor Points) (+10 stomach capacity)", Color_ItemAffordable)) {
                        2000 -=> flavor_points;
                        10 +=> stomach_max_cap;
                    }
                    UI.sameLine();
                    UI.text("2000 FPs");
                } else {
                    coloredButton("Work Out (+10 stomach capacity)", Color_ItemTooExpensive);
                    UI.sameLine();
                    UI.textColored(Color_ItemTooExpensiveVec4, "2000 FPs");
                }

                UI.separatorText("Pot Upgrades");

                UI.separatorText("Food Upgrades");
                for (int i; i < foods.size(); i++) {
                    foods[i] @=> FoodType category;
                    // TODO: per-ingredient upgrades? (like in balatro you can upgrade certain hand types?)
                }

                UI.dummy(@(0.0f, 20.0f)); // vertical spacing

                if (centeredButton("LET'S EAT", @(pane_size.x * .2, 40), Color.GOLD)) {
                    // transition back to eat
                    RoomType_Eat => active_room_type.val;
                    day_curr++;
                    0 => stomach_curr_cap; // reset stomach
                }
                
                UI.dummy(@(0.0f, 20.0f)); // vertical spacing


                { // Messages
                    UI.beginChild(
                        "MESSAGES",
                        // @(window_size.x * .5,0),
                        // @(-1,window_size.y * .75),
                        @(0,0),
                        // @(size.x * .2, 0), 
                        UI_ChildFlags.Border,
                        0
                    );
                    UI.separatorText("Messages");
                    UI.endChild();
                }


                UI.endChild();

                if (ui_disabled) {
                    UI.endDisabled();
                }
            }

            // developer console
            {
                UI.beginChild(
                    "DEV CONSOLE",
                    // @(window_size.x * .5,0),
                    @(0, 0),
                    // @(size.x * .2, 0), 
                    UI_ChildFlags.Border,
                    0
                );

                UI.separatorText("Developer Console");

                if (UI.listBox("Active Room", active_room_type, room_type_names)) {
                    <<< "setting room to: ", room_type_names[active_room_type.val()] >>>;
                }

                flavor_points => dev_flavor_points.val;
                if (UI.drag("Flavor Points", dev_flavor_points)) {
                    dev_flavor_points.val() => flavor_points;
                }

                UI.drag("Skill Points", skill_points);

                stomach_curr_cap => dev_stomach_curr_cap.val;
                if (UI.slider("Stomach Fullness", dev_stomach_curr_cap, 0, stomach_max_cap)) {
                    dev_stomach_curr_cap.val() => stomach_curr_cap;
                }

                pot_curr_cap => dev_pot_curr_cap.val;
                if (UI.slider("Pot Fullness", dev_pot_curr_cap, 0, pot_max_cap)) {
                    dev_pot_curr_cap.val() => pot_curr_cap;
                }

                UI.separatorText("Stats");
                UI.drag("player_vegetable_mult", player_vegetable_mult);
                UI.drag("player_mushroom_mult", player_mushroom_mult);
                UI.drag("player_meat_mult", player_meat_mult);
                UI.drag("player_seafood_mult", player_seafood_mult);
                UI.drag("player_protein_mult", player_protein_mult);
                UI.drag("player_carb_mult", player_carb_mult);

                UI.slider("Time Scale", dev_time_factor, .1, 10);

                UI.slider("Font Scale", font_global_scale, .1, 5);
                font_global_scale.val() => UI_IO.fontGlobalScale;

                // TODO: add font support eventually...
                // see ImFontAtlas, ImFont, ImFontConfig in cimgui
                UI.showFontSelector("Font");

                UI.endChild();
            }

        }
        UI.end();
        UI.popStyleVar(1); // WindowRounding = 0.0
    }


}

GameState gs;
gs.init();

while (true) {
    GG.nextFrame() => now;

    gs.update(GG.dt());
    gs.draw();
}