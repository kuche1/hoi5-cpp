
// mouse event tracking: https://invisible-island.net/xterm/ctlseqs/ctlseqs.html#h3-Any-event-tracking
// https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797?permalink_comment_id=3878578
// only get mouse clicks with: code 9 (`h` to turn on; `l` to turn off) (printf "\e[?9h"); I notices that this works in bash and not in fish

// would be cool if we could use a framebuffer of some sort, to shooth out the refreshing

#include <stdint.h>
#include <stdio.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <cmath>
#include <random>
#include <cassert>
#include <ranges>

using namespace std;

struct _Country;
typedef struct _Country Country;

///////////
//////////////
///////////////// generic
//////////////
///////////

#define LENOF(comptime_arr) \
    (sizeof(comptime_arr) / sizeof(*comptime_arr))

float random_0_to_1() {
    // Seed the random number generator
    random_device rd;
    mt19937 gen(rd());
    
    // Define a uniform distribution over the range [0, 1]
    uniform_real_distribution<float> dist(0.0, 1.0);
    
    // Generate a random number
    float randomNum = dist(gen);

    return randomNum;
}

// range is inclusive
int random_int(int from, int to) {
    // Random number generation setup
    std::random_device rd;  // Obtain a random number from hardware
    std::mt19937 gen(rd()); // Seed the generator
    std::uniform_int_distribution<> distr(from, to); // Define the range

    // Generate a random index
    int random_number = distr(gen);

    return random_number;
}

template<typename T>
bool vec_contains(const vector<T>& vec, const T& element) {
    return find(vec.begin(), vec.end(), element) != vec.end();
}

template<typename T>
void vec_push_back_nodup(vector<T>& vec, const T& element) {
    if(!vec_contains(vec, element)){
        vec.push_back(element);
    }
}

template<typename T>
void vec_remove_if_exist(vector<T>& vec, const T& element) {
    if(vec_contains(vec, element)){
        auto it = find(vec.begin(), vec.end(), element);
        vec.erase(it);
    }
}

template<typename T>
T vec_get_random_element(vector<T>& vec) {
    int length = vec.size();
    assert(length > 0);
    int index = random_int(0, length - 1);
    return vec[index];
}

void term(char *command) {
    int ret_code = system(command);
    if(ret_code != 0){
        cerr << "ERROR: command failed: " << command << '\n';
        exit(1);
    }
}

///////////
//////////////
///////////////// ansi escape codes / terminal settings
//////////////
///////////

// https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797

typedef char* Color;

#define DISP_CLEAR "\033[H\033[J"

#define COL_RESET ((Color)"\033[0m")

#define COL_YELLOW_DARK ((Color)"\033[33m")

#define COL_INT(color_number) ((Color)"\033[38;5;" #color_number "m")

// mouse click

void terminal_mouse_click_log_enable() {
    printf("\033[?1006h");
}

void terminal_mouse_click_log_disable() {
    printf("\033[?1006l");
}

// echo

void terminal_echo_enable() {
    term((char*)"stty echo");
}

void terminal_echo_disable() {
    term((char*)"stty -echo");
}

// line buffering

void terminal_line_buffering_enable() {
    term((char*)"stty icanon");
}

void terminal_line_buffering_disable() {
    term((char*)"stty -icanon");
}

///////////
//////////////
///////////////// map
//////////////
///////////

#define MAP_SIZE_Y 30
#define MAP_SIZE_X 205

#define MAP_LOOPS_Y false
#define MAP_LOOPS_X false

#define MAP_TILE_VALUE_MODIFIER 250.0

// typedef enum {
//     TILE_TYPE_PLAIN,
//     TILE_TYPE_URBAN,
//     TILE_TYPE_FOREST,
// } TileType;

typedef struct _Tile {
    Country* owner;
    // TileType type;
    vector<struct _Tile*> borders;
    // number of factories
    float civs;
    float mils;
} Tile;

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
    // factories: total
    float civs = 0.0; // those need to be updated in the game loop
    float mils = 0.0;
    // civs: action
    CivProduction civ_production; // what are the civs producing
    // equipment
    float equipment; // I plan on allowing for a defficit
    // war
    vector<struct _Country *> at_war_with;
    // tiles
    int tiles = 0; // needs to be updated in the game loop
};

