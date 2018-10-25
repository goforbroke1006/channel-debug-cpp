#!/usr/bin/env bash

mkdir -p ./asm-analyse/
cd ./asm-analyse/
g++ --std=c++11 -O2 -S ../channel.h ../main.cpp -lpthread
