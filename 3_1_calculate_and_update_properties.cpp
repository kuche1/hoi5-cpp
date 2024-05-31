{
    label_calculate_and_update_properties:

    // country: update owned tiles

    for(Country* country : countries){
        country->tiles = {};
    }

    for(auto& map_row : map){
        for(auto& tile : map_row){
            tile.owner->tiles.push_back(&tile);
        }
    }

    // country: update number of factories

    for(Country* country : countries){
        country->civs = country->civs_base;
        country->mils = country->mils_base;
    }

    for(int y=0; y<MAP_SIZE_Y; ++y){
        for(int x=0; x<MAP_SIZE_X; ++x){
            Tile* tile = &map[y][x];
            tile->owner->civs += tile->civs;
            tile->owner->mils += tile->mils;
        }
    }

    for(Country* country : countries){
        country->factories = country->civs + country->mils;
    }

    // country: update "borders countries" property
    // tile: update "border" property
    // country: update "borders with other countries" property

    for(auto& map_row : map){
        for(auto& tile : map_row){
            tile.is_border = false;
        }
    }

    for(Country* country : countries){
        country->borders = {};
        country->bordering_countries__border_length = {};
        country->borders_with_other_countries = 0;

        vector<Country*> bordering_countries = {};
        vector<int> bordering_country_borders = {};

        for(Tile* tile : country->tiles){
            for(Tile* border_tile : tile->borders){
                Country* bordering_country = border_tile->owner;
                if(bordering_country != country){

                    tile->is_border = true;
                    country->borders_with_other_countries += 1;
                    country->borders.push_back(tile);

                    if(vec_contains(bordering_countries, bordering_country)){
                        int bordering_country_idx = vec_get_index(bordering_countries, bordering_country);
                        bordering_country_borders[bordering_country_idx] += 1;
                    }else{
                        bordering_countries.push_back(bordering_country);
                        bordering_country_borders.push_back(1);
                    }

                }
            }
        }

        for(auto [bordering_country, border_length] : vec_zip(bordering_countries, bordering_country_borders)){
            country->bordering_countries__border_length[bordering_country] = border_length;
        }
    }

    // tile: update "secondary border" property

    for(Country* country : countries){

        country->secondary_borders = {};

        for(Tile* tile : country->tiles){
            tile->is_secondary_border = false;
        }

    }

    for(Country* country : countries){
        for(Tile* border_tile : country->borders){

            for(Tile* secondary_border_tile : border_tile->borders){
                if(secondary_border_tile->owner != country){
                    continue;
                }
                if(secondary_border_tile->is_border){
                    continue;
                }
                secondary_border_tile->is_secondary_border = true;
                country->secondary_borders.push_back(secondary_border_tile);
            }

        }
    }

    // tile: update "trenary border" property
    // TODO I feel like this is too inefficient, and it would be better to use "distance from border" field or something like that

    for(Country* country : countries){

        country->trenary_borders = {};

        for(Tile* tile : country->tiles){
            tile->is_trenary_border = false;
        }

    }

    for(Country* country : countries){
        for(Tile* secondary_border_tile : country->secondary_borders){

            for(Tile* trenary_border_tile : secondary_border_tile->borders){
                if(trenary_border_tile->owner != country){
                    assert(false);
                }
                if(trenary_border_tile->is_border){
                    continue;
                }
                if(trenary_border_tile->is_secondary_border){
                    continue;
                }
                trenary_border_tile->is_trenary_border = true;
                country->trenary_borders.push_back(trenary_border_tile);
            }

        }
    }

    // tiles: update "offensive/defensive border" property

    for(Country* country : countries){
        country->offensive_borders = 0;
        country->deffensive_borders = 0;
    }

    for(auto& map_row : map){
        for(auto& tile : map_row){
            tile.is_offensive_border = false;
            tile.is_deffensive_border = false;
        }
    }

    for(auto& map_row : map){
        for(auto& tile : map_row){

            for(Country* country_at_war : tile.owner->attacking){
                for(Tile* tile_border : tile.borders){
                    if(country_at_war == tile_border->owner){

                        tile.is_offensive_border = true;
                        tile.owner->offensive_borders += 1;

                        tile_border->is_deffensive_border = true;
                        tile_border->owner->deffensive_borders += 1;
                    }
                }
            }

        }
    }

    // country: update unit strengths

    for(Country* country : countries){

        int total_active_borders = country->offensive_borders + country->deffensive_borders;
        if(total_active_borders <= 0){
            // without this it always looks as if we're on max strength
            // it would be much better if we acted as if we're guarding all out borders instead
            total_active_borders = static_cast<float>(country->borders_with_other_countries);
        }

        float strength_per_active_tile = country->equipment / static_cast<float>(total_active_borders);

        if(strength_per_active_tile > GAME_MAX_UNIT_BASE_STRENGTH){
            strength_per_active_tile = GAME_MAX_UNIT_BASE_STRENGTH;
        }

        country->base_unit_strength       = strength_per_active_tile;
        country->offensive_unit_strength  = strength_per_active_tile * GAME_OFFENSIVE_STRENGTH_MULTIPLIER;
        country->deffensive_unit_strength = strength_per_active_tile * GAME_DEFFENSIVE_STRENGTH_MULTIPLIER;
    }

    // country: end war if no longer sharing a border
    // country: delete if no tiles left

    for(Country* country : countries){
        for(Country* country_at_war : ranges::reverse_view(country->attacking)){
            if(!map_contains(country->bordering_countries__border_length, country_at_war)){
                vec_remove_if_exist(country->attacking, country_at_war);
            }
        }
    }

    for(Country* country : ranges::reverse_view(countries)){
        if(country->tiles.size() <= 0){
            vec_remove_if_exist(countries, country);
            // TOD0 we're memory leaking here since no one is referencing the country anymore, however this allows us to not care about rendering `player` if he is dead
        }
    }

    // country, tile: take equipment from non-border tiles, and put it to the border tiles
    // TODO implement, also we need to create a tile_change_owner that returns the equipment back to it's controller, aosl
    //  we could make it so that if a country is defeated you get it's stockpile

    // country: update "being attacked by" property

    for(Country* country : countries){
        country->being_attacked = {};
    }

    for(Country* country_attacking : countries){
        for(Country* country_defending : country_attacking->attacking){
            vec_push_back_nodup(country_defending->being_attacked, country_attacking);
        }
    }

}
