
///////////
//////////////
///////////////// gameplay
//////////////
///////////

#define GAME_CIV_PRODUCE(number_of_civs) (floor(number_of_civs) * 0.0004)

#define GAME_MIL_PRODUCE(number_of_mils) (floor(number_of_mils) * 23.0)

#define GAME_ATK_WIN_CHANCE 0.15 // what is the change (0.0 to 1.0) that a terriroty would be gained upon attack

#define GAME_ATK_EQUIPMENT_COST 16.0 // how much equipment a single attack costs
#define GAME_DEF_EQUIPMENT_COST (GAME_ATK_EQUIPMENT_COST * 0.15) // how much equipment does it cost to deffend an attack

#define GAME_DEF_NO_EQUIPMENT_MULTIPLIER 0.6

#define GAME_PERCENT_FACTORIES_DESTROYED_ON_LAND_TRANSFER 0.09 // during the land transfer some of the factories are destroyed

#define GAME_PERCENT_FACTORIES_DESTROYED_ON_ATTACK 0.04 // when a tile is being attacked, some of the factories also get damaged

#define GAME_STARTING_EQUIPMENT (25'000)
