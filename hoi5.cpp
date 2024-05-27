
// mouse event tracking: https://invisible-island.net/xterm/ctlseqs/ctlseqs.html#h3-Any-event-tracking
// https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797?permalink_comment_id=3878578
// only get mouse clicks with: code 9 (`h` to turn on; `l` to turn off) (printf "\e[?9h"); I notices that this works in bash and not in fish

#include <stdint.h>
#include <stdio.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <cmath>
// #include <string>

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
bool vec_contains(const std::vector<T>& vec, const T& element) {
    return std::find(vec.begin(), vec.end(), element) != vec.end();
}

template<typename T>
void vec_push_back_nodup(std::vector<T>& vec, const T& element) {
    if(!vec_contains(vec, element)){
        vec.push_back(element);
    }
}

void term(char *command) {
    int ret_code = system(command);
    if(ret_code != 0){
        std::cerr << "ERROR: command failed: " << command << '\n';
        exit(1);
    }
}

///////////
//////////////
///////////////// ansi escape codes
//////////////
///////////

typedef char* Color;

#define COL_RESET ((Color)"\033[0m")

#define COL_RED_DARK ((Color)"\033[31m")
#define COL_YELLOW_DARK ((Color)"\033[33m")
#define COL_MAGENTA_DARK ((Color)"\033[35m")

#define DISP_CLEAR "\033[H\033[J"

#define MOUSE_CLICK_LOG_ON  "\033[?9h"
#define MOUSE_CLICK_LOG_OFF "\033[?9l"

// echo

void terminal_echo_enable() {
    term((char*)"stty echo");
}

void terminal_echo_disable() {
    term((char*)"stty -echo");
}

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

typedef struct _Country {
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
    std::vector<struct _Country*> at_war_with;
} Country;

///////////
//////////////
///////////////// map
//////////////
///////////

#define MAP_SIZE_Y 10
#define MAP_SIZE_X 70

// typedef enum {
//     TILE_TYPE_PLAIN,
//     TILE_TYPE_URBAN,
//     TILE_TYPE_FOREST,
// } TileType;

typedef struct _Tile {
    Country* owner;
    // TileType type;
    // uint32_t troops;
    std::vector<struct _Tile*> borders;
} Tile;

///////////
//////////////
///////////////// gameplay
//////////////
///////////

#define GAME_CIV_PRODUCE(number_of_civs) (std::floor(number_of_civs) * 0.1)

#define GAME_MIL_PRODUCE(number_of_mils) (std::floor(number_of_mils) * 3.3)

///////////
//////////////
///////////////// input processing
//////////////
///////////

#define EVENT_MOUSE_CLICK {27, 91, 77, 32}
#define EVENT_MOUSE_CLICK_LEN 4
#define EVENT_MOUSE_CLICK_POS_OFFSET -33

std::tuple<std::string, bool, int, int> input_line() {
    std::string line;
    bool clicked = false;
    int mouse_y = 0;
    int mouse_x = 0;

    std::getline(std::cin, line);

    for(;;){

        int mouse_event_idx = line.find(EVENT_MOUSE_CLICK);

        if(mouse_event_idx < 0){
            break;
        }

        clicked = true;
        mouse_x = line[mouse_event_idx + EVENT_MOUSE_CLICK_LEN    ] + EVENT_MOUSE_CLICK_POS_OFFSET;
        mouse_y = line[mouse_event_idx + EVENT_MOUSE_CLICK_LEN + 1] + EVENT_MOUSE_CLICK_POS_OFFSET;

        line.erase(mouse_event_idx, EVENT_MOUSE_CLICK_LEN + 2);

    }

    return std::make_tuple(line, clicked, mouse_y, mouse_x);
}

Country* input_country(std::vector<std::vector<Tile>> *map) {

    terminal_echo_disable();

    Country* ret;

    for(;;){
        auto [command, mouse_click, mouse_y, mouse_x] = input_line();

        if(!mouse_click){
            continue;
        }

        if(mouse_y < 0 || mouse_x < 0 || mouse_y >= MAP_SIZE_Y || mouse_x >= MAP_SIZE_X){
            continue;
        }

        ret = (*map)[mouse_y][mouse_x].owner;
        break;
    }

    terminal_echo_enable();

    return ret;
}

