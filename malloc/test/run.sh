#!/bin/bash

. ../test/lib.sh

make || exit 1

LD_PRELOAD=./libmalloc.so ls
