
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
    float average_unit_strength = 0.0; // needs to be updated in the game loop
    // map-related: need to be updated in the game loop
    vector<Tile*> tiles = {};
    vector<Country*> bordering_countries = {}; // needs to be updated in the game loop
};

void country_print(Country* country){
    cout << country->color << country->name << COL_RESET << "<" << "tiles:" << country->tiles.size() << " civs:" << country->civs << " mils:" << country->mils << " equipment:" << country->equipment << " AUS:" << country->average_unit_strength << ">";
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
