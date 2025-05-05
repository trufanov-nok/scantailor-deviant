#!/bin/sh -ef

cd "$(dirname "$0")"

shellcheck ./build.sh
shellcheck ./test.sh
shellcheck ./package.sh
