
#include <stdint.h>
#include <stdio.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <cmath>

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
///////////////// generic visual effects
//////////////
///////////

typedef char* Color;

#define COL_RESET ((Color)"\033[0m")

#define COL_RED_DARK ((Color)"\033[31m")
#define COL_YELLOW_DARK ((Color)"\033[33m")
#define COL_MAGENTA_DARK ((Color)"\033[35m")

#define DISP_CLEAR "\033[H\033[J"

///////////
//////////////
///////////////// country
//////////////
///////////

typedef char CountryName[60];

// this struct must not use any wishy washy data types (like pointers)
// since it's going to be loaded/saved to disk
typedef struct {
    // misc
    CountryName name;
    Color color;
    // ...
    float factories_civ;
    float factories_mil;
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
} Tile;

///////////
//////////////
///////////////// gameplay
//////////////
///////////

#define GAME_CIV_PRODUCE(number_of_civs) (std::floor(number_of_civs) / 10)

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
            .name = "Soviet Union",
            .color = COL_RED_DARK,
            .factories_civ = 30,
            .factories_mil = 20,
        },
        {
            .name = "Germany",
            .color = COL_YELLOW_DARK,
            .factories_civ = 40,
            .factories_mil = 12,
        },
        {
            .name = "Poland",
            .color = COL_MAGENTA_DARK,
            .factories_civ = 20,
            .factories_mil = 8,
        },
    };

    // FOREACH(country, countries, {
    //     printf("country name: %s%s%s\n", country.color, country.name, COL_RESET);
    // })

    // load map (TODO from file; see comment on loading countries for more info on how we're going to
    // do that easily)

    Tile map[] = {
        {.owner = &countries[0]},
        {.owner = &countries[0]},
        {.owner = &countries[0]},
        {.owner = &countries[0]},
        {.owner = &countries[0]},
        {.owner = &countries[0]},

        {.owner = &countries[1]},
        {.owner = &countries[1]},
        {.owner = &countries[1]},
        {.owner = &countries[1]},
        {.owner = &countries[1]},
        {.owner = &countries[1]},

        {.owner = &countries[2]},
        {.owner = &countries[2]},
        {.owner = &countries[2]},
        {.owner = &countries[2]},
        {.owner = &countries[2]},
        {.owner = &countries[2]},
    };

    // ...

    for(;;){

        printf("%s", DISP_CLEAR);

        FOREACH(tile, map, {
            printf("%s0%s", tile->owner->color, COL_RESET);

            if(tile_idx % 6 == 5){
                printf("\n");
            }
        })

        printf("\n");

        FOREACH(country, countries, {
            country->factories_civ += GAME_CIV_PRODUCE(country->factories_civ);
        })

        FOREACH(country, countries, {
            printf("country: %s%s%s civs:%f mils:%f\n", country->color, country->name, COL_RESET, country->factories_civ, country->factories_mil);
        })

        std::this_thread::sleep_for(std::chrono::seconds(3));

    }

    return 0;
}
