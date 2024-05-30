
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
    // civs: action
    CivProduction civ_production; // what are the civs producing
    // equipment
    float equipment = GAME_STARTING_EQUIPMENT;
    // war
    vector<struct _Country *> at_war_with = {};

    // everything below this line needs to be updated in the game loop

    // factories: total
    float civs = 0.0;
    float mils = 0.0;
    float factories = 0.0;
    // owned territory
    vector<Tile*> tiles = {};
    // borders
    vector<Tile*> borders = {};
    vector<Tile*> secondary_borders = {};
    vector<Tile*> trenary_borders = {};
    map<Country*, int> bordering_countries__border_length = {};
    int borders_with_other_countries = 0; // how many tiles are bordering other countries
    // war-time borders
    int offensive_borders = 0; // how many of our tiles are being attacked
    int deffensive_borders = 0; // how many tiles are we attacking
    // units
    float base_unit_strength = 0.0;
    float deffensive_unit_strength = 0.0;
    float offensive_unit_strength = 0.0;
};

void country_print(Country* country, bool do_not_finish = false){
    cout << country->color << country->name << COL_RESET << "<" << "tiles:" << country->tiles.size() << " civs:" << country->civs << " mils:" << country->mils;

    if(!do_not_finish){
        cout << ">";
    }
}

void country_print_long(Country* country){
    country_print(country, true);
    cout << " equipment:" << country->equipment << " offensive-borders:" << country->offensive_borders << " deffensive-borders:" << country->deffensive_borders << " offensive-strength:" << country->offensive_unit_strength << " deffensive-strength:" << country->deffensive_unit_strength << ">";
}

Tile* country_get_random_tile(Country *country) {
    return vec_get_random_element(country->tiles);
}

// for example if your country is a circle: the chance of getting something in the center is higher
// TODO we could actually use the newly intrudoced secondary and trenary borders
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
