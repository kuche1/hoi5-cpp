
///////////
//////////////
///////////////// gameplay
//////////////
///////////

#define GAME_CIV_PRODUCE(number_of_civs) (floor(number_of_civs) * 0.001)

#define GAME_MIL_PRODUCE(number_of_mils) (floor(number_of_mils) * 21.0)

#define GAME_ATK_WIN_CHANCE 0.11 // what is the base change (0.0 to 1.0) that a terriroty would be gained upon attack

#define GAME_ATK_EQUIPMENT_COST 12.0 // how much equipment a single attack costs
#define GAME_DEF_EQUIPMENT_COST (GAME_ATK_EQUIPMENT_COST * 0.15) // how much equipment does it cost to deffend an attack

#define GAME_DEF_NO_EQUIPMENT_MULTIPLIER 0.6

#define GAME_PERCENT_FACTORIES_DESTROYED_ON_LAND_TRANSFER 0.09 // during the land transfer some of the factories are destroyed

#define GAME_PERCENT_FACTORIES_DESTROYED_ON_ATTACK 0.04 // when a tile is being attacked, some of the factories also get damaged

#define GAME_STARTING_EQUIPMENT (25'000)

#define GAME_DEF_MULTIPLIER_IF_MORE_STRENGTH 1.2
#define GAME_ATK_MULTIPLIER_IF_MORE_STRENGTH 1.2

// I heard somewhere that in IRL if an army is to take some territory, the attacking forces need to be 3 times more than the deffending forces
#define GAME_OFFENSIVE_STRENGTH_MULTIPLIER  0.5
#define GAME_DEFFENSIVE_STRENGTH_MULTIPLIER 1.5

#define GAME_MAX_BASE_UNIT_STRENGTH 200.0

#define GAME_PLACE_BASE_FACTORY_RATE 0.004 // how many of your base factories are you going to loose and put on your land per turn

#define GAME_DEFFENDER_BOOST_PER_FACTORY_DESTROYED 1.5 // if an attacker destroys 1 factory, the deffender's deffence will increase by [define value] of itself
