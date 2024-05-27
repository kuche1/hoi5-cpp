
#include <stdint.h>
#include <stdio.h>

///////////
//////////////
///////////////// generic defines
//////////////
///////////

#define LENOF(comptime_arr) \
    (sizeof(comptime_arr) / sizeof(*comptime_arr))

#define FOREACH(item_var, comptime_arr, code) \
    for(size_t item_var ## _idx = 0; item_var ## _idx < LENOF(comptime_arr); ++ item_var ## _idx) {\
        __typeof__(*comptime_arr) item_var = comptime_arr[item_var ## _idx]; \
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
    int32_t factories_civ;
    int32_t factories_mil;
} Country;

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
    };

    FOREACH(country, countries, {
        printf("country name: %s%s%s\n", country.color, country.name, COL_RESET);
    })

    // load map (TODO from file; see comment on loading countries for more info on how we're going to
    // do that easily)

    // TODO

    return 0;
}
