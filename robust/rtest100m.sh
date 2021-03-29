#!/bin/sh

# 4s
echo all_bytes: > $1.test100m
echo rtest -x -f $1 -e $2 -p 5000 -q5000 -n 1 -t 0 -d 1 -r 5 >> $1.test100m
time rtest -x -f $1 -e $2 -p 5000 -q5000 -n 1 -t 0 -d 1 -r 5 >> $1.test100m

# 2s
echo all_16 >> $1.test100m
echo rtest -x -f $1 -e $2 -p 20 -q 20 -n 1 -t 1 -d 1 -r 1 >> $1.test100m
time rtest -x -f $1 -e $2 -p 20 -q 20 -n 1 -t 1 -d 1 -r 1 >> $1.test100m

# 5s 8s
echo >> $1.test100m
echo sts_serial: >> $1.test100m
echo rtest -x -f $1 -e $2 -p 1000 -q 1000 -n 10000  -t 3 -d 48 -r 1 >> $1.test100m
time rtest -x -f $1 -e $2 -p 1000 -q 1000 -n 10000  -t 3 -d 48 -r 1 >> $1.test100m

# 20s 32s 50s
echo >> $1.test100m
echo opso: >> $1.test100m 
echo rtest -x -f $1 -e $2 -p 6 -q 6 -n 2097153 -t 4 -d 23 -r 1 >> $1.test100m
time rtest -x -f $1 -e $2 -p 6 -q 6 -n 2097153 -t 4 -d 23 -r 1 >> $1.test100m

# 54s 61s 95s
echo >> $1.test100m
echo oqso: >> $1.test100m
echo rtest -x -f $1 -e $2 -p 6 -q 6 -n 2097155 -t 5 -d 28 -r 1 >> $1.test100m
time rtest -x -f $1 -e $2 -p 6 -q 6 -n 2097155 -t 5 -d 28 -r 1 >> $1.test100m

# 3s 5s
echo >> $1.test100m
echo bytedistribs: >> $1.test100m
echo rtest -x -f $1 -e $2 -p 400 -q 400 -n 10000 -t 6 -d 1 -r 1  >> $1.test100m
time rtest -x -f $1 -e $2 -p 400 -q 400 -n 10000 -t 6 -d 1 -r 1 >> $1.test100m

# 3s
echo >> $1.test100m
echo knuth_runs: >> $1.test100m
echo rtest -x -f $1 -e $2 -p 500 -q 500 -n 20000 -t 7 -d 2 -r 1  >> $1.test100m
time rtest -x -f $1 -e $2 -p 500 -q 500 -n 20000 -t 7 -d 2 -r 1 >> $1.test100m

# 43s 70s 72s
echo >> $1.test100m
echo osums: >> $1.test100m
echo rtest -x -f $1 -e $2 -p 2500 -q 2500 -n 200 -t 8 -d 1 -r 10  >> $1.test100m
time rtest -x -f $1 -e $2 -p 2500 -q 2500 -n 200 -t 8 -d 1 -r 10 >> $1.test100m

# 2s
echo >> $1.test100m
echo ent_8_16: >> $1.test100m
echo rtest -x -f $1 -e $2 -p 1000 -q 1000 -n 10000 -t 9 -d 4 -r 1  >> $1.test100m
time rtest -x -f $1 -e $2 -p 1000 -q 1000 -n 10000 -t 9 -d 4 -r 1 >> $1.test100m

# 18s 15s
echo >> $1.test100m
echo fftest: >> $1.test100m
echo rtest -x -f $1 -e $2 -p 2000 -q 2000 -n 8192 -t 10 -d 1 -r 1  >> $1.test100m
time rtest -x -f $1 -e $2 -p 2000 -q 2000 -n 8192 -t 10 -d 1 -r 1 >> $1.test100m

# 4s
echo >> $1.test100m
echo rank32x32: >> $1.test100m
echo rtest -x -f $1 -e $2 -p 200 -q 200 -n 1000 -t 11 -d 1 -r 1  >> $1.test100m
time rtest -x -f $1 -e $2 -p 200 -q 200 -n 1000 -t 11 -d 1 -r 1 >> $1.test100m

# 24s 30s
echo >> $1.test100m
echo rank6x8: >> $1.test100m
echo rtest -x -f $1 -e $2 -p 1000 -q 1000 -n 1500 -t 12 -d 25 -r 1 >> $1.test100m
time rtest -x -f $1 -e $2 -p 1000 -q 1000 -n 1500 -t 12 -d 25 -r 1 >> $1.test100m

# 10s 13s
echo >> $1.test100m
echo bitstream_o: >> $1.test100m
echo rtest -x -f $1 -e $2 -p 150 -q 150 -n 2097171 -t 13 -d 1 -r 1 >> $1.test100m
time rtest -x -f $1 -e $2 -p 150 -q 150 -n 2097171 -t 13 -d 1 -r 1 >> $1.test100m

# 4s 6s
echo >> $1.test100m
echo bitstream_n: >> $1.test100m
echo rtest -x -f $1 -e $2 -p 9 -q 9 -n 41943040 -t 14 -d 1 -r 1 >> $1.test100m
time rtest -x -f $1 -e $2 -p 9 -q 9 -n 41943040 -t 14 -d 1 -r 1 >> $1.test100m

# 11s 14s
echo >> $1.test100m
echo lz_split >> $1.test100m
echo rtest -x -f $1 -e $2 -p 320 -q 320 -n 31250 -t 15 -d 1 -r 1 >> $1.test100m
time rtest -x -f $1 -e $2 -p 320 -q 320 -n 31250 -t 15 -d 1 -r 1 >> $1.test100m

# 117s 124s
echo >> $1.test100m
echo birthdays >> $1.test100m
echo rtest -x -f $1 -e $2 -p 100 -q 100 -n 100 -t 16 -d 32 -r 1 >> $1.test100m
time rtest -x -f $1 -e $2 -p 100 -q 100 -n 100 -t 16 -d 32 -r 1 >> $1.test100m
