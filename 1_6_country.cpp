
///////////
//////////////
///////////////// country
//////////////
///////////

typedef enum {
    CIV_PRODUCTION_CIV,
    CIV_PRODUCTION_MIL,
} CivProduction;

struct _Country {
    // misc
    string name;
    Color color;
    // factories: base
    float civs_base;
    float mils_base;
    // factories: total: those need to be updated in the game loop
    float civs = 0.0;
    float mils = 0.0;
    float factories = 0.0;
    // civs: action
    CivProduction civ_production; // what are the civs producing
    // equipment
    float equipment = GAME_STARTING_EQUIPMENT;
    // war
    vector<struct _Country *> at_war_with = {};
    // map-related: need to be updated in the game loop
    vector<Tile*> tiles = {};
    vector<Tile*> borders = {};
    vector<Country*> bordering_countries = {};
    vector<tuple<Country*, int>> bordering_countries_and_borders = {};
    int offensive_borders = 0; // how many of our tiles are being attacked
    int deffensive_borders = 0; // how many tiles are we attacking
    int borders_with_other_countries = 0; // how many tiles are bordering other countries
    // units: need to be updated in game loop
    float deffensive_unit_strength = 0.0;
    float offensive_unit_strength = 0.0;
    float average_unit_strength = 0.0; // cosmetic, do not use to determine battle outcome, mey be used for AI tho
};

void country_print(Country* country, bool do_not_finish = false){
    cout << country->color << country->name << COL_RESET << "<" << "tiles:" << country->tiles.size() << " civs:" << country->civs << " mils:" << country->mils << " equipment:" << country->equipment;
    cout << " OFB:" << country->offensive_borders << " DFB:" << country->deffensive_borders << " OUS:" << country->offensive_unit_strength << " DUS:" << country->deffensive_unit_strength; // debug

    if(!do_not_finish){
        cout << ">";
    }
}

void country_print_long(Country* country){
    country_print(country, true);
    cout << " OFB:" << country->offensive_borders << " DFB:" << country->deffensive_borders << " OUS:" << country->offensive_unit_strength << " DUS:" << country->deffensive_unit_strength << ">";
}

Tile* country_get_random_tile(Country *country) {
    return vec_get_random_element(country->tiles);
}

// for example if your country is a circle: the chance of getting something in the center is higher
pair<bool, Tile*> country_get_random_tile_based_on_density(Country* country) {
    vector<Tile*> tiles = country->tiles;

    if(tiles.size() <= 0){
        return make_pair(true, nullptr);
    }

    vector<Tile*> candidates;

    for(Tile* tile : tiles){

        int points = 1;

        points += MAP_TILE_BORDERS_MAX_LEN - tile->borders.size(); // so that if we're near a corner it counts as a good thing

        for(Tile* border_tile : tile->borders){
            if(border_tile->owner == country){
                points += 1;
            }
        }

        for(int i=0; i<points; ++i){
            candidates.push_back(tile);
        }
    }

    return make_pair(false, vec_get_random_element(candidates));
}