vector<Tile*> country_get_tiles(Country *country, vector<vector<Tile>> *map) {
    vector<Tile*> owned_tiles;

    for(int y=0; y<MAP_SIZE_Y; ++y){
        for(int x=0; x<MAP_SIZE_X; ++x){
            Tile *tile = &((*map)[y][x]);
            if(tile->owner == country){
                owned_tiles.push_back(tile);
            }
        }
    }

    return owned_tiles;
}

Tile* country_get_random_tile(Country *country, vector<vector<Tile>> *map) {
    vector<Tile*> tiles = country_get_tiles(country, map);
    return vec_get_random_element(tiles);
}

// for example if your country is a circle: the chance of getting something in the center is higher
pair<bool, Tile*> country_get_random_tile_based_on_density(Country* country, vector<vector<Tile>>* map) {
    vector<Tile*> tiles = country_get_tiles(country, map);

    if(tiles.size() <= 0){
        return make_pair(true, nullptr);
    }

    vector<Tile*> candidates;

    for(Tile* tile : tiles){

        int points = 1;

        points += 4 - tile->borders.size(); // so that if we're near a corner it counts as a good thing

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

///////////
//////////////
///////////////// gameplay
//////////////
///////////

#define GAME_CIV_PRODUCE(number_of_civs) (floor(number_of_civs) * 0.0002)

#define GAME_MIL_PRODUCE(number_of_mils) (floor(number_of_mils) * 20.0)

#define GAME_ATK_WIN_CHANCE 0.15 // what is the change (0.0 to 1.0) that a terriroty would be gained upon attack

#define GAME_ATK_EQUIPMENT_COST 16.0 // how much equipment a single attack costs
#define GAME_DEF_EQUIPMENT_COST (GAME_ATK_EQUIPMENT_COST * 0.15) // how much equipment does it cost to deffend an attack

#define GAME_DEF_NO_EQUIPMENT_MULTIPLIER 0.6

#define GAME_PERCENT_FACTORIES_DESTROYED_ON_LAND_TRANSFER 0.15 // during the land transfer some of the factories are destroyed

#define GAME_PERCENT_FACTORIES_DESTROYED_ON_ATTACK 0.08 // when a tile is being attacked, some of the factories also get damaged

///////////
//////////////
///////////////// input processing
//////////////
///////////

void input_enter() {
    string trash;
    getline(cin, trash);
}

// {
//     // this is the old way of getting the input
//     // this fucking sucks since it's limited to 93 characters on the X axis

//     // you can enable this mode by printing "\033[?9h", and you can disable it with "\033[?9l"

//     #define EVENT_MOUSE_CLICK {'\033', '[', 'M', ' '}
//     #define EVENT_MOUSE_CLICK_LEN 4
//     #define EVENT_MOUSE_CLICK_POS_OFFSET -33

//     std::tuple<std::string, bool, int, int> input_line() {
//         std::string line;
//         bool clicked = false;
//         int mouse_y = 0;
//         int mouse_x = 0;

//         std::getline(std::cin, line);

//         std::cout << "byte#0: " << (int)line[0] << '\n'; // 27
//         std::cout << "byte#1: " << (int)line[1] << '\n'; // 91
//         std::cout << "byte#2: " << (int)line[2] << '\n'; // 77
//         std::cout << "byte#3: " << (int)line[3] << '\n'; // 32
//         std::cout << "byte#4: " << (int)line[4] << '\n'; // 33 + pos_x
//         std::cout << "byte#5: " << (int)line[5] << '\n'; // 33 + pos_y
//         std::cout << "byte#6: " << (int)line[6] << '\n';
//         std::cout << "byte#7: " << (int)line[7] << '\n';

//         for(;;){

//             int mouse_event_idx = line.find(EVENT_MOUSE_CLICK);

//             if(mouse_event_idx < 0){
//                 break;
//             }

//             clicked = true;
//             mouse_x = line[mouse_event_idx + EVENT_MOUSE_CLICK_LEN    ] + EVENT_MOUSE_CLICK_POS_OFFSET;
//             mouse_y = line[mouse_event_idx + EVENT_MOUSE_CLICK_LEN + 1] + EVENT_MOUSE_CLICK_POS_OFFSET;

//             line.erase(mouse_event_idx, EVENT_MOUSE_CLICK_LEN + 2);

//         }

//         printf("mouse event: %d\n", clicked);

//         return std::make_tuple(line, clicked, mouse_y, mouse_x);
//     }
// }

// some info on the input modes: https://invisible-island.net/xterm/ctlseqs/ctlseqs.html#h3-Extended-coordinates

#define CSI {'\033', '['}
#define CSI_LEN 2

#define SGR_BEGIN '<'
#define SGR_SEP ';'
#define SGR_END 'M'

tuple<int, int> input_mouse_click() {

    terminal_line_buffering_disable();
    terminal_echo_disable();
    terminal_mouse_click_log_enable();

    int mouse_y = 0;
    int mouse_x = 0;

    for(;;){

        string line;
        // printf("\n");
        // cout << "byte#0: " << (int)line[0] << '\n';
        // cout << "byte#1: " << (int)line[1] << '\n';
        // cout << "byte#2: " << (int)line[2] << '\n';
        // cout << "byte#3: " << (int)line[3] << '\n';
        // cout << "byte#4: " << (int)line[4] << '\n';
        // cout << "byte#5: " << (int)line[5] << '\n';
        // cout << "byte#6: " << (int)line[6] << '\n';
        // cout << "byte#7: " << (int)line[7] << '\n';

        for(;;){
            char ch = getchar();
            
            line += ch;

            if(ch == SGR_END){

                int csi_idx = line.rfind(CSI);

                if(csi_idx < 0){
                    continue;
                }

                line.erase(0, csi_idx + CSI_LEN);

                break;
            }
        }

        assert(line[0] == SGR_BEGIN);
        line.erase(0, 1);

        // this really should not be an assert since this can be 0=click 64=mwheelup 65=mwheeldown
        assert(line[0] == '0');
        line.erase(0, 1);
        assert(line[0] == SGR_SEP);
        line.erase(0, 1);

        mouse_y = 0;
        mouse_x = 0;

        // parse mouse_x
        for(;;){
            char ch = line[0];
            line.erase(0, 1);

            if(ch == SGR_SEP){
                break;
            }

            assert(ch >= '0');
            assert(ch <= '9');

            mouse_x *= 10;
            mouse_x += ch - '0';
        }

        // parse mouse_y
        for(;;){
            char ch = line[0];
            line.erase(0, 1);

            if(ch == SGR_END){
                break;
            }

            assert(ch >= '0');
            assert(ch <= '9');

            mouse_y *= 10;
            mouse_y += ch - '0';
        }

        // ...

        assert(line.length() == 0);

        break;

    }

    terminal_mouse_click_log_disable();
    terminal_echo_enable();
    terminal_line_buffering_enable();

    // correct the positions, from 1-indexed, to 0-indexed
    mouse_y -= 1;
    mouse_x -= 1;

    return make_tuple(mouse_y, mouse_x);
}

Country* input_country(vector<vector<Tile>> *map, Country* exclude = NULL) {

    Country* selected_country;

    for(;;){
        auto [mouse_y, mouse_x] = input_mouse_click();

        if(mouse_y < 0 || mouse_x < 0 || mouse_y >= MAP_SIZE_Y || mouse_x >= MAP_SIZE_X){
            continue;
        }

        selected_country = (*map)[mouse_y][mouse_x].owner;

        if(selected_country == exclude){
            continue;
        }

        break;
    }

    return selected_country;
}

Country* input_another_country(vector<vector<Tile>> *map, Country *player) {
    return input_country(map, player);
}

///////////
//////////////
///////////////// ...
//////////////
///////////

int main() {

    // ...

    Country nobody = {
        .name = "Nobody",
        .color = COL_RESET,
        .civs_base = 0,
        .mils_base = 0,
        .civ_production = CIV_PRODUCTION_CIV,
        .equipment = -1,
        .at_war_with = {},
    };

    // load map

    vector<vector<Tile>> map;

    for(int y=0; y<MAP_SIZE_Y; ++y){
        vector<Tile> row;
        for(int x=0; x<MAP_SIZE_X; ++x){
            Tile tile = {
                .owner = &nobody,
                .borders = {},
                .civs = 0,
                .mils = 0,
            };
            row.push_back(tile);
        }
        map.push_back(row);
    }

    // conect tiles

    for(int y=0; y<MAP_SIZE_Y; ++y){
        for(int x=0; x<MAP_SIZE_X; ++x){
            
            Tile *tile = &map[y][x];

            for(auto [new_y, new_x] : {make_tuple(y-1, x), make_tuple(y, x+1), make_tuple(y+1, x), make_tuple(y, x-1)}) {

                // y

                if(new_y < 0){
                    if(MAP_LOOPS_Y){
                        new_y += MAP_SIZE_Y;
                    }else{
                        continue;
                    }
                }

                if(new_y >= MAP_SIZE_Y){
                    if(MAP_LOOPS_Y){
                        new_y -= MAP_SIZE_Y;
                    }else{
                        continue;
                    }
                }

                // x

                if(new_x < 0){
                    if(MAP_LOOPS_X){
                        new_x += MAP_SIZE_X;
                    }else{
                        continue;
                    }
                }

                if(new_x >= MAP_SIZE_X){
                    if(MAP_LOOPS_X){
                        new_x -= MAP_SIZE_X;
                    }else{
                        continue;
                    }
                }

                // connect borders

                tile->borders.push_back(&map[new_y][new_x]);

            }
        }
    }

    // load countries

    vector<Country> countries;

    countries.push_back(
        {
            .name = "Russia",
            .color = COL_INT(124),
            .civs_base = 45,
            .mils_base = 32,
            .civ_production = CIV_PRODUCTION_CIV,
            .equipment = 1'000,
            .at_war_with = {&nobody},
        }
    );

    countries.push_back(
        {
            .name = "Germany",
            .color = COL_YELLOW_DARK,
            .civs_base = 34,
            .mils_base = 28,
            .civ_production = CIV_PRODUCTION_CIV,
            .equipment = 1'000,
            .at_war_with = {&nobody},
        }
    );

    countries.push_back(
        {
            .name = "Poland",
            .color = COL_INT(13),
            .civs_base = 17,
            .mils_base = 9,
            .civ_production = CIV_PRODUCTION_CIV,
            .equipment = 1'000,
            .at_war_with = {&nobody},
        }
    );

    countries.push_back(
        {
            .name = "Turkey",
            .color = COL_INT(144),
            .civs_base = 11,
            .mils_base = 4,
            .civ_production = CIV_PRODUCTION_CIV,
            .equipment = 1'000,
            .at_war_with = {&nobody},
        }
    );

    countries.push_back(
        {
            .name = "Grece",
            .color = COL_INT(39),
            .civs_base = 7,
            .mils_base = 2,
            .civ_production = CIV_PRODUCTION_CIV,
            .equipment = 1'000,
            .at_war_with = {&nobody},
        }
    );

    countries.push_back(
        {
            .name = "Bulgaria",
            .color = COL_INT(118),
            .civs_base = 11,
            .mils_base = 3,
            .civ_production = CIV_PRODUCTION_CIV,
            .equipment = 1'000,
            .at_war_with = {&nobody},
        }
    );

    countries.push_back(
        {
            .name = "Romania",
            .color = COL_INT(184),
            .civs_base = 11,
            .mils_base = 7,
            .civ_production = CIV_PRODUCTION_CIV,
            .equipment = 1'000,
            .at_war_with = {&nobody},
        }
    );

    countries.push_back(
        {
            .name = "Yugoslavia",
            .color = COL_INT(27),
            .civs_base = 14,
            .mils_base = 3,
            .civ_production = CIV_PRODUCTION_CIV,
            .equipment = 1'000,
            .at_war_with = {&nobody},
        }
    );

    countries.push_back(
        {
            .name = "Hungary",
            .color = COL_INT(9),
            .civs_base = 10,
            .mils_base = 6,
            .civ_production = CIV_PRODUCTION_CIV,
            .equipment = 1'000,
            .at_war_with = {&nobody},
        }
    );

    countries.push_back(
        {
            .name = "Czechoslovakia",
            .color = COL_INT(38),
            .civs_base = 16,
            .mils_base = 9,
            .civ_production = CIV_PRODUCTION_CIV,
            .equipment = 1'000,
            .at_war_with = {&nobody},
        }
    );

    countries.push_back(
        {
            .name = "Austria",
            .color = COL_INT(145),
            .civs_base = 10,
            .mils_base = 3,
            .civ_production = CIV_PRODUCTION_CIV,
            .equipment = 1'000,
            .at_war_with = {&nobody},
        }
    );

    // put countries onto map

    {
        // russia
        map[MAP_SIZE_Y * 0.2][MAP_SIZE_X * 0.9].owner = &countries[0];
        map[MAP_SIZE_Y * 0.3][MAP_SIZE_X * 0.8].owner = &countries[0];
        map[MAP_SIZE_Y * 0.4][MAP_SIZE_X * 0.85].owner = &countries[0];
        map[MAP_SIZE_Y * 0.5][MAP_SIZE_X * 0.9].owner = &countries[0];

        // germany
        map[MAP_SIZE_Y * 0.3][MAP_SIZE_X * 0.55].owner = &countries[1];

        // poland
        map[MAP_SIZE_Y * 0.4][MAP_SIZE_X * 0.7].owner = &countries[2];
        map[MAP_SIZE_Y * 0.5][MAP_SIZE_X * 0.75].owner = &countries[2];

        // turkey
        map[MAP_SIZE_Y * 0.82][MAP_SIZE_X * 0.82].owner = &countries[3];
        map[MAP_SIZE_Y * 0.82][MAP_SIZE_X * 0.83].owner = &countries[3];
        map[MAP_SIZE_Y * 0.82][MAP_SIZE_X * 0.84].owner = &countries[3];

        // grece
        map[MAP_SIZE_Y * 0.85][MAP_SIZE_X * 0.7].owner = &countries[4];

        // bulgaria
        map[MAP_SIZE_Y * 0.77][MAP_SIZE_X * 0.80].owner = &countries[5];
        map[MAP_SIZE_Y * 0.77][MAP_SIZE_X * 0.81].owner = &countries[5];
        map[MAP_SIZE_Y * 0.77][MAP_SIZE_X * 0.82].owner = &countries[5];

        // romania
        map[MAP_SIZE_Y * 0.57][MAP_SIZE_X * 0.82].owner = &countries[6];

        // yugoslavia
        map[MAP_SIZE_Y * 0.7][MAP_SIZE_X * 0.65].owner = &countries[7];
        map[MAP_SIZE_Y * 0.65][MAP_SIZE_X * 0.6].owner = &countries[7];

        // hungary
        map[MAP_SIZE_Y * 0.57][MAP_SIZE_X * 0.7].owner = &countries[8];

        // cze
        map[MAP_SIZE_Y * 0.55][MAP_SIZE_X * 0.65].owner = &countries[9];

        // austria
        map[MAP_SIZE_Y * 0.5][MAP_SIZE_X * 0.55].owner = &countries[10];
    }

    // player-related

    Country* player = &countries[0];

    int gui_additional_turns_to_pass = 0;

    // main loop

    for(;;){

        // update number of factories based on land

        for(Country& country : countries){
            country.civs = country.civs_base;
            country.mils = country.mils_base;
        }

        for(int y=0; y<MAP_SIZE_Y; ++y){
            for(int x=0; x<MAP_SIZE_X; ++x){
                Tile* tile = &map[y][x];
                tile->owner->civs += tile->civs;
                tile->owner->mils += tile->mils;
            }
        }

        // update number of tiles

        for(Country& country : countries){

            country.tiles = 0;

            for(int y=0; y<MAP_SIZE_Y; ++y){
                for(int x=0; x<MAP_SIZE_X; ++x){
                    Tile *tile = &map[y][x];
                    if(tile->owner == &country){
                        country.tiles += 1;
                    }
                }
            }
        }

        // process civs

        for(Country& country : countries){

            float production = GAME_CIV_PRODUCE(country.civs);

            auto [failure, tile] = country_get_random_tile_based_on_density(&country, &map);
            if(failure){
                continue;
            }

            switch(country.civ_production){
                case CIV_PRODUCTION_CIV: {
                    tile->civs += production;
                } break;

                case CIV_PRODUCTION_MIL: {
                    tile->mils += production;
                } break;
            }
        }

        // process mils

        for(Country& country : countries){
            country.equipment += GAME_MIL_PRODUCE(country.mils);
        }

        // process wars

        for(Country& country : countries){

            if(country.equipment <= 0){
                // you can't fight if you don't have any equipment
                continue;
            }

            for(Country* country_at_war : country.at_war_with){

                vector<Tile*> tiles_to_process;

                for(int map_y=0; map_y<MAP_SIZE_Y; ++map_y){
                    for(int map_x=0; map_x<MAP_SIZE_X; ++map_x){
                        Tile *tile = &map[map_y][map_x];
                        if(tile->owner == &country){
                            tiles_to_process.push_back(tile);
                        }
                    }
                }

                for(Tile* tile : tiles_to_process){
                    for(Tile* border : tile->borders){
                        if(border->owner == country_at_war){

                            // loose equipment

                            country.equipment        -= GAME_ATK_EQUIPMENT_COST;
                            country_at_war->equipment -= GAME_DEF_EQUIPMENT_COST;

                            // damage land

                            border->civs *= 1.0 - GAME_PERCENT_FACTORIES_DESTROYED_ON_ATTACK;
                            border->mils *= 1.0 - GAME_PERCENT_FACTORIES_DESTROYED_ON_ATTACK;

                            // determine battle result

                            float deffender_multiplier = 1.0;

                            if(country_at_war->equipment <= 0){
                                deffender_multiplier *= GAME_DEF_NO_EQUIPMENT_MULTIPLIER;
                            }

                            if(random_0_to_1() * deffender_multiplier < GAME_ATK_WIN_CHANCE){
                                // battle has been won, transfer land

                                border->owner = &country;

                                border->civs *= 1.0 - GAME_PERCENT_FACTORIES_DESTROYED_ON_LAND_TRANSFER;
                                border->mils *= 1.0 - GAME_PERCENT_FACTORIES_DESTROYED_ON_LAND_TRANSFER;
                            }

                        }
                    }
                }

            }
        }

        // AI

        for(Country& country : countries){
            if(&country == player){
                continue;
            }

            // determine civ production

            // if both we're attacking a given country
            // AND they're attacking us, the numbers will double
            float total_enemy_factories = 0.0;
            float total_enemy_equipment = 0.0;
            int total_enemy_tiles = 0;

            // calc countries that we're attacking
            for(Country* country_at_war : country.at_war_with){
                total_enemy_factories += country_at_war->civs + country_at_war->mils;
                total_enemy_equipment += country_at_war->equipment;
                total_enemy_tiles += country_at_war->tiles;
            }

            // calc countries that are attacking us
            for(Country& attacker : countries){
                if(vec_contains(attacker.at_war_with, &country)){
                    total_enemy_factories += attacker.civs + attacker.mils;
                    total_enemy_equipment += attacker.equipment;
                    total_enemy_tiles += attacker.tiles;
                }
            }
            
            // TODO
            // also take bordering nations into consideration

            // decide what to build
            if(
                total_enemy_factories >= country.civs + country.mils ||
                total_enemy_equipment >= country.equipment ||
                total_enemy_tiles >= country.tiles
            ){
                country.civ_production = CIV_PRODUCTION_MIL;
            }else{
                country.civ_production = CIV_PRODUCTION_CIV;
            }

        }

        // graphics

        {
            // pass turns if told so

            if(gui_additional_turns_to_pass > 0){
                gui_additional_turns_to_pass -= 1;
                continue;
            }
        }

        for(;;){

            // clear display

            printf("%s", DISP_CLEAR);

            // draw map

            // TOD0 we could perhaps optimise the rendering by not changing the color twice for every tile
            for(auto map_row : map){
                for(auto tile : map_row){

                    float float_factories = (tile.civs + tile.mils) * MAP_TILE_VALUE_MODIFIER;

                    int int_factories = static_cast<int>( floor(float_factories) );

                    char char_factories = '?';

                    if(int_factories <= 9){
                        char_factories = '0' + int_factories;
                    }else{
                        int_factories -= 10;
                        if(int_factories <= 25){
                            char_factories = 'A' + int_factories;
                        }else{
                            char_factories = '+';
                        }
                    }

                    printf("%s%c%s", tile.owner->color, char_factories, COL_RESET);
                }
                printf("\n");
            }

            // draw "hud"

            printf("\n");

            {
                int iter = 0;

                for(Country& country : countries){

                    if(country.tiles <= 0){
                        continue;
                    }

                    if(iter > 0){
                        if(iter % 3 == 2){
                            printf("\n");
                        }else{
                            printf(" ");
                        }
                    }

                    cout << country.color << country.name << COL_RESET << "<" << "tiles~" << country.tiles << " civs:" << country.civs << " mils:" << country.mils << " equipment:" << country.equipment << ">";

                    iter += 1;
                }

            }

            printf("\n");

            // process command

            printf("\n");

            cout << "Enter command: ";

            string command;
            getline(cin, command);

            vector<string> cmds_pass = {"", "pass", "next-turn"};
            vector<string> cmds_pass_10 = {"pass10", "pass-10-turns"};
            vector<string> cmds_pass_50 = {"pass50", "pass-50-turns"};
            vector<string> cmds_quit = {"q", "quit", "quit-game"};
            vector<string> cmds_attack = {"a", "attack", "attack-country"};
            vector<string> cmds_stop_attacking = {"s", "stop-attack", "stop-attacking-country"};
            vector<vector<string>> cmds_ALL = {cmds_pass, cmds_quit, cmds_attack};

            if(vec_contains(cmds_pass, command)){
                goto break_loop_command;
            
            }else if(vec_contains(cmds_pass_10, command)){
                gui_additional_turns_to_pass = 10 - 1;
                goto break_loop_command;

            }else if(vec_contains(cmds_pass_50, command)){
                gui_additional_turns_to_pass = 50 - 1;
                goto break_loop_command;

            }else if(vec_contains(cmds_quit, command)){
                goto break_loop_game;

            }else if(vec_contains(cmds_attack, command)){

                printf("Click on the country that you want to attack\n");
                Country *target = input_another_country(&map, player);

                vec_push_back_nodup(player->at_war_with, target);

            }else if(vec_contains(cmds_stop_attacking, command)){

                printf("Click on the country that you want to stop attacking\n");
                Country *piece_target = input_country(&map);

                vec_remove_if_exist(player->at_war_with, piece_target);

            }else if("test" == command){

                for(;;){
                    // auto [mouse_y, mouse_x] = input_mouse_click();
                    // printf("y:%d x:%d\n", mouse_y, mouse_x);

                    int num = random_int(0, 10);
                    printf("num:%d\n", num);
                }

            }else{

                cout << '\n';

                cout << "Unknown command `" << command << "`\n";

                cout << "List of commands:" << '\n';

                for(auto cmds : cmds_ALL){
                    for(auto cmd : cmds){
                        cout << "<" << cmd << "> ";
                    }
                    cout << '\n';
                }

                cout << "\nPRESS ENTER\n";
                input_enter();
            }

        }
        break_loop_command:

    }
    break_loop_game:

    // ...

    return 0;
}
