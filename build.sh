#!/bin/bash

# set -x


gcc -Wall -O0 code/*.c -o project/_project -D PC

./project/_project
