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

    for(Country* country : countries){
        country->bordering_countries = {};

        for(Tile* tile : country->tiles){
            for(Tile* border_tile : tile->borders){
                Country* bordering_country = border_tile->owner;
                if(bordering_country != country){
                    vec_push_back_nodup(country->bordering_countries, bordering_country);
                }
            }
        }
    }

    // tiles: update "war border" property

    for(auto& map_row : map){
        for(auto& tile : map_row){

            tile.is_war_border = false;

            for(Country* country_at_war : tile.owner->at_war_with){
                for(Tile* tile_border : tile.borders){
                    if(country_at_war == tile_border->owner){
                        tile.is_war_border = true; // TODO is_offensive_border
                        tile.owner->offensive_borders += 1;
                        country_at_war->deffensive_borders += 1;
                    }
                }
            }

        }
    }

    // country: update unit strengths
    // TODO there should be 2 miltipliers - 1 for offensive border, and 1 for deffensive border

    for(Country* country : countries){
        country->offensive_unit_strength = country->equipment / country->offensive_borders;
        country->deffensive_unit_strength = country->equipment / country->deffensive_borders;
    }

}
