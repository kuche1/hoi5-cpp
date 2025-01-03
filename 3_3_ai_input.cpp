
// TODO
//
// make it so that if a country has a lot of factories and not a lot of equipment it ABSOLUTELY
// GETS GANG BANGED (as to prevent it from becoming too powerful in the future)
//
// make it so that if there are a couple of big countries they fight it out and do not bother the small
// ones too much
//
// the question that countries should be asking is "is it worth loosing my equipment in order to get this territory"
//
// in fact, if a country takes too much of the world all other countries MUST UNITE and ciece to fight
// if we can get them attacking synchrynously that would be awesome
//
// the building logic is fucked, AI is building civs when should abviously build mils (not a bug)

{

    // AI

    // determine if extreme conditions are met
    // TODO this MUST NOT be here
    // TODO if we can expand this to top2 or top3 or topN and have the AI auto calc the alliances that would be awesome

    Country* gangbang_target = NULL;

    {

        Country* most_powerful_mf = countries[0];

        for(Country* country : countries){
            if(country->factories > most_powerful_mf->factories){
                most_powerful_mf = country;
            }
        }

        float powerful_mf_power = 0.0;
        float rest_of_world_power = 0.0;

        for(Country* country : countries){
            if(country == most_powerful_mf){
                powerful_mf_power += country->factories;
            }else{
                rest_of_world_power += country->factories;
            }
        }

        if(powerful_mf_power * AI_ENGAGE_GANGBANG_AT_RATIO >= rest_of_world_power){
            gangbang_target = most_powerful_mf;
        }

    }

    for(Country* country : countries){

        // skip AI calc for human player and for `nobody`
        if((country == player) || (country == nobody)){
            continue;
        }

        // start war

        {
            // kill this MF
            if(gangbang_target != NULL){
                vec_push_back_nodup(country->attacking, gangbang_target);
            }

            // this actually turned out to be a bad idea since there are situations where the only way of
            //  winning is to attack back
            // // don't start any new wars if we are being attacked
            // if(country->being_attacked.size() > 0){
            //     goto label_dont_start_any_new_wars;
            // }

            // there is a change that the AI is going to skip on starting any new wars if it doesn't have enough strength
            if(country->base_unit_strength < GAME_MAX_BASE_UNIT_STRENGTH){
                if(random_0_to_1() <= AI_DONT_START_WAR_CHANCE_IF_NOT_FULL_STRENGTH){
                    goto label_dont_start_any_new_wars;
                }
            }

            // if we have much more equipment than them
            for(auto [bordering_country, _border_length] : country->bordering_countries__border_length){
                if(bordering_country->equipment <= country->equipment * AI_START_WAR_IF_NEIGHBOUR_HAS_LESS_EQUIPMENT_THRESHOLD){
                    vec_push_back_nodup(country->attacking, bordering_country);
                }
            }

            // if they have much more factories
            for(auto [bordering_country, _border_length] : country->bordering_countries__border_length){
                if(bordering_country->factories >= country->factories * AI_START_WAR_IF_NEIGHBOUR_HAS_MORE_FACTORIES_THRESHOLD){
                    vec_push_back_nodup(country->attacking, bordering_country);
                }
            }

            // if we're bordering someone worse equipment/factory ratio
            for(auto [bordering_country, _border_length] : country->bordering_countries__border_length){

                float his_ratio = bordering_country->equipment / bordering_country->factories;
                float our_ratio = country->equipment / country->factories;

                if(his_ratio * AI_START_WAR_FOR_EQUIPMENT_FACTORY_RATIO_THRESHOLD < our_ratio){
                    vec_push_back_nodup(country->attacking, bordering_country);
                }
            }

        }
        label_dont_start_any_new_wars:

        // end war

        {

            // stop attacking everyone but the gangbang target
            if(gangbang_target != NULL){
                for(Country* country_that_we_are_attacking : ranges::reverse_view(country->attacking)){
                    if(country_that_we_are_attacking != gangbang_target){
                        vec_remove(country->attacking, country_that_we_are_attacking);
                    }
                }
            }

            // if we have much less equipment than any of our neighbours
            for(auto [neighbour, _border_length] : country->bordering_countries__border_length){
                if(neighbour->equipment * AI_STOP_WAR_IF_WE_HAVE_LESS_EQUIPMENT_THRESHOLD > country->equipment){
                    for(Country* country_that_we_are_attacking : ranges::reverse_view(country->attacking)){
                        vec_remove(country->attacking, country_that_we_are_attacking);
                    }
                }
            }

            // if we have less equipment that what we were to produce in a handful of turns, stop attacking
            // since if we got attacked ourselves we would be defenseless
            float production_for_short_amount_of_time = GAME_MIL_PRODUCE(country->mils) * AI_STOP_WAR_IF_PRODUCTION_FOR_A_COUPLE_OF_DAYS_EXCEEDS_EQUIPMENT;
            if(production_for_short_amount_of_time >= country->equipment){
                for(Country* country_at_war : ranges::reverse_view(country->attacking)){
                    vec_remove_if_exist(country->attacking, country_at_war);
                }
            }

            // if someone is attacking both you and your enemy (who you are in a war with), end the war
            for(Country* country_attacking_us : countries){
                if(!vec_contains(country_attacking_us->attacking, country)){
                    continue;
                }

                for(Country* enemy : ranges::reverse_view(country->attacking)){
                    if(vec_contains(country_attacking_us->attacking, enemy)){
                        vec_remove(country->attacking, enemy);
                    }
                }
            }

        }

        // determine weather to build civs or mils

        {
            // if both we're attacking a given country
            // AND they're attacking us, the numbers will double
            float total_enemy_factories = 0.0;
            float total_enemy_equipment = 0.0;
            int total_enemy_tiles = 0;

            // calc countries that we're attacking
            for(Country* country_at_war : country->attacking){
                total_enemy_factories += country_at_war->civs + country_at_war->mils;
                total_enemy_equipment += country_at_war->equipment;
                total_enemy_tiles += country_at_war->tiles.size();
            }

            // calc countries that are attacking us
            for(Country* attacker : countries){
                if(vec_contains(attacker->attacking, country)){
                    total_enemy_factories += attacker->civs + attacker->mils;
                    total_enemy_equipment += attacker->equipment;
                    total_enemy_tiles += attacker->tiles.size();
                }
            }
            
            // TODO
            // also take bordering nations into consideration
            // for example: if a country on our border has much less civs and much more mils than us

            // decide what to build
            if(
                total_enemy_factories >= country->civs + country->mils ||
                total_enemy_equipment >= country->equipment ||
                total_enemy_tiles >= static_cast<int>( country->tiles.size() )
            ){
                country->civ_production = CIV_PRODUCTION_MIL;
            }else{
                country->civ_production = CIV_PRODUCTION_CIV;
            }

            // TODO perhaps we should add some modifiers here
        }

    }

}
