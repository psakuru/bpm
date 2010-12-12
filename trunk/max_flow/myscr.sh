#!/bin/bash

./randomplay $1 $2 
#./ek 500 <eg_${1}_${2}.dat
./bk 5000 <eg_${1}_${2}.dat
./pr 5000 <eg_${1}_${2}.dat
rm eg_${1}_${2}.dat

./randomplay $1 $2 
#./ek 500 <eg_${1}_${2}.dat
./bk 5000 <eg_${1}_${2}.dat
./pr 5000 <eg_${1}_${2}.dat
rm eg_${1}_${2}.dat

./randomplay $1 $2 
#./ek 500 <eg_${1}_${2}.dat
./bk 5000 <eg_${1}_${2}.dat
./pr 5000 <eg_${1}_${2}.dat
rm eg_${1}_${2}.dat

./randomplay $1 $2 
#./ek 500 <eg_${1}_${2}.dat
./bk 5000 <eg_${1}_${2}.dat
./pr 5000 <eg_${1}_${2}.dat
rm eg_${1}_${2}.dat

./randomplay $1 $2 
#./ek 500 <eg_${1}_${2}.dat
./bk 5000 <eg_${1}_${2}.dat
./pr 5000 <eg_${1}_${2}.dat
rm eg_${1}_${2}.dat