Country* input_another_country(Country *player, std::vector<std::vector<Tile>> *map) {
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
        .equipment = 0,
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
            .factories_civ = 30,
            .factories_mil = 20,
            .civ_production = CIV_PRODUCTION_CIV,
            .equipment = 10'000,
            .at_war_with = {&nobody},
        },
        {
            .name = "Germany",
            .color = COL_YELLOW_DARK,
            .factories_civ = 40,
            .factories_mil = 12,
            .civ_production = CIV_PRODUCTION_CIV,
            .equipment = 5'000,
            .at_war_with = {&nobody},
        },
        {
            .name = "Poland",
            .color = COL_MAGENTA_DARK,
            .factories_civ = 20,
            .factories_mil = 8,
            .civ_production = CIV_PRODUCTION_CIV,
            .equipment = 4'000,
            .at_war_with = {&nobody},
        },
    };

    // load map (TOD0 from file; see comment on loading countries for more info on how we're going to
    // do that easily)

    std::vector<std::vector<Tile>> map;

    for(int y=0; y<MAP_SIZE_Y; ++y){
        std::vector<Tile> row;
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

            {
                int new_y = y - 1;
                int new_x = x;
                if(new_y >= 0 && new_y < MAP_SIZE_Y){
                    if(new_x < 0){
                        new_x += MAP_SIZE_X;
                    }else if(new_x >= MAP_SIZE_X){
                        new_x -= MAP_SIZE_X;
                    }
                    tile->borders.push_back(&map[new_y][new_x]);
                }
            }

            {
                int new_y = y;
                int new_x = x + 1;
                if(new_y >= 0 && new_y < MAP_SIZE_Y){
                    if(new_x < 0){
                        new_x += MAP_SIZE_X;
                    }else if(new_x >= MAP_SIZE_X){
                        new_x -= MAP_SIZE_X;
                    }
                    tile->borders.push_back(&map[new_y][new_x]);
                }
            }

            {
                int new_y = y + 1;
                int new_x = x;
                if(new_y >= 0 && new_y < MAP_SIZE_Y){
                    if(new_x < 0){
                        new_x += MAP_SIZE_X;
                    }else if(new_x >= MAP_SIZE_X){
                        new_x -= MAP_SIZE_X;
                    }
                    tile->borders.push_back(&map[new_y][new_x]);
                }
            }

            {
                int new_y = y;
                int new_x = x - 1;
                if(new_y >= 0 && new_y < MAP_SIZE_Y){
                    if(new_x < 0){
                        new_x += MAP_SIZE_X;
                    }else if(new_x >= MAP_SIZE_X){
                        new_x -= MAP_SIZE_X;
                    }
                    tile->borders.push_back(&map[new_y][new_x]);
                }
            }

        }
    }

    // put countries onto map (TOD0 from file)

    map[MAP_SIZE_Y * 0.5][MAP_SIZE_X * 0.8].owner = &countries[0];

    map[MAP_SIZE_Y * 0.5][MAP_SIZE_X * 0.2].owner = &countries[1];

    map[MAP_SIZE_Y * 0.5][MAP_SIZE_X * 0.5].owner = &countries[2];

    // ...

    Country* player = &countries[0];

    // ...

    printf(MOUSE_CLICK_LOG_ON);

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

                std::vector<Tile*> tiles_to_process;

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
                            border->owner = country;
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

            std::cout << "Enter command: ";

            auto [command, mouse_click, mouse_y, mouse_x] = input_line();

            if(mouse_click){
                std::cout << "y:" << mouse_y << " x:" << mouse_x << '\n';
            }

            std::vector<std::string> cmds_pass = {"", "pass", "next-turn"};
            std::vector<std::string> cmds_quit = {"q", "quit", "quit-game"};
            std::vector<std::string> cmds_attack = {"a", "attack", "attack-country"};
            std::vector<std::vector<std::string>> cmds_ALL = {cmds_pass, cmds_quit, cmds_attack};

            if(vec_contains(cmds_pass, command)){
                goto break_loop_command;

            }else if(vec_contains(cmds_quit, command)){
                goto break_loop_game;

            }else if(vec_contains(cmds_attack, command)){

                printf("Click on target and press enter\n");
                Country *target = input_another_country(player, &map);

                vec_push_back_nodup(player->at_war_with, target);

            }else{
                // std::cout << "byte#0: " << (int)command[0] << '\n'; // 27
                // std::cout << "byte#1: " << (int)command[1] << '\n'; // 91
                // std::cout << "byte#2: " << (int)command[2] << '\n'; // 77
                // std::cout << "byte#3: " << (int)command[3] << '\n'; // 32
                // std::cout << "byte#4: " << (int)command[4] << '\n'; // 33 + pos_x
                // std::cout << "byte#5: " << (int)command[5] << '\n'; // 33 + pos_y

                std::cout << '\n';

                std::cout << "Unknown command `" << command << "`\n";

                std::cout << "List of commands:" << '\n';

                for(auto cmds : cmds_ALL){
                    for(auto cmd : cmds){
                        std::cout << "<" << cmd << "> ";
                    }
                    std::cout << '\n';
                }

                std::cout << "\nPRESS ENTER\n";
                input_line();
            }

        }
        break_loop_command:

        // sleep (only make sense if we're using a GUI)

        // std::this_thread::sleep_for(std::chrono::seconds(2));

    }
    break_loop_game:

    // ...

    printf(MOUSE_CLICK_LOG_OFF);

    // ...

    return 0;
}
