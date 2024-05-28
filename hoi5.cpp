
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

#define FOREACH(item_var, comptime_arr, code) \
    for(size_t item_var ## _idx = 0; item_var ## _idx < LENOF(comptime_arr); ++ item_var ## _idx) {\
        __typeof__(*comptime_arr) *item_var = &comptime_arr[item_var ## _idx]; \
        code; \
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

void term(char *command) {
    int ret_code = system(command);
    if(ret_code != 0){
        cerr << "ERROR: command failed: " << command << '\n';
        exit(1);
    }
}

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

///////////
//////////////
///////////////// ansi escape codes
//////////////
///////////

// https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797

typedef char* Color;

#define DISP_CLEAR "\033[H\033[J"

#define COL_RESET ((Color)"\033[0m")

#define COL_RED_DARK ((Color)"\033[31m")
#define COL_YELLOW_DARK ((Color)"\033[33m")
#define COL_MAGENTA_DARK ((Color)"\033[35m")

#define COL_39  ((Color)"\033[38;5;39m")
#define COL_118 ((Color)"\033[38;5;118m")
#define COL_144 ((Color)"\033[38;5;144m")

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

///////////
//////////////
///////////////// map
//////////////
///////////

#define MAP_SIZE_Y 35
#define MAP_SIZE_X 205

#define MAP_LOOPS_Y false
#define MAP_LOOPS_X false

// typedef enum {
//     TILE_TYPE_PLAIN,
//     TILE_TYPE_URBAN,
//     TILE_TYPE_FOREST,
// } TileType;

typedef struct _Tile {
    Country* owner;
    // TileType type;
    // uint32_t troops;
    vector<struct _Tile*> borders;
} Tile;

///////////
//////////////
///////////////// country
//////////////
///////////

typedef char CountryName[60];

typedef enum {
    CIV_PRODUCTION_CIV,
    CIV_PRODUCTION_MIL,
} CivProduction;

struct _Country {
    // misc
    CountryName name;
    Color color;
    // factories
    float factories_civ;
    float factories_mil;
    CivProduction civ_production; // what are the civs producing
    // equipment
    float equipment; // I plan on allowing for a defficit
    // war
    vector<struct _Country*> at_war_with;
};

int country_count_tiles(Country *country, vector<vector<Tile>> *map) {
    int count = 0;

    for(int y=0; y<MAP_SIZE_Y; ++y){
        for(int x=0; x<MAP_SIZE_X; ++x){
            Tile *tile = &((*map)[y][x]);
            if(tile->owner == country){
                count += 1;
            }
        }
    }

    return count;
}

///////////
//////////////
///////////////// gameplay
//////////////
///////////

#define GAME_CIV_PRODUCE(number_of_civs) (floor(number_of_civs) * 0.1)

#define GAME_MIL_PRODUCE(number_of_mils) (floor(number_of_mils) * 20.0)

#define GAME_ATK_WIN_CHANCE 0.2 // what is the change (0 to 1) that a terriroty would be gained upon attack

#define GAME_ATK_EQUIPMENT_COST 18.0 // how much equipment a single attack costs

#define GAME_DEF_EQUIPMENT_COST (GAME_ATK_EQUIPMENT_COST * 0.2) // how much equipment does it cost to deffend an attack

#define GAME_ATK_NO_EQUIPMENT_PENALTY 0.4

#define GAME_DEF_NO_EQUIPMENT_PENALTY 0.7

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

#define CSI_BEGIN {'\033', '['}
#define CSI_BEGIN_LEN 2

// mode info on the input modes: https://invisible-island.net/xterm/ctlseqs/ctlseqs.html#h3-Extended-coordinates
tuple<int, int> input_mouse_click() {

    terminal_echo_disable();
    terminal_mouse_click_log_enable();

    int mouse_y = 0;
    int mouse_x = 0;

    for(;;){

        string line;
        getline(cin, line);

        // printf("\n");
        // cout << "byte#0: " << (int)line[0] << '\n';
        // cout << "byte#1: " << (int)line[1] << '\n';
        // cout << "byte#2: " << (int)line[2] << '\n';
        // cout << "byte#3: " << (int)line[3] << '\n';
        // cout << "byte#4: " << (int)line[4] << '\n';
        // cout << "byte#5: " << (int)line[5] << '\n';
        // cout << "byte#6: " << (int)line[6] << '\n';
        // cout << "byte#7: " << (int)line[7] << '\n';

        int csi_idx = line.rfind(CSI_BEGIN);

        if(csi_idx < 0){
            continue;
        }

        line.erase(csi_idx, CSI_BEGIN_LEN);

        assert(line[0] == '<');
        line.erase(0, 1);

        // this really should not be an assert since this can be 0=click 64=mwheelup 65=mwheeldown
        assert(line[0] == '0');
        line.erase(0, 1);
        assert(line[0] == ';');
        line.erase(0, 1);

        mouse_y = 0;
        mouse_x = 0;

        // parse mouse_x
        for(;;){
            char ch = line[0];
            line.erase(0, 1);

            if(ch == ';'){
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

            if(ch == 'M'){
                break;
            }

            assert(ch >= '0');
            assert(ch <= '9');

            mouse_y *= 10;
            mouse_y += ch - '0';
        }

        // fuck anything left in the string

        // cout << "left over: " << line << '\n';

        // cout << "byte#0: " << (int)line[0] << '\n';
        // cout << "byte#1: " << (int)line[1] << '\n';
        // cout << "byte#2: " << (int)line[2] << '\n';
        // cout << "byte#3: " << (int)line[3] << '\n';
        // cout << "byte#4: " << (int)line[4] << '\n';
        // cout << "byte#5: " << (int)line[5] << '\n';
        // cout << "byte#6: " << (int)line[6] << '\n';
        // cout << "byte#7: " << (int)line[7] << '\n';

        // printf("\n");

        break;

    }

    terminal_mouse_click_log_disable();
    terminal_echo_enable();

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
    for(;;){
        Country *country = input_country(map);

        if(country == player){
            continue;
        }

        return country;
    }
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
        .factories_civ = 0,
        .factories_mil = 0,
        .civ_production = CIV_PRODUCTION_CIV,
        .equipment = -1,
        .at_war_with = {},
    };

    // load countries (TOD0 from file; we would read the file straingt into the array;
    // we can infer the size of the array given the size of the file;
    // if % is not 0, the binary representation must have changed;
    // would probably be a good idea to put both the countries and the map structures into
    // another not-padded structure, then add a field for the game version (removing padding might
    // not be necessary if we include a game version, alto it would be more correct))

    Country countries[] = {
        {
            .name = "Russia",
            .color = COL_RED_DARK,
            .factories_civ = 45,
            .factories_mil = 32,
            .civ_production = CIV_PRODUCTION_CIV,
            .equipment = 1'000,
            .at_war_with = {&nobody},
        },
        {
            .name = "Germany",
            .color = COL_YELLOW_DARK,
            .factories_civ = 34,
            .factories_mil = 28,
            .civ_production = CIV_PRODUCTION_CIV,
            .equipment = 1'000,
            .at_war_with = {&nobody},
        },
        {
            .name = "Poland",
            .color = COL_MAGENTA_DARK,
            .factories_civ = 17,
            .factories_mil = 9,
            .civ_production = CIV_PRODUCTION_CIV,
            .equipment = 1'000,
            .at_war_with = {&nobody},
        },
        {
            .name = "Turkey",
            .color = COL_144,
            .factories_civ = 11,
            .factories_mil = 4,
            .civ_production = CIV_PRODUCTION_CIV,
            .equipment = 1'000,
            .at_war_with = {&nobody},
        },
        {
            .name = "Grece",
            .color = COL_39,
            .factories_civ = 7,
            .factories_mil = 2,
            .civ_production = CIV_PRODUCTION_CIV,
            .equipment = 1'000,
            .at_war_with = {&nobody},
        },
        {
            .name = "Bulgaria",
            .color = COL_118,
            .factories_civ = 11,
            .factories_mil = 3,
            .civ_production = CIV_PRODUCTION_CIV,
            .equipment = 1'000,
            .at_war_with = {&nobody},
        },
    };

    // load map (TOD0 from file; see comment on loading countries for more info on how we're going to
    // do that easily)

    vector<vector<Tile>> map;

    for(int y=0; y<MAP_SIZE_Y; ++y){
        vector<Tile> row;
        for(int x=0; x<MAP_SIZE_X; ++x){
            Tile tile = {
                .owner = &nobody,
                .borders = {},
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

    // put countries onto map (TOD0 from file)

    // russia
    map[MAP_SIZE_Y * 0.2][MAP_SIZE_X * 0.9].owner = &countries[0];
    map[MAP_SIZE_Y * 0.3][MAP_SIZE_X * 0.8].owner = &countries[0];
    map[MAP_SIZE_Y * 0.4][MAP_SIZE_X * 0.85].owner = &countries[0];

    // germany
    map[MAP_SIZE_Y * 0.5][MAP_SIZE_X * 0.2].owner = &countries[1];

    // poland
    map[MAP_SIZE_Y * 0.5][MAP_SIZE_X * 0.5].owner = &countries[2];

    // turkey
    map[MAP_SIZE_Y * 0.8][MAP_SIZE_X * 0.8].owner = &countries[3];
    map[MAP_SIZE_Y * 0.8][MAP_SIZE_X * 0.9].owner = &countries[3];

    // grece
    map[MAP_SIZE_Y * 0.8][MAP_SIZE_X * 0.7].owner = &countries[4];

    // bulgaria
    map[MAP_SIZE_Y * 0.7][MAP_SIZE_X * 0.75].owner = &countries[5];

    // create player

    Country* player = &countries[0];

    // main loop

    for(;;){

        // process civs

        FOREACH(country, countries, {

            float production = GAME_CIV_PRODUCE(country->factories_civ);

            switch(country->civ_production){
                case CIV_PRODUCTION_CIV: {
                    country->factories_civ += production;
                } break;

                case CIV_PRODUCTION_MIL: {
                    country->factories_mil += production;
                } break;
            }
        })

        // process mils

        FOREACH(country, countries, {
            country->equipment += GAME_MIL_PRODUCE(country->factories_mil);
        })

        // process wars

        FOREACH(country, countries, {

            for(Country* country_at_war : country->at_war_with){

                vector<Tile*> tiles_to_process;

                for(int map_y=0; map_y<MAP_SIZE_Y; ++map_y){
                    for(int map_x=0; map_x<MAP_SIZE_X; ++map_x){
                        Tile *tile = &map[map_y][map_x];
                        if(tile->owner == country){
                            tiles_to_process.push_back(tile);
                        }
                    }
                }

                for(Tile* tile : tiles_to_process){
                    for(Tile* border : tile->borders){
                        if(border->owner == country_at_war){

                            // loose equipment
                            country->equipment        -= GAME_ATK_EQUIPMENT_COST;
                            country_at_war->equipment -= GAME_DEF_EQUIPMENT_COST;

                            // determine battle result

                            float attacker_mult = 1;
                            if(country->equipment < 0){
                                attacker_mult *= GAME_ATK_NO_EQUIPMENT_PENALTY;
                            }

                            float deffender_mult = 1;
                            if(country_at_war->equipment < 0){
                                deffender_mult *= GAME_DEF_NO_EQUIPMENT_PENALTY;
                            }

                            if(random_0_to_1() * deffender_mult < GAME_ATK_WIN_CHANCE * attacker_mult){
                                // battle has been won, transfer land

                                int looser_tiles = country_count_tiles(country_at_war, &map);

                                border->owner = country;

                                float percent_of_land_lost = 1 / looser_tiles;

                                country_at_war->factories_civ *= (1.0 - percent_of_land_lost);

                                country_at_war->factories_mil *= (1.0 - percent_of_land_lost);

                                // TODO untested

                            }

                        }
                    }
                }

            }
        })

        // graphics

        for(;;){

            // clear display

            printf("%s", DISP_CLEAR);

            // draw map

            for(auto map_row : map){
                for(auto tile : map_row){
                    printf("%s%d%s", tile.owner->color, 0, COL_RESET);
                }
                printf("\n");
            }

            // draw "hud"

            printf("\n");

            FOREACH(country, countries, {
                printf("idx:%lu country:%s%s%s civs:%f mils:%f equipment:%f\n", country_idx, country->color, country->name, COL_RESET, country->factories_civ, country->factories_mil, country->equipment);
            })

            // process command

            printf("\n");

            cout << "Enter command: ";

            string command;
            getline(cin, command);

            vector<string> cmds_pass = {"", "pass", "next-turn"};
            vector<string> cmds_quit = {"q", "quit", "quit-game"};
            vector<string> cmds_attack = {"a", "attack", "attack-country"};
            vector<string> cmds_stop_attacking = {"s", "stop-attack", "stop-attacking-country"};
            vector<vector<string>> cmds_ALL = {cmds_pass, cmds_quit, cmds_attack};

            if(vec_contains(cmds_pass, command)){
                goto break_loop_command;

            }else if(vec_contains(cmds_quit, command)){
                goto break_loop_game;

            }else if(vec_contains(cmds_attack, command)){

                printf("Click on the country that you want to attack and press enter\n");
                Country *target = input_another_country(&map, player);

                vec_push_back_nodup(player->at_war_with, target);
            
            }else if(vec_contains(cmds_stop_attacking, command)){

                printf("Click on the country that you want to stop attacking and press enter\n");
                Country *piece_target = input_country(&map);

                vec_remove_if_exist(player->at_war_with, piece_target);

            }else if("test" == command){

                for(;;){
                    auto [mouse_y, mouse_x] = input_mouse_click();
                    printf("y:%d x:%d\n", mouse_y, mouse_x);
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

        // sleep (only make sense if we're using a GUI)

        // this_thread::sleep_for(chrono::seconds(2));

    }
    break_loop_game:

    // ...

    return 0;
}
