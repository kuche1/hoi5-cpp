#! /usr/bin/env bash

set -euo pipefail

FLAGS_STANDARD='-std=c++23'

FLAGS_CRASH_ON_FIRST_ERROR='-Wfatal-errors'

FLAGS_STRICT='-Werror -Wextra -Wall -pedantic'
# sadly, `-Wmissing-field-initializers` doesn't seem to do anything

clear

g++ $FLAGS_STANDARD $FLAGS_CRASH_ON_FIRST_ERROR $FLAGS_STRICT -o hoi5 0_hoi5.cpp

./hoi5
