#!/bin/bash

git submodule update --init
cd lib/libopencm3
make
