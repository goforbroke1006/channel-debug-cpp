#!/usr/bin/env bash

mkdir -p ./ams-analyse/
cd ./ams-analyse/
g++ --std=c++11 -O2 -S ../channel.h ../main.cpp -lpthread
