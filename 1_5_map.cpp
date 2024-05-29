
///////////
//////////////
///////////////// map
//////////////
///////////

#define MAP_SIZE_Y 33
#define MAP_SIZE_X 205

#define MAP_LOOPS_Y false
#define MAP_LOOPS_X false

#define MAP_TILE_BORDERS_MAX_LEN 4 // you can have 4 borders max (an assertion)

#define MAP_TILE_VALUE_MODIFIER 250.0 // increases visibility for humans

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
    // to be updated in the game loop
    bool is_border = false; // if it is bordering another country
    bool is_offensive_border = false; // if it's a border that we're attacking from
    bool is_deffensive_border = false; // if it's a border that we're deffending from
} Tile;
