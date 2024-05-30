
// TODO
//
// make it so that if a country has a lot of factories and not a lot of equipment it will get attacked
// (as to prevent it from becoming too powerful in the future)
//
// make it so that wars last longer

{

    // AI

    for(Country* country : countries){

        // skip AI calc for human player and for `nobody`
        if((country == player) || (country == nobody)){
            continue;
        }

        // TOD0 we can synchronise the stop-war and build-mils actions
        // (altho in this case it might not be necessary)

        // start war

        {
            // if we have much more equipment than them
            for(Country* bordering_country : country->bordering_countries){
                if(bordering_country->equipment < country->equipment * AI_START_WAR_IF_NEIGHBOUR_HAS_LESS_EQUIPMENT_THRESHOLD){
                    vec_push_back_nodup(country->at_war_with, bordering_country);
                }
            }

            // if we're bordering someone with a lot of factories, but not a lot of equipment
            for(Country* bordering_country : country->bordering_countries){

                float his_ratio = bordering_country->equipment / bordering_country->factories;
                float our_ratio = country->equipment / country->factories;

                if(his_ratio < our_ratio){
                    vec_push_back_nodup(country->at_war_with, bordering_country);
                }
            }

            // TODO ako nqkoi ve4e te natqga nedei da po4va6 novi voini
        }

        // end war

        {

            // if we have much less equipment than them
            for(Country* country_at_war : ranges::reverse_view(country->at_war_with)){
                if(country_at_war->equipment * AI_STOP_WAR_IF_WE_HAVE_LESS_EQUIPMENT_THRESHOLD > country->equipment){
                    vec_remove_if_exist(country->at_war_with, country_at_war);
                }
            }

            // if we have less equipment that what we were to produce in a handful of turns, stop attacking
            // since if we got attacked ourselves we would be defenseless
            float production_for_short_amount_of_time = GAME_MIL_PRODUCE(country->mils) * AI_STOP_WAR_IF_PRODUCTION_FOR_A_COUPLE_OF_DAYS_EXCEEDS_EQUIPMENT;
            if(production_for_short_amount_of_time >= country->equipment){
                for(Country* country_at_war : ranges::reverse_view(country->at_war_with)){
                    vec_remove_if_exist(country->at_war_with, country_at_war);
                }
            }

            // if someone is attacking both you and your enemy (who you are in a war with), end the war
            for(Country* country_attacking_us : countries){
                if(!vec_contains(country_attacking_us->at_war_with, country)){
                    continue;
                }

                for(Country* enemy : ranges::reverse_view(country->at_war_with)){
                    if(vec_contains(country_attacking_us->at_war_with, enemy)){
                        vec_remove(country->at_war_with, enemy);
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
            for(Country* country_at_war : country->at_war_with){
                total_enemy_factories += country_at_war->civs + country_at_war->mils;
                total_enemy_equipment += country_at_war->equipment;
                total_enemy_tiles += country_at_war->tiles.size();
            }

            // calc countries that are attacking us
            for(Country* attacker : countries){
                if(vec_contains(attacker->at_war_with, country)){
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
