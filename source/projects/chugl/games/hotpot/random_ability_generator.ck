GOrbitCamera cam --> GG.scene();
GG.scene().camera(cam);
[
    "Ingredient added",
    "Ingredient removed / consumed",
    "Ingredient overcooked",
    "At end of round",
    "Triggers every N flavor points that have accumulated (on this ingredient or overall)",
    "On shop reroll",
    "On ingredient level up",
    "Is very first ingredient added during round",
    "Is very last ingredient added during round",
] @=> string triggers[];

[
    "N other ingredients from <TIER> also cooking",
    "N other ingredients from <CATEGORY> also cooking",
    "N other ingredients with same name",
    "# ingredients from <TIER> cooked previously",
    "# ingredients from <CATEGORY> cooked previously",
    "# ingredients with same name cooked previously",
    "Time passes (no condition)",
    "Pot below N% capacity or N items",
    "Pot at exactly N items",
    "Pot above N% capacity or N items",
    "(stomach) Player stomach fullness below N",
    "(stomach) Player stomach fullness above N",
    "# of overcooked ingredients currently in pot",
    "# of overcooked ingredients previously",
    "# of <TIER> ingredients cooked previously",
    "# of <INGREDIENT> cooked previously",
    "# of <CATEGORY> ingredients cooked previously",
] @=> string conditions[];

[
    "Immediate +X FPs",
    "FP rate",
    "FP Rate +MULT",
    "FP Rate xMULT",
    "Ingredient Volume ",
    "Pot Capacity",
    "Time Remaining",
    "Cook Time",
    "Money $$$",
    "Stomach Capacity",
    "Shop Reroll",
    "Shop Items",
] @=> string effects[];

[
    "THIS ingredient",
    "All other ingredients in pot",
    "Random ingredient in pot",
    "<CATEGORY> ingredients in pot",
    "<TIER> ingredients in pot",
    "<NAME> ingredients in pot",
    "All ingredients added after",
    "All ingredients added before",
    "Broth",
    "Pot",
    "Shop",
    "<CATEGORY> ingredients this run",
    "<TIER> ingredients this run",
    "<NAME> ingredients this run",
] @=> string targets[];

fun string choose(string arr[]) {
    return arr[Math.random2(0, arr.size()-1)];
}

GText ability_text --> GG.scene();
ability_text.sca(.3);

fun void generate() {

    /*
    40% ability
    40% active
    20% both
    */
    Math.randomf() => float ability_roll;
    "IF " => string ability;
    if (ability_roll < .4) { // trigger
        ability + choose(triggers) => ability;
    } else if (ability_roll < .8) { // active
        ability + choose(conditions) => ability;
    } else { // both
        ability + choose(triggers) + " AND/OR " => ability;
        ability + choose(conditions) => ability;
    }

    ability + " CAUSES " + choose(targets) + " TO EFFECT " + choose(effects) + "\n" => ability;
    cherr <= ability;

    ability => ability_text.text;
}


while (true) {
    // TODO chugl without Window mode?
    GG.nextFrame() => now;

    if (GWindow.keyDown(GWindow.Key_Down)) generate();
}