#!/bin/sh

echo all_bytes: > $1.test10m
echo rtest -x -f $1 -e $2 -p 2500 -q2500 -n 1 -t 0 -d 1 -r 1 >> $1.test10m
rtest -x -f $1 -e $2 -p 2500 -q2500 -n 1 -t 0 -d 1 -r 1 >> $1.test10m

# nor really useful with 2+2 experiments
echo all_16 >> $1.test10m
echo rtest -x -f $1 -e $2 -p 2 -q 2 -n 1 -t 1 -d 1 -r 1 >> $1.test10m
rtest -x -f $1 -e $2 -p 2 -q 2 -n 1 -t 1 -d 1 -r 1 >> $1.test10m

echo >> $1.test10m
echo sts_serial: >> $1.test10m
echo rtest -x -f $1 -e $2 -p 200 -q 200 -n 6000  -t 3 -d 45 -r 1 >> $1.test10m
rtest -x -f $1 -e $2 -p 200 -q 200 -n 6000  -t 3 -d 45 -r 1 >> $1.test10m

# opso: not enough values for 10Mbyte [each test value costs 8 Mbytes]
# rtest -x -f $1 -e $2 -p 10 -q 10 -n 2097153 -t 4 -d 23 -r 1 -v >> $1.test10m

# oqso: not enough values for 10Mbyte [each test value costs 8 Mbytes]
#rtest -x -f $1 -e $2 -p 10 -q 10 -n 2097155 -t 5 -d 28 -r 1 -v >> $1.test10m

echo >> $1.test10m
echo bytedistribs: >> $1.test10m
echo rtest -x -f $1 -e $2 -p 100 -q 100 -n 4000 -t 6 -d 1 -r 1  >> $1.test10m
rtest -x -f $1 -e $2 -p 100 -q 100 -n 4000 -t 6 -d 1 -r 1 >> $1.test10m

echo >> $1.test10m
echo knuth_runs: >> $1.test10m
echo rtest -x -f $1 -e $2 -p 50 -q 50 -n 20000 -t 7 -d 2 -r 1  >> $1.test10m
rtest -x -f $1 -e $2 -p 50 -q 50 -n 20000 -t 7 -d 2 -r 1 >> $1.test10m

echo >> $1.test10m
echo osums: >> $1.test10m
echo rtest -x -f $1 -e $2 -p 2500 -q 2500 -n 200 -t 8 -d 1 -r 1  >> $1.test10m
rtest -x -f $1 -e $2 -p 2500 -q 2500 -n 200 -t 8 -d 1 -r 1 >> $1.test10m

echo >> $1.test10m
echo ent_8_16 >> $1.test10m
echo rtest -x -f $1 -e $2 -p 250 -q 250 -n 1000 -t 9 -d 4 -r 1  >> $1.test10m
rtest -x -f $1 -e $2 -p 250 -q 250 -n 1000 -t 9 -d 4 -r 1 >> $1.test10m

echo >> $1.test10m
echo fftest >> $1.test10m
echo rtest -x -f $1 -e $2 -p 200 -q 200 -n 2048 -t 10 -d 1 -r 1  >> $1.test10m
rtest -x -f $1 -e $2 -p 200 -q 200 -n 2048 -t 10 -d 1 -r 1 >> $1.test10m

echo >> $1.test10m
echo rank32x32 >> $1.test10m
echo rtest -x -f $1 -e $2 -p 50 -q 50 -n 600 -t 11 -d 1 -r 1  >> $1.test10m
rtest -x -f $1 -e $2 -p 50 -q 50 -n 600 -t 11 -d 1 -r 1 >> $1.test10m

echo >> $1.test10m
echo rank6x8 >> $1.test10m
echo rtest -x -f $1 -e $2 -p 150 -q 150 -n 1000 -t 12 -d 25 -r 1 >> $1.test10m
rtest -x -f $1 -e $2 -p 150 -q 150 -n 1000 -t 12 -d 25 -r 1 >> $1.test10m

# only -p = -q =1 possible for 1Mbyte, no meaningful p-value
echo >> $1.test10m
echo bitstream_o >> $1.test10m
echo rtest -x -f $1 -e $2 -p 10 -q 10 -n 2097171 -t 13 -d 1 -r 1 >> $1.test10m
rtest -x -f $1 -e $2 -p 10 -q 10 -n 2097171 -t 13 -d 1 -r 1 >> $1.test10m

# no chances for 10Mbyte (only one value)
# echo >> $1.test10m
# echo bitstream_n >> $1.test10m
# rtest -x -f $1 -e $2 -p 1 -q 1 -n 41913040 -t 14 -d 1 -r 1 >> $1.test10m

echo >> $1.test10m
echo lz_split >> $1.test10m
echo rtest -x -f $1 -e $2 -p 32 -q 32 -n 31250 -t 15 -d 1 -r 1 >> $1.test10m
rtest -x -f $1 -e $2 -p 32 -q 32 -n 31250 -t 15 -d 1 -r 1 >> $1.test10m

echo >> $1.test10m
echo birthdays >> $1.test10m
echo rtest -x -f $1 -e $2 -p 10 -q 10 -n 100 -t 16 -d 32 -r 1 >> $1.test10m
rtest -x -f $1 -e $2 -p 10 -q 10 -n 100 -t 16 -d 32 -r 1 >> $1.test10m
