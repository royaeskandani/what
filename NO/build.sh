#!/bin/bash

# set -x

gcc -Wall -O0 code/*.c -o project/_project -D Mac
./project/_project

# ./project/_project > dataset.txt
