#!/usr/bin/bash

make clean
make
make copy
cd ../sat_solver
make clean
make
./sat -c test/c1355.cnf
sleep 5m