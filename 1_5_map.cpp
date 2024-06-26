
///////////
//////////////
///////////////// map
//////////////
///////////

#define MAP_SIZE_Y 33
#define MAP_SIZE_X 205

#define MAP_LOOPS_Y false
#define MAP_LOOPS_X true

#define MAP_TILE_BORDERS_MAX_LEN 4 // you can have 4 borders max (an assertion)

// insentivise players to attack `nobody` by providing some resources for all tiles
#define MAP_TILE_INITIAL_CIVS 0.006
// we could do the same for the mils, the problem is - this would increase `nobody`'s equipment, so the AI
// would be discouraged from attacking

typedef struct _Tile {
    Country* owner;
    vector<struct _Tile*> borders;
    // number of factories
    float civs = MAP_TILE_INITIAL_CIVS;
    float mils = 0;

    // everything below this line needs to be updated in the game loop

    bool is_offensive_border = false; // if it's a border that we're attacking from
    bool is_deffensive_border = false; // if it's a border that we're deffending from
    bool is_border = false; // if it is bordering another country
    bool is_secondary_border = false; // is it bordering a border
    bool is_trenary_border = false;
    // bool is_trenary_border = false; // is it bordering a secondary border
    // float equipment = 0; // the owner can put some of it's equipment on a tile
} Tile;
