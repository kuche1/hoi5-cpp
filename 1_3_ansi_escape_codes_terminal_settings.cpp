
///////////
//////////////
///////////////// ansi escape codes / terminal settings
//////////////
///////////

// https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797

typedef char* Color;

#define DISP_CLEAR "\033[H\033[J"

#define COL_RESET ((Color)"\033[0m")

#define COL_YELLOW_DARK ((Color)"\033[33m")

#define COL_INT(color_number) ((Color)"\033[38;5;" #color_number "m")

#define EFFECT_INVERSE_ON  "\033[7m"
#define EFFECT_INVERSE_OFF "\033[27m" // untested

// mouse click

void terminal_mouse_click_log_enable() {
    printf("\033[?9h"); // the line on the bottom doesnt work without this one
    printf("\033[?1006h");
}

void terminal_mouse_click_log_disable() {
    printf("\033[?1006l");
    printf("\033[?9l");
}

// echo

void terminal_echo_enable() {
    term((char*)"stty echo");
}

void terminal_echo_disable() {
    term((char*)"stty -echo");
}

// line buffering

void terminal_line_buffering_enable() {
    term((char*)"stty icanon");
}

void terminal_line_buffering_disable() {
    term((char*)"stty -icanon");
}
