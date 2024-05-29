
// TODO
//
// the fucking vectors can realloc, so I need to check what happens if I use push_back in a function that got passed a pointer/reference to the vector (nothing bad should happen, but I need to make sure)

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

#define EFFECT_INVERSE_ON  "\033[7m"
#define EFFECT_INVERSE_OFF "\033[27m" // untested

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

#define MAP_SIZE_Y 35
#define MAP_SIZE_X 205

#define MAP_LOOPS_Y false
#define MAP_LOOPS_X false

#define MAP_TILE_BORDERS_MAX_LEN 4 // you can have 4 borders max (an assertion)

#define MAP_TILE_VALUE_MODIFIER 250.0 // increases visibility for humans

// insentivise players to attack `nobody` by providing some resources for all tiles
#define MAP_TILE_INITIAL_CIVS 0.003
// we could do the same for the mils, the problem is - this would increase `nobody`'s equipment, so the AI
// would be discouraged from attacking

typedef struct _Tile {
    Country* owner;
    vector<struct _Tile*> borders;
    // number of factories
    float civs = MAP_TILE_INITIAL_CIVS;
    float mils = 0;
    // to be updated in the game loop
    bool is_war_border = false;
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
    vector<struct _Country *> at_war_with = {};
    // map-related
    int tiles = 0; // needs to be updated in the game loop
    vector<Country*> bordering_countries = {}; // needs to be updated in the game loop
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
///////////////// AI behaviour
//////////////
///////////

#define AI_START_WAR_IF_NEIGHBOUR_HAS_LESS_EQUIPMENT_THRESHOLD 0.2

#define AI_STOP_WAR_IF_WE_HAVE_LESS_EQUIPMENT_THRESHOLD 0.2

#define AI_STOP_WAR_IF_PRODUCTION_FOR_A_COUPLE_OF_DAYS_EXCEEDS_EQUIPMENT 10 // if the current amount of equipment that we have could be produced in less than current amount of time stop fighting

///////////
//////////////
///////////////// ...
//////////////
///////////

int main() {

    #include "1_init.cpp"

    #include "2_0_main_loop.cpp"

    return 0;
}
