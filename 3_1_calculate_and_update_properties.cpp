{

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
        country->bordering_countries = {};
        country->borders_with_other_countries = 0;

        for(Tile* tile : country->tiles){
            for(Tile* border_tile : tile->borders){
                Country* bordering_country = border_tile->owner;
                if(bordering_country != country){
                    vec_push_back_nodup(country->bordering_countries, bordering_country);
                    tile->is_border = true;
                    country->borders_with_other_countries += 1;
                }
            }
        }
    }

    // tiles: update "war border" property

    for(auto& map_row : map){
        for(auto& tile : map_row){
            tile.is_offensive_border = false;
            tile.is_deffensive_border = false;
        }
    }

    for(auto& map_row : map){
        for(auto& tile : map_row){

            for(Country* country_at_war : tile.owner->at_war_with){
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
        float strength_per_active_tile = country->equipment / static_cast<float>(total_active_borders);

        country->offensive_unit_strength  = strength_per_active_tile * GAME_OFFENSIVE_STRENGTH_MULTIPLIER;
        country->deffensive_unit_strength = strength_per_active_tile * GAME_DEFFENSIVE_STRENGTH_MULTIPLIER;

        country->average_unit_strength = country->equipment / static_cast<float>(country->borders_with_other_countries);
    }

    // country: end war if no longer sharing a border

    for(Country* country : countries){
        for(Country* country_at_war : ranges::reverse_view(country->at_war_with)){
            if(!vec_contains(country->bordering_countries, country)){
                vec_remove_if_exist(country->bordering_countries, country_at_war);
            }
        }
    }

    // country: delete if no tiles left

    for(Country* country : ranges::reverse_view(countries)){
        if(country->tiles.size() <= 0){
            vec_remove_if_exist(countries, country);
            // TOD0 we're memory leaking here since no one is referencing the country anymore, however this allows us to not care about rendering `player` if he is dead
        }
    }

}
