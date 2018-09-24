#!/usr/bin/env bash

DATE=`date '+%Y-%m-%d-%H:%M:%S'`

mkdir -p ./leaks-check/
g++ --std=c++11 -O2 -o ./leaks-check/channel_debug channel.h main.cpp -lpthread

valgrind --leak-check=full \
         --show-leak-kinds=all \
         --track-origins=yes \
         --verbose \
         --log-file=./leaks-check/leaks-${DATE}.log \
         ./leaks-check/channel_debug

valgrind --tool=massif --stacks=yes --massif-out-file=./leaks-check/massif.out ./leaks-check/channel_debug
ms_print ./leaks-check/massif.out > "./leaks-check/ram-usage-${DATE}.log"