
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

            for(Tile* tile : country->tiles){
                for(Tile* border : tile->borders){
                    if(border->owner == country_at_war){

                        // loose equipment

                        country->equipment        -= GAME_ATK_EQUIPMENT_COST;
                        country_at_war->equipment -= GAME_DEF_EQUIPMENT_COST;

                        // damage land

                        border->civs *= 1.0 - GAME_PERCENT_FACTORIES_DESTROYED_ON_ATTACK;
                        border->mils *= 1.0 - GAME_PERCENT_FACTORIES_DESTROYED_ON_ATTACK;

                        // determine battle result
                        // TODO take the unit strength values into account, in a way that doesn't suck
                        // TODO perhaps it would be a good idea to delete as much equipment as it's actually used (so the unit strenght (or some avg unit strength without the multipliers)); we could also probably have MAX_EQUIPMENT_PER_UNIT or something like that

                        float deffender_multiplier = 1.0;

                        if(country_at_war->equipment <= 0){
                            deffender_multiplier *= GAME_DEF_NO_EQUIPMENT_MULTIPLIER;
                        }

                        if(country_at_war->deffensive_unit_strength > country->offensive_unit_strength){
                            deffender_multiplier = GAME_DEF_MULTIPLIER_IF_MORE_STRENGTH;
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
