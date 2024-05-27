
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
///////////////// generic defines
//////////////
///////////

#define LENOF(comptime_arr) \
    (sizeof(comptime_arr) / sizeof(*comptime_arr))

#define FOREACH(item_var, comptime_arr, code) \
    for(size_t item_var ## _idx = 0; item_var ## _idx < LENOF(comptime_arr); ++ item_var ## _idx) {\
        __typeof__(*comptime_arr) *item_var = &comptime_arr[item_var ## _idx]; \
        code; \
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

// this struct must not use any wishy washy data types (like pointers)
// since it's going to be loaded/saved to disk
typedef struct {
    // misc
    CountryName name;
    Color color;
    // factories
    float factories_civ;
    float factories_mil;
    CivProduction civ_production; // what are the civs producing
    // equipment
    float equipment; // I plan on allowing for a defficit
    
} Country;

///////////
//////////////
///////////////// map
//////////////
///////////

// typedef enum {
//     TILE_TYPE_PLAIN,
//     TILE_TYPE_URBAN,
//     TILE_TYPE_FOREST,
// } TileType;

typedef struct {
    Country* owner;
    // TileType type;
    // uint32_t troops;
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

#define CMD_PASS ""
#define CMD_QUIT "quit"

#define CMDS_ALL {CMD_PASS, CMD_QUIT}

std::tuple<std::string, bool, int, int> read_line() {
    std::string line;
    bool clicked = false;
    int mouse_y = 0;
    int mouse_x = 0;

    std::getline(std::cin, line);

    int mouse_event_idx = line.find(EVENT_MOUSE_CLICK);

    // std::cout << "found: " << mouse_event_idx << '\n';

    if(mouse_event_idx >= 0){
        clicked = true;
        mouse_x = line[mouse_event_idx + EVENT_MOUSE_CLICK_LEN    ] + EVENT_MOUSE_CLICK_POS_OFFSET;
        mouse_y = line[mouse_event_idx + EVENT_MOUSE_CLICK_LEN + 1] + EVENT_MOUSE_CLICK_POS_OFFSET;

        line.erase(mouse_event_idx, EVENT_MOUSE_CLICK_LEN + 2);
    }

    return std::make_tuple(line, clicked, mouse_y, mouse_x);
}

///////////
//////////////
///////////////// ...
//////////////
///////////

int main() {

    // load countries (TODO from file; we would read the file straingt into the array;
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
        },
        {
            .name = "Germany",
            .color = COL_YELLOW_DARK,
            .factories_civ = 40,
            .factories_mil = 12,
            .civ_production = CIV_PRODUCTION_CIV,
            .equipment = 5'000,
        },
        {
            .name = "Poland",
            .color = COL_MAGENTA_DARK,
            .factories_civ = 20,
            .factories_mil = 8,
            .civ_production = CIV_PRODUCTION_CIV,
            .equipment = 4'000,
        },
    };

    // FOREACH(country, countries, {
    //     printf("country name: %s%s%s\n", country.color, country.name, COL_RESET);
    // })

    // load map (TODO from file; see comment on loading countries for more info on how we're going to
    // do that easily)

    Tile map[] = {
        {.owner = &countries[1],},
        {.owner = &countries[1],},
        {.owner = &countries[1],},
        {.owner = &countries[2],},
        {.owner = &countries[2],},
        {.owner = &countries[0],},
        {.owner = &countries[0],}, //
        {.owner = &countries[1],},
        {.owner = &countries[1],},
        {.owner = &countries[2],},
        {.owner = &countries[2],},
        {.owner = &countries[2],},
        {.owner = &countries[0],},
        {.owner = &countries[0],}, //
        {.owner = &countries[1],},
        {.owner = &countries[1],},
        {.owner = &countries[1],},
        {.owner = &countries[2],},
        {.owner = &countries[2],},
        {.owner = &countries[0],},
        {.owner = &countries[0],}, //
    };

    // ...

    printf(MOUSE_CLICK_LOG_ON);

    // ...

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

        // FOREACH(country, countries, {
        //     FOREACH(tile, map, {
        //         if(tile->owner == country){
        //             printf("yee\n");
        //         }
        //     })
        // })

        FOREACH(country, countries, {
            country->equipment += GAME_MIL_PRODUCE(country->factories_mil);
        })

        for(;;){

            // clear display

            printf("%s", DISP_CLEAR);

            // draw map

            FOREACH(tile, map, {
                printf("%s%d%s", tile->owner->color, 0, COL_RESET);

                if(tile_idx % 7 == 6){
                    printf("\n");
                }
            })

            // draw "hud"

            printf("\n");

            FOREACH(country, countries, {
                printf("idx:%lu country:%s%s%s civs:%f mils:%f equipment:%f\n", country_idx, country->color, country->name, COL_RESET, country->factories_civ, country->factories_mil, country->equipment);
            })

            // process command

            printf("\n");

            std::cout << "Enter command: ";

            auto [command, mouse_click, mouse_y, mouse_x] = read_line();

            if(mouse_click){
                std::cout << "y:" << mouse_y << " x:" << mouse_x << '\n';
            }

            if(command == CMD_PASS){
                goto break_loop_command;
            
            }else if(command == CMD_QUIT){
                goto break_loop_game;

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

                auto cmds = CMDS_ALL;

                for(auto cmd : cmds){
                    std::cout << "`" << cmd << "` - TODO add description\n";
                }

                std::cout << "\nPRESS ENTER\n";
                read_line();
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
