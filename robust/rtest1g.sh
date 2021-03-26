#!/bin/sh

# 5
echo >> $1.test1g
echo bytedistribs: >> $1.test1g
echo bytedistribs: 
echo rtest -x -f $1 -e $2 -p 100 -q 100 -n 40000 -t 6 -d 1 -r 1  >> $1.test1g
time rtest -x -f $1 -e $2 -p 100 -q 100 -n 40000 -t 6 -d 1 -r 1 >> $1.test1g

# 6
echo all_bytes: > $1.test1g
echo all_bytes:
echo rtest -x -f $1 -e $2 -p 5000 -q5000 -n 1 -t 0 -d 1 -r 10 >> $1.test1g
time rtest -x -f $1 -e $2 -p 5000 -q5000 -n 1 -t 0 -d 1 -r 10 >> $1.test1g

# 20
echo all_16 >> $1.test1g
echo all_16 
echo rtest -x -f $1 -e $2 -p 300 -q 300 -n 1 -t 1 -d 1 -r 1 >> $1.test1g
time rtest -x -f $1 -e $2 -p 300 -q 300 -n 1 -t 1 -d 1 -r 1 >> $1.test1g

# 15
echo >> $1.test1g
echo knuth_runs: >> $1.test1g
echo knuth_runs: 
echo rtest -x -f $1 -e $2 -p 1000 -q 1000 -n 40000 -t 7 -d 2 -r 1  >> $1.test1g
time rtest -x -f $1 -e $2 -p 1000 -q 1000 -n 40000 -t 7 -d 2 -r 1 >> $1.test1g

# 75
echo >> $1.test1g
echo rank32x32: >> $1.test1g
echo rank32x32:
echo rtest -x -f $1 -e $2 -p 500 -q 500 -n 5000 -t 11 -d 1 -r 1  >> $1.test1g
time rtest -x -f $1 -e $2 -p 500 -q 500 -n 5000 -t 11 -d 1 -r 1 >> $1.test1g

# 78
echo >> $1.test1g
echo ent_8_16: >> $1.test1g
echo ent_8_16: 
echo rtest -x -f $1 -e $2 -p 250 -q 250 -n 400000 -t 9 -d 4 -r 1  >> $1.test1g
time rtest -x -f $1 -e $2 -p 250 -q 250 -n 400000 -t 9 -d 4 -r 1 >> $1.test1g

# 82
echo >> $1.test1g
echo bitstream_n: >> $1.test1g
echo bitstream_n: 
echo rtest -x -f $1 -e $2 -p 80 -q 80 -n 41943040 -t 14 -d 1 -r 1 >> $1.test1g
time rtest -x -f $1 -e $2 -p 80 -q 80 -n 41943040 -t 14 -d 1 -r 1 >> $1.test1g


# 145
echo >> $1.test1g
echo sts_serial: >> $1.test1g
echo sts_serial: 
echo rtest -x -f $1 -e $2 -p 1000 -q 1000 -n 100000  -t 3 -d 57 -r 1 >> $1.test1g
time rtest -x -f $1 -e $2 -p 1000 -q 1000 -n 100000  -t 3 -d 57 -r 1 >> $1.test1g

# 148???
echo >> $1.test1g
echo opso: >> $1.test1g 
echo opso: 
echo rtest -x -f $1 -e $2 -p 50 -q 50 -n 2097153 -t 4 -d 23 -r 1 >> $1.test1g
time rtest -x -f $1 -e $2 -p 50 -q 50 -n 2097153 -t 4 -d 23 -r 1 >> $1.test1g

# 459???
echo >> $1.test1g
echo oqso: >> $1.test1g
echo oqso: 
echo rtest -x -f $1 -e $2 -p 50 -q 50 -n 2097155 -t 5 -d 28 -r 1 >> $1.test1g
time rtest -x -f $1 -e $2 -p 50 -q 50 -n 2097155 -t 5 -d 28 -r 1 >> $1.test1g

# 248
echo >> $1.test1g
echo osums: >> $1.test1g
echo osums: 
echo rtest -x -f $1 -e $2 -p 5000 -q 5000 -n 200 -t 8 -d 1 -r 10  >> $1.test1g
time rtest -x -f $1 -e $2 -p 5000 -q 5000 -n 200 -t 8 -d 1 -r 10 >> $1.test1g

# 333
echo >> $1.test1g
echo rank6x8: >> $1.test1g
echo rank6x8: 
echo rtest -x -f $1 -e $2 -p 1000 -q 1000 -n 10000 -t 12 -d 25 -r 1 >> $1.test1g
time rtest -x -f $1 -e $2 -p 1000 -q 1000 -n 10000 -t 12 -d 25 -r 1 >> $1.test1g

# 438 
echo >> $1.test1g
echo fftest: >> $1.test1g
echo fftest:
echo rtest -x -f $1 -e $2 -p 2000 -q 2000 -n 8192 -t 10 -d 1 -r 1  >> $1.test1g
time rtest -x -f $1 -e $2 -p 2000 -q 2000 -n 8192 -t 10 -d 1 -r 1 >> $1.test1g

# 446
echo >> $1.test1g
echo bitstream_o: >> $1.test1g
echo bitstream_o: 
echo rtest -x -f $1 -e $2 -p 1500 -q 1500 -n 2097171 -t 13 -d 1 -r 1 >> $1.test1g
time rtest -x -f $1 -e $2 -p 1500 -q 1500 -n 2097171 -t 13 -d 1 -r 1 >> $1.test1g

# 453
echo >> $1.test1g 
echo lz_split: >> $1.test1g
echo lz_split: 
echo rtest -x -f $1 -e $2 -p 1000 -q 1000 -n 100000 -t 15 -d 1 -r 1 >> $1.test1g
time rtest -x -f $1 -e $2 -p 1000 -q 1000 -n 100000 -t 15 -d 1 -r 1 >> $1.test1g

# 1442
echo >> $1.test1g
echo birthdays >> $1.test1g
echo birthdays:
echo rtest -x -f $1 -e $2 -p 200 -q 200 -n 500 -t 16 -d 32 -r 1 >> $1.test1g
time rtest -x -f $1 -e $2 -p 200 -q 200 -n 500 -t 16 -d 32 -r 1 >> $1.test1g
