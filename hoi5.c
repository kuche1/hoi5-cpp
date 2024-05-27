
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

///////////
//////////////
///////////////// country
//////////////
///////////

typedef char CountryName[60];

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

    // load countries (TODO from file; we dould read the file straingt into the array; we can infer the
    // size of the array given the size of the file;
    // if % is not 0, the binary representation must have changed)

    Country countries[1] = {
        {
            .name = "Soviet Union",
        }
    };

    // for(int32_t country_idx=0; country_idx<LENOF(countries); ++country_idx){

    // }

    FOREACH(country, countries, {
        printf("country name: %s\n", country.name);
    })

    return 0;
}
