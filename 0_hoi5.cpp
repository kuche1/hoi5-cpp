
// TODO
//
// the fucking vectors can realloc, so I need to check what happens if I use push_back in a function that got passed a pointer/reference to the vector (nothing bad should happen, but I need to make sure)
//
// make it so that if the defender just sits there there is some kind of bonus, for example: if the defender has more equipment than you they get a deffensive bonus?

// mouse event tracking: https://invisible-island.net/xterm/ctlseqs/ctlseqs.html#h3-Any-event-tracking
// https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797?permalink_comment_id=3878578
// only get mouse clicks with: code 9 (`h` to turn on; `l` to turn off) (printf "\e[?9h"); I notices that this works in bash and not in fish

// would be cool if we could use a framebuffer of some sort, to shooth out the refreshing

#include "1_0_includes_structs_and_defines.cpp"

int main() {

    #include "2_init.cpp"

    #include "3_0_main_loop.cpp"

    return 0;
}
