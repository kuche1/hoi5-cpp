
///////////
//////////////
///////////////// input processing
//////////////
///////////

void input_enter() {
    cout << "PRESS ENTER\n";
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
