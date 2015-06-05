#!/bin/bash

clear
make clean
make
make copy
cd ../sat_solver
make runtests
