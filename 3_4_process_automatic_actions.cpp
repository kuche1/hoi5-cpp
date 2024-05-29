
{

    // process civs

    for(Country* country : countries){

        // TODO instead of constructing all on a single tile the construction should be done across (say) 10% of your land

        float production = GAME_CIV_PRODUCE(country->civs);

        auto [failure, tile] = country_get_random_tile_based_on_density(country);
        if(failure){
            continue;
        }

        switch(country->civ_production){
            case CIV_PRODUCTION_CIV: {
                tile->civs += production;
            } break;

            case CIV_PRODUCTION_MIL: {
                tile->mils += production;
            } break;
        }
    }

    // process mils

    for(Country* country : countries){
        country->equipment += GAME_MIL_PRODUCE(country->mils);
    }

    // process wars

    for(Country* country : countries){

        if(country->equipment <= 0){
            // you can't attack if you don't have any equipment
            continue;
        }

        for(Country* country_at_war : country->at_war_with){

            vector<Tile*> tiles_to_process;

            for(int map_y=0; map_y<MAP_SIZE_Y; ++map_y){
                for(int map_x=0; map_x<MAP_SIZE_X; ++map_x){
                    Tile *tile = &map[map_y][map_x];
                    if(tile->owner == country){
                        tiles_to_process.push_back(tile);
                    }
                }
            }

            for(Tile* tile : tiles_to_process){
                for(Tile* border : tile->borders){
                    if(border->owner == country_at_war){

                        // loose equipment

                        country->equipment        -= GAME_ATK_EQUIPMENT_COST;
                        country_at_war->equipment -= GAME_DEF_EQUIPMENT_COST;

                        // damage land

                        border->civs *= 1.0 - GAME_PERCENT_FACTORIES_DESTROYED_ON_ATTACK;
                        border->mils *= 1.0 - GAME_PERCENT_FACTORIES_DESTROYED_ON_ATTACK;

                        // determine battle result

                        float deffender_multiplier = 1.0;

                        if(country_at_war->equipment <= 0){
                            deffender_multiplier *= GAME_DEF_NO_EQUIPMENT_MULTIPLIER;
                        }

                        if(random_0_to_1() * deffender_multiplier < GAME_ATK_WIN_CHANCE){
                            // battle has been won, transfer land

                            border->owner = country;

                            border->civs *= 1.0 - GAME_PERCENT_FACTORIES_DESTROYED_ON_LAND_TRANSFER;
                            border->mils *= 1.0 - GAME_PERCENT_FACTORIES_DESTROYED_ON_LAND_TRANSFER;
                        }

                    }
                }
            }

        }
    }

}
