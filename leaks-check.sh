#!/usr/bin/env bash

mkdir -p ./leaks-check/
g++ --std=c++11 -o ./leaks-check/channel_debug channel.h main.cpp -lpthread
valgrind --leak-check=full \
         --show-leak-kinds=all \
         --track-origins=yes \
         --verbose \
         --log-file=./leaks-check/report.log \
         ./leaks-check/channel_debug