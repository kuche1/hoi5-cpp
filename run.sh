#! /usr/bin/env bash

set -euo pipefail

FLAGS_STRICT='-Werror -Wextra -Wall -pedantic'

clear

gcc $FLAGS_STRICT -o hoi5 hoi5.c

./hoi5
