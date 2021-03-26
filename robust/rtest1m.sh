#!/bin/sh

echo all_bytes: > $1.tst1m
echo rtest -x -f $1 -e $2 -p 250 -q250 -n 1 -t 0 -d 1 -r 1 >> $1.tst1m
rtest -x -f $1 -e $2 -p 250 -q250 -n 1 -t 0 -d 1 -r 1 >> $1.tst1m

# not enough values for 1 Mb, even for one test
# echo all_16
# echo rtest -x -f $1 -e $2 -p 1 -q 1 -n 1 -t 1 -d 1 -r 1 -v >> $1.tst1m
# rtest -x -f $1 -e $2 -p 1 -q 1 -n 1 -t 1 -d 1 -r 1 -v >> $1.tst1m

echo >> $1.tst1m
echo sts_serial: >> $1.tst1m
echo rtest -x -f $1 -e $2 -p 100 -q 100 -n 1200  -t 3 -d 36 -r 1 >> $1.tst1m
rtest -x -f $1 -e $2 -p 100 -q 100 -n 1200  -t 3 -d 36 -r 1 >> $1.tst1m

# opso: not enough values for 1Mbyte [each test value costs 8 Mbytes]
# rtest -x -f $1 -e $2 -p 10 -q 10 -n 2097153 -t 4 -d 23 -r 1 -v >> $1.tst1m

# oqso: not enough values for 1Mbyte [each test value costs 8 Mbytes]
#rtest -x -f $1 -e $2 -p 10 -q 10 -n 2097155 -t 5 -d 28 -r 1 -v >> $1.tst1m

echo >> $1.tst1m
echo bytedistribs: >> $1.tst1m
echo rtest -x -f $1 -e $2 -p 40 -q 40 -n 1024 -t 6 -d 1 -r 1  >> $1.tst1m
rtest -x -f $1 -e $2 -p 40 -q 40 -n 1024 -t 6 -d 1 -r 1 >> $1.tst1m

echo >> $1.tst1m
echo knuth_runs: >> $1.tst1m
echo rtest -x -f $1 -e $2 -p 10 -q 10 -n 10000 -t 7 -d 2 -r 1  >> $1.tst1m
rtest -x -f $1 -e $2 -p 10 -q 10 -n 10000 -t 7 -d 2 -r 1 >> $1.tst1m

echo >> $1.tst1m
echo osums: >> $1.tst1m
echo rtest -x -f $1 -e $2 -p 500 -q 500 -n 100 -t 8 -d 1 -r 1  >> $1.tst1m
rtest -x -f $1 -e $2 -p 500 -q 500 -n 100 -t 8 -d 1 -r 1 >> $1.tst1m

echo >> $1.tst1m
echo ent_8_16 >> $1.tst1m
echo rtest -x -f $1 -e $2 -p 200 -q 200 -n 256 -t 9 -d 4 -r 1  >> $1.tst1m
rtest -x -f $1 -e $2 -p 200 -q 200 -n 256 -t 9 -d 4 -r 1 >> $1.tst1m

echo >> $1.tst1m
echo fftest >> $1.tst1m
echo rtest -x -f $1 -e $2 -p 200 -q 200 -n 256 -t 10 -d 1 -r 1  >> $1.tst1m
rtest -x -f $1 -e $2 -p 200 -q 200 -n 256 -t 10 -d 1 -r 1 >> $1.tst1m

echo >> $1.tst1m
echo rank32x32 >> $1.tst1m
echo rtest -x -f $1 -e $2 -p 16 -q 16 -n 200 -t 11 -d 1 -r 1  >> $1.tst1m
rtest -x -f $1 -e $2 -p 16 -q 16 -n 200 -t 11 -d 1 -r 1 >> $1.tst1m

echo >> $1.tst1m
echo rank6x8 >> $1.tst1m
echo rtest -x -f $1 -e $2 -p 40 -q 40 -n 400 -t 12 -d 25 -r 1 >> $1.tst1m
rtest -x -f $1 -e $2 -p 40 -q 40 -n 400 -t 12 -d 25 -r 1 >> $1.tst1m

# only -p=-q=1 possible for 1Mbyte, no meaningful p-value
# echo >> $1.tst1m
# echo bitstream_o >> $1.tst1m
# rtest -x -f $1 -e $2 -p 1 -q 1 -n 2097171 -t 13 -d 1 -r 1 -v >> $1.tst1m

# no chances for 1Mbyte
# echo >> $1.tst1m
# echo bitstream_o >> $1.tst1m
# rtest -x -f $1 -e $2 -p 1 -q 1 -n 2097171 -t 13 -d 1 -r 1 >> $1.tst1m

echo >> $1.tst1m
echo lz_split >> $1.tst1m
echo rtest -x -f $1 -e $2 -p 60 -q 60 -n 2000 -t 15 -d 1 -r 1 >> $1.tst1m
rtest -x -f $1 -e $2 -p 60 -q 60 -n 2000 -t 15 -d 1 -r 1 >> $1.tst1m

# echo >> $1.tst1m
# echo birthdays >> $1.tst1m
# rtest -x -f $1 -e $2 -p 1 -q 1 -n 100 -t 16 -d 32 -r 1 >> $1.tst1m


