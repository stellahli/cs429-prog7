#!/usr/bin/env bash

rm -rf build
cmake -S . -B build
make -C build

gcc -g main.c libtdmm/tdmm.c -I./libtdmm -o run_metrics_tests

gcc -g tester.c libtdmm/tdmm.c -I./libtdmm -o run_tests