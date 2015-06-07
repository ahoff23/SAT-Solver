#!/usr/bin/bash

make clean
make
make copy
cd ../sat_solver
make clean
make
./sat -c test/c6288.cnf
sleep 5m