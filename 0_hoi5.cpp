
// TODO
//
// the fucking vectors can realloc, so I need to check what happens if I use push_back in a function that got passed a pointer/reference to the vector (nothing bad should happen, but I need to make sure)
//
// make it so that if the defender just sits there there is some kind of bonus, for example: if the defender has more equipment than you they get a deffensive bonus?
//
// if tile is "border" show army strength, otherwise show factory count; would be super cool if we could do it on country-per-country basis OR reflect the actual offensive/deffensive strength during war
//
// implement visibility, ie only see you country and up to N border of border tiles; also make the rendering and clicking based not on the actual map, but on the tile data in the country struct (this will actually be quite painfull without adding a X and Y properties of the tile struct); also make the rendering the very last thing in the main loop again (if we want to sychronise the clicks with the map rather than the data in the country struct)
//
// improve rendering performance by using buffering of some sort
//
// delete dead countries from countries vec; bring back dead ccountry scan; remove tile checks

// mouse event tracking: https://invisible-island.net/xterm/ctlseqs/ctlseqs.html#h3-Any-event-tracking
// https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797?permalink_comment_id=3878578
// only get mouse clicks with: code 9 (`h` to turn on; `l` to turn off) (printf "\e[?9h"); I notices that this works in bash and not in fish

#include "1_0_includes_structs_and_defines.cpp"

int main() {

    #include "2_init.cpp"

    #include "3_0_main_loop.cpp"

    return 0;
}
