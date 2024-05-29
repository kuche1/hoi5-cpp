
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
        for(auto map_row : map){
            for(auto tile : map_row){

                float float_factories = (tile.civs + tile.mils) * MAP_TILE_VALUE_MODIFIER;

                int int_factories = static_cast<int>( floor(float_factories) );

                char char_factories = '?';

                if(int_factories <= 9){
                    char_factories = '0' + int_factories;
                }else{
                    int_factories -= 10;
                    if(int_factories <= 25){
                        char_factories = 'A' + int_factories;
                    }else{
                        char_factories = '+';
                    }
                }

                cout << tile.owner->color;

                if(tile.is_war_border){
                    cout << EFFECT_INVERSE_ON;
                }

                cout << char_factories;

                // no need, the reset will take care of this
                // if(tile.is_war_border){
                //     cout << EFFECT_INVERSE_ON;
                // }

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

                if(country->tiles.size() <= 0){
                    continue;
                }

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
        vector<string> cmds_pass_50 = {"p50", "pass50", "pass-50-turns"};
        vector<string> cmds_pass_200 = {"p200", "pass200", "pass-200-turns"};

        vector<string> cmds_quit = {"q", "quit", "quit-game"};

        vector<string> cmds_attack = {"a", "attack", "attack-country"};
        vector<string> cmds_stop_attacking = {"s", "stop-attack", "stop-attacking-country"};
        vector<string> cmds_info = {"i", "info", "show-country-information"};

        vector<string> cmds_construct_civs = {"cc", "construct-civs", "focus-construction-on-civillian-factories"};
        vector<string> cmds_construct_mils = {"cm", "construct-mils", "focus-construction-on-military-factories"};

        vector<vector<string>> cmds_ALL = {cmds_pass, cmds_pass_10, cmds_pass_50, cmds_pass_200, cmds_quit, cmds_attack, cmds_stop_attacking,
            cmds_info, cmds_construct_civs, cmds_construct_mils};

        if(vec_contains(cmds_pass, command)){
            goto break_loop_command;
        
        }else if(vec_contains(cmds_pass_10, command)){
            gui_additional_turns_to_pass = 10 - 1;
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
            country_print(target);
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
        }

    }
    break_loop_command:

}
