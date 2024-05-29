
// create country: cobody

vector<Country*> countries;
// TOD0 one memory leak for each country, too bad I dont give a shit haha

Country* nobody = new Country{
    .name = "Nobody",
    .color = COL_RESET,
    .civs_base = 0,
    .mils_base = 0,
    .civ_production = CIV_PRODUCTION_CIV,
    .equipment = -1,
};

countries.push_back(nobody);

// load map

vector<vector<Tile>> map;

for(int y=0; y<MAP_SIZE_Y; ++y){
    vector<Tile> row;
    for(int x=0; x<MAP_SIZE_X; ++x){
        Tile tile = {
            .owner = nobody,
            .borders = {},
        };
        row.push_back(tile);
    }
    map.push_back(row);
}

// conect tiles

for(int y=0; y<MAP_SIZE_Y; ++y){
    for(int x=0; x<MAP_SIZE_X; ++x){
        
        Tile *tile = &map[y][x];

        for(auto [new_y, new_x] : {make_tuple(y-1, x), make_tuple(y, x+1), make_tuple(y+1, x), make_tuple(y, x-1)}) {

            // y

            if(new_y < 0){
                if(MAP_LOOPS_Y){
                    new_y += MAP_SIZE_Y;
                }else{
                    continue;
                }
            }

            if(new_y >= MAP_SIZE_Y){
                if(MAP_LOOPS_Y){
                    new_y -= MAP_SIZE_Y;
                }else{
                    continue;
                }
            }

            // x

            if(new_x < 0){
                if(MAP_LOOPS_X){
                    new_x += MAP_SIZE_X;
                }else{
                    continue;
                }
            }

            if(new_x >= MAP_SIZE_X){
                if(MAP_LOOPS_X){
                    new_x -= MAP_SIZE_X;
                }else{
                    continue;
                }
            }

            // connect borders

            tile->borders.push_back(&map[new_y][new_x]);

        }

        assert(tile->borders.size() <= MAP_TILE_BORDERS_MAX_LEN);
    }
}

// load countries

countries.push_back(
    new Country{
        .name = "Russia",
        .color = COL_INT(124),
        .civs_base = 45,
        .mils_base = 32,
        .civ_production = CIV_PRODUCTION_CIV,
        .equipment = 1'000,
    }
);

countries.push_back(
    new Country{
        .name = "Germany",
        .color = COL_YELLOW_DARK,
        .civs_base = 34,
        .mils_base = 28,
        .civ_production = CIV_PRODUCTION_CIV,
        .equipment = 1'000,
    }
);

countries.push_back(
    new Country{
        .name = "Poland",
        .color = COL_INT(13),
        .civs_base = 17,
        .mils_base = 9,
        .civ_production = CIV_PRODUCTION_CIV,
        .equipment = 1'000,
    }
);

countries.push_back(
    new Country{
        .name = "Turkey",
        .color = COL_INT(144),
        .civs_base = 11,
        .mils_base = 4,
        .civ_production = CIV_PRODUCTION_CIV,
        .equipment = 1'000,
    }
);

countries.push_back(
    new Country{
        .name = "Grece",
        .color = COL_INT(39),
        .civs_base = 7,
        .mils_base = 2,
        .civ_production = CIV_PRODUCTION_CIV,
        .equipment = 1'000,
    }
);

countries.push_back(
    new Country{
        .name = "Bulgaria",
        .color = COL_INT(118),
        .civs_base = 11,
        .mils_base = 3,
        .civ_production = CIV_PRODUCTION_CIV,
        .equipment = 1'000,
    }
);

countries.push_back(
    new Country{
        .name = "Romania",
        .color = COL_INT(184),
        .civs_base = 11,
        .mils_base = 7,
        .civ_production = CIV_PRODUCTION_CIV,
        .equipment = 1'000,
    }
);

countries.push_back(
    new Country{
        .name = "Yugoslavia",
        .color = COL_INT(27),
        .civs_base = 14,
        .mils_base = 3,
        .civ_production = CIV_PRODUCTION_CIV,
        .equipment = 1'000,
    }
);

countries.push_back(
    new Country{
        .name = "Hungary",
        .color = COL_INT(9),
        .civs_base = 10,
        .mils_base = 6,
        .civ_production = CIV_PRODUCTION_CIV,
        .equipment = 1'000,
    }
);

countries.push_back(
    new Country{
        .name = "Czechoslovakia",
        .color = COL_INT(38),
        .civs_base = 16,
        .mils_base = 9,
        .civ_production = CIV_PRODUCTION_CIV,
        .equipment = 1'000,
    }
);

countries.push_back(
    new Country{
        .name = "Austria",
        .color = COL_INT(145),
        .civs_base = 10,
        .mils_base = 3,
        .civ_production = CIV_PRODUCTION_CIV,
        .equipment = 1'000,
    }
);

// put countries onto map

{
    // russia
    map[MAP_SIZE_Y * 0.2][MAP_SIZE_X * 0.9].owner  = countries[1];
    map[MAP_SIZE_Y * 0.3][MAP_SIZE_X * 0.8].owner  = countries[1];
    map[MAP_SIZE_Y * 0.4][MAP_SIZE_X * 0.85].owner = countries[1];
    map[MAP_SIZE_Y * 0.5][MAP_SIZE_X * 0.9].owner  = countries[1];

    // germany
    map[MAP_SIZE_Y * 0.3][MAP_SIZE_X * 0.55].owner = countries[2];

    // poland
    map[MAP_SIZE_Y * 0.4][MAP_SIZE_X * 0.7].owner  = countries[3];
    map[MAP_SIZE_Y * 0.5][MAP_SIZE_X * 0.75].owner = countries[3];

    // turkey
    map[MAP_SIZE_Y * 0.82][MAP_SIZE_X * 0.82].owner = countries[4];
    map[MAP_SIZE_Y * 0.82][MAP_SIZE_X * 0.83].owner = countries[4];
    map[MAP_SIZE_Y * 0.82][MAP_SIZE_X * 0.84].owner = countries[4];

    // grece
    map[MAP_SIZE_Y * 0.85][MAP_SIZE_X * 0.7].owner = countries[5];

    // bulgaria
    map[MAP_SIZE_Y * 0.77][MAP_SIZE_X * 0.80].owner = countries[6];
    map[MAP_SIZE_Y * 0.77][MAP_SIZE_X * 0.81].owner = countries[6];
    map[MAP_SIZE_Y * 0.77][MAP_SIZE_X * 0.82].owner = countries[6];

    // romania
    map[MAP_SIZE_Y * 0.57][MAP_SIZE_X * 0.82].owner = countries[7];

    // yugoslavia
    map[MAP_SIZE_Y * 0.7][MAP_SIZE_X * 0.65].owner = countries[8];
    map[MAP_SIZE_Y * 0.65][MAP_SIZE_X * 0.6].owner = countries[8];

    // hungary
    map[MAP_SIZE_Y * 0.57][MAP_SIZE_X * 0.7].owner = countries[9];

    // cze
    map[MAP_SIZE_Y * 0.55][MAP_SIZE_X * 0.65].owner = countries[10];

    // austria
    map[MAP_SIZE_Y * 0.5][MAP_SIZE_X * 0.55].owner = countries[11];
}

// player-related

// TODO choose from a menu rather than hardcoding this
Country* player = countries[1];

int gui_additional_turns_to_pass = 0;
