
// TODO
//
// use a buffer of some sort and flush it only when the full map has been drawn (perhaps disabling flush on \n would be sufficient)
// https://www.quora.com/When-programming-in-C-is-it-more-efficient-to-use-cout-or-printf

{

    // skip gui + input if told so

    if(gui_additional_turns_to_pass > 0){
        gui_additional_turns_to_pass -= 1;
        goto break_loop_command;
    }

    // gui + input

    for(;;){

        // clear display

        printf("%s", DISP_CLEAR);

        // draw map

        // TOD0 we could perhaps optimise the rendering by not changing the color twice for every tile
        for(auto& map_row : map){
            for(auto& tile : map_row){

                cout << tile.owner->color;

                if(tile.is_border){
                    cout << EFFECT_BOLD_ON;
                }

                if(tile.is_secondary_border){
                    cout << EFFECT_ITALIC_ON;
                    cout << EFFECT_UNDERLINE_ON;
                }

                if(tile.is_offensive_border){
                    cout << EFFECT_INVERSE_ON;
                }

                if(tile.is_deffensive_border){
                    cout << EFFECT_STRIKETROUGH_ON;
                }

                if(tile.owner == gui_marked_country){
                    cout << EFFECT_BLINK_ON;
                }

                // put either number of factories OR army strength

                float float_tile = 0.0;

                if(tile.is_border){

                    for(Tile* bordering_tile : tile.borders){
                        for(auto [bordering_country, bordering_country_border] : tile.owner->bordering_countries__border_length){

                            if(bordering_country == bordering_tile->owner){

                                int territory = tile.owner->active_borders;

                                // give a hint as to what that army strength is going to be if the conflict is esculated
                                if(!vec_contains(tile.owner->attacking, bordering_country) || !vec_contains(tile.owner->being_attacked, bordering_country)){
                                    territory += bordering_country_border;
                                }

                                float_tile = country_calc_eventual_unit_strength_if_no_limit(tile.owner->equipment, territory);
                                float_tile *= static_cast<float>(GUI_MAP_NUMERICALLY_REPRESENTABLE_VALUES) / GAME_MAX_BASE_UNIT_STRENGTH;

                                goto label_break_loop_determine_border_value;
                            }

                        }
                    }

                    assert(false);

                    label_break_loop_determine_border_value:
                    // float_tile *= GUI_ARMY_STRENGTH_MODIFIER;

                }else if(tile.is_secondary_border){

                    float_tile = tile.owner->base_unit_strength_if_no_limit * static_cast<float>(GUI_MAP_NUMERICALLY_REPRESENTABLE_VALUES) / GAME_MAX_BASE_UNIT_STRENGTH;

                }else{

                    float_tile = (tile.civs + tile.mils) * GUI_TILE_VALUE_MODIFIER;

                }

                // determine int value

                int int_tile = 0;

                if(isinf(float_tile) > 0){
                    // positive infinity
                    int_tile = INT_MAX;
                }else{
                    int_tile = static_cast<int>(floor(float_tile)); // or we could round?
                }

                char char_tile = '?';

                if(int_tile < 0){
                    char_tile = '-';
                }else{
                    if(int_tile <= 9){
                        char_tile = '0' + int_tile;
                    }else{
                        int_tile -= 10;
                        if(int_tile <= 25){
                            char_tile = 'A' + int_tile;
                        }else{
                            char_tile = '*';
                        }
                    }
                }

                cout << char_tile;

                // no need to turn off any of the effects, the reset will take care of that

                cout << COL_RESET;

            }
            printf("\n");
        }

        // draw player country

        cout << '\n';
        country_print(player);
        cout << '\n';

        // draw list of countries

        cout << '\n';

        {
            int iter = 0;

            for(Country* country : countries){

                if(iter > 0){
                    if(iter % GUI_HUD_COUNTRIES_PER_ROW == 0){
                        printf("\n");
                    }else{
                        printf(" ");
                    }
                }

                country_print(country);

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
        vector<string> cmds_pass_many = {"pm", "pass-many", "pass-many-turns"};

        vector<string> cmds_quit = {"qu", "quit", "quit-game"};

        vector<string> cmds_attack = {"a", "attack", "attack-country"};
        vector<string> cmds_stop_attacking = {"s", "stop-attack", "stop-attacking-country"};
        vector<string> cmds_info = {"i", "info", "show-country-information"};

        vector<string> cmds_construct_civs = {"cc", "construct-civs", "focus-construction-on-civillian-factories"};
        vector<string> cmds_construct_mils = {"cm", "construct-mils", "focus-construction-on-military-factories"};

        vector<string> cmds_mark = {"m", "mark", "mark-country", "visually-mark-or-unmark-a-country"};

        vector<vector<string>> cmds_ALL = {cmds_pass, cmds_pass_many, cmds_quit, cmds_attack, cmds_stop_attacking,
            cmds_info, cmds_construct_civs, cmds_construct_mils, cmds_mark};

        if(vec_contains(cmds_pass, command)){
            goto break_loop_command;
        
        }else if(vec_contains(cmds_pass_many, command)){

            cout << "Enter number of turns to pass: ";
            string str_turns;
            getline(cin, str_turns);

            int int_turns = atoi(str_turns.c_str());

            int_turns -= 1;
            if(int_turns < 0){
                int_turns = 0;
            }

            // cout << "turns:" << int_turns << '\n';
            // input_enter();

            gui_additional_turns_to_pass = int_turns;
            goto break_loop_command;

        }else if(vec_contains(cmds_quit, command)){
            goto break_loop_game;

        }else if(vec_contains(cmds_attack, command)){

            printf("Click on the country that you want to attack\n");
            Country *target = input_another_country(&map, player);
            vec_push_back_nodup(player->attacking, target);

        }else if(vec_contains(cmds_stop_attacking, command)){

            printf("Click on the country that you want to stop attacking\n");
            Country *piece_target = input_country(&map);
            vec_remove_if_exist(player->attacking, piece_target);

        }else if(vec_contains(cmds_info, command)){

            cout << "Click on the country that you want to show info for\n";
            Country *target = input_country(&map);
            country_print_long(target);
            cout << '\n';
            input_enter();

        }else if(vec_contains(cmds_construct_civs, command)){

            player->civ_production = CIV_PRODUCTION_CIV;

        }else if(vec_contains(cmds_construct_mils, command)){

            player->civ_production = CIV_PRODUCTION_MIL;

        }else if(vec_contains(cmds_mark, command)){
            // TODO this is worthless, if the user already knows where
            // the country is on the screen, what help would this be?

            Country* country = input_country(&map);
            if(gui_marked_country == country){
                gui_marked_country = nullptr;
            }else{
                gui_marked_country = country;
            }

        }else if("test" == command){

            for(;;){
                // auto [mouse_y, mouse_x] = input_mouse_click();
                // printf("y:%d x:%d\n", mouse_y, mouse_x);

                int num = random_int(0, 10);
                printf("num:%d\n", num);
            }

        }else if("test-a0" == command){

            vec_push_back_nodup(player->attacking, countries[0]);

        }else if("test-s0" == command){

            vec_remove_if_exist(player->attacking, countries[0]);

        }else if("test-eq" == command) {

            player->equipment += 1'000'000;

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

            input_enter();

            continue;
        }

        goto label_calculate_and_update_properties;
        // for example: declaring a war would change some of the tiles' properties
        // so in order to have that updated on the go, we'll just recalc everything

    }
    break_loop_command:

}
