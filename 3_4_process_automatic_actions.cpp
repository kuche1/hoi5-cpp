
{

    // construct with civs

    for(Country* country : countries){

        // TODO instead of constructing all on a single tile the construction should be done across (say) 10% of your land

        float production = GAME_CIV_PRODUCE(country->civs);

        // auto [failure, tile] = country_get_random_tile_based_on_density(country);
        // if(failure){
        //     continue;
        // }
        Tile* tile = country_get_random_tile_based_on_density(country);

        switch(country->civ_production){
            case CIV_PRODUCTION_CIV: {
                tile->civs += production;
            } break;

            case CIV_PRODUCTION_MIL: {
                tile->mils += production;
            } break;
        }
    }

    // produce equipment with mils

    for(Country* country : countries){
        country->equipment += GAME_MIL_PRODUCE(country->mils);
    }

    // transfer some of base civs/mils onto land

    for(Country* country : countries){

        if(country->civs_base > 0){
            float factories = country->civs_base * GAME_PLACE_BASE_FACTORY_RATE;
            country->civs_base -= factories;
            Tile* tile = country_get_random_tile_based_on_density(country);
            tile->civs += factories;
        }

        if(country->mils_base > 0){
            float factories = country->mils_base * GAME_PLACE_BASE_FACTORY_RATE;
            country->mils_base -= factories;
            Tile* tile = country_get_random_tile_based_on_density(country);
            tile->mils += factories;
        }

    }

    // process wars

    for(Country* country : countries){

        if(country->equipment <= 0){
            // you can't attack if you don't have any equipment
            continue;
        }

        for(Country* country_at_war : country->attacking){

            for(Tile* tile : country->tiles){
                for(Tile* border : tile->borders){
                    if(border->owner == country_at_war){

                        // loose equipment

                        country->equipment        -= GAME_ATK_EQUIPMENT_COST;
                        country_at_war->equipment -= GAME_DEF_EQUIPMENT_COST;

                        // damage land

                        float destroyed_civs = border->civs * GAME_PERCENT_FACTORIES_DESTROYED_ON_ATTACK;
                        float destroyed_mils = border->mils * GAME_PERCENT_FACTORIES_DESTROYED_ON_ATTACK;

                        border->civs -= destroyed_civs;
                        border->mils -= destroyed_mils;

                        float total_destroyed_factories = destroyed_civs + destroyed_mils;
                        // used in the battle calculation

                        // determine battle result

                        float deffender_multiplier = 1.0;

                        if(country_at_war->equipment <= 0){
                            deffender_multiplier *= GAME_DEF_NO_EQUIPMENT_MULTIPLIER;
                        }

                        if(country_at_war->deffensive_unit_strength > country->offensive_unit_strength){
                            deffender_multiplier *= GAME_DEF_MULTIPLIER_IF_MORE_STRENGTH;
                        }

                        deffender_multiplier += deffender_multiplier * (GAME_DEFFENDER_BOOST_PER_FACTORY_DESTROYED * total_destroyed_factories);

                        float offender_multiplier = 1.0; // TODO debuff if army is not full strength

                        if(country->offensive_unit_strength > country_at_war->deffensive_unit_strength){
                            offender_multiplier *= GAME_ATK_MULTIPLIER_IF_MORE_STRENGTH;
                        }

                        if(random_0_to_1() * deffender_multiplier < GAME_ATK_WIN_CHANCE * offender_multiplier){
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
