
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
                    cout << EFFECT_ITALIC_ON;
                }

                if(tile.is_secondary_border){
                    cout << EFFECT_UNDERLINE_ON;
                }

                if(tile.is_offensive_border){
                    cout << EFFECT_INVERSE_ON;
                }

                if(tile.is_deffensive_border){
                    cout << EFFECT_STRIKETROUGH_ON;
                }

                // put either number of factories OR army strength

                float float_tile = 0.0;

                if(tile.is_border){

                    for(Tile* bordering_tile : tile.borders){
                        for(auto [bordering_country, bordering_country_border] : tile.owner->bordering_countries_and_borders){
                            if(bordering_country == bordering_tile->owner){
                                float_tile = tile.owner->equipment / bordering_country_border;
                                goto label_break_loop_determine_border_value;
                            }
                        }
                    }

                    assert(false);
                    // float_tile = tile.owner->average_unit_strength * GUI_ARMY_STRENGTH_MODIFIER;

                    label_break_loop_determine_border_value:
                    float_tile *= GUI_ARMY_STRENGTH_MODIFIER;

                }else{

                    float_tile = (tile.civs + tile.mils) * GUI_TILE_VALUE_MODIFIER;

                }

                int int_tile = static_cast<int>(floor(float_tile)); // or we could round?

                char char_tile = '?';

                if(int_tile < 0){
                    char_tile = '0';
                }else{
                    int_tile += 1;
                    if(int_tile <= 9){
                        char_tile = '0' + int_tile;
                    }else{
                        int_tile -= 10;
                        if(int_tile <= 25){
                            char_tile = 'A' + int_tile;
                        }else{
                            char_tile = '+';
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
                    if(iter % 3 == 0){
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

        vector<string> cmds_pass_10 = {"p10", "pass10", "pass-10-turns"};
        vector<string> cmds_pass_20 = {"p20", "pass20", "pass-20-turns"};
        vector<string> cmds_pass_50 = {"p50", "pass50", "pass-50-turns"};
        vector<string> cmds_pass_200 = {"p200", "pass200", "pass-200-turns"};

        vector<string> cmds_quit = {"qu", "quit", "quit-game"};

        vector<string> cmds_attack = {"a", "attack", "attack-country"};
        vector<string> cmds_stop_attacking = {"s", "stop-attack", "stop-attacking-country"};
        vector<string> cmds_info = {"i", "info", "show-country-information"};

        vector<string> cmds_construct_civs = {"cc", "construct-civs", "focus-construction-on-civillian-factories"};
        vector<string> cmds_construct_mils = {"cm", "construct-mils", "focus-construction-on-military-factories"};

        vector<vector<string>> cmds_ALL = {cmds_pass, cmds_pass_10, cmds_pass_20, cmds_pass_50, cmds_pass_200, cmds_quit, cmds_attack, cmds_stop_attacking,
            cmds_info, cmds_construct_civs, cmds_construct_mils};

        if(vec_contains(cmds_pass, command)){
            goto break_loop_command;
        
        }else if(vec_contains(cmds_pass_10, command)){
            gui_additional_turns_to_pass = 10 - 1;
            goto break_loop_command;

        }else if(vec_contains(cmds_pass_20, command)){
            gui_additional_turns_to_pass = 20 - 1;
            goto break_loop_command;

        }else if(vec_contains(cmds_pass_50, command)){
            gui_additional_turns_to_pass = 50 - 1;
            goto break_loop_command;

        }else if(vec_contains(cmds_pass_200, command)){
            gui_additional_turns_to_pass = 200 - 1;
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

        }else if("test" == command){

            for(;;){
                // auto [mouse_y, mouse_x] = input_mouse_click();
                // printf("y:%d x:%d\n", mouse_y, mouse_x);

                int num = random_int(0, 10);
                printf("num:%d\n", num);
            }

        }else if("test-a0" == command){

            vec_push_back_nodup(player->at_war_with, countries[0]);

        }else if("test-s0" == command){

            vec_remove_if_exist(player->at_war_with, countries[0]);

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
