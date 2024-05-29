{

    // update number of factories based on land

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

    // update number of tiles

    for(Country* country : countries){

        country->tiles = 0;

        for(int y=0; y<MAP_SIZE_Y; ++y){
            for(int x=0; x<MAP_SIZE_X; ++x){
                Tile *tile = &map[y][x];
                if(tile->owner == country){
                    country->tiles += 1;
                }
            }
        }

    }

    // update country borders

    for(Country* country : countries){
        country->bordering_countries = {};

        vector<Tile*> tiles = country_get_tiles(country, &map);
        for(Tile* tile : tiles){
            for(Tile* border_tile : tile->borders){
                Country* bordering_country = border_tile->owner;
                if(bordering_country != country){
                    vec_push_back_nodup(country->bordering_countries, bordering_country);
                }
            }
        }
    }

    // update tiles

    for(int y=0; y<MAP_SIZE_Y; ++y){
        for(int x=0; x<MAP_SIZE_X; ++x){
            Tile* tile = &map[y][x];

            tile->is_war_border = false;

            for(Country* country_at_war : tile->owner->at_war_with){
                for(Tile* tile_border : tile->borders){
                    if(country_at_war == tile_border->owner){
                        tile->is_war_border = true;
                    }
                }
            }

        }
    }

}