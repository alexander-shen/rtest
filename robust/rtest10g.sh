#!/bin/sh

# 5
echo >> $1.test10g
echo bytedistribs: >> $1.test10g
echo bytedistribs: 
echo rtest -x -f $1 -e $2 -p 1000 -q 1000 -n 40000 -t 6 -d 1 -r 1  >> $1.test10g
time rtest -x -f $1 -e $2 -p 1000 -q 1000 -n 40000 -t 6 -d 1 -r 1 >> $1.test10g

# 6
echo all_bytes: > $1.test10g
echo all_bytes:
echo rtest -x -f $1 -e $2 -p 5000 -q5000 -n 1 -t 0 -d 1 -r 100 >> $1.test10g
time rtest -x -f $1 -e $2 -p 5000 -q5000 -n 1 -t 0 -d 1 -r 100 >> $1.test10g

# 20
echo all_16 >> $1.test10g
echo all_16 
echo rtest -x -f $1 -e $2 -p 3000 -q 3000 -n 1 -t 1 -d 1 -r 1 >> $1.test10g
time rtest -x -f $1 -e $2 -p 3000 -q 3000 -n 1 -t 1 -d 1 -r 1 >> $1.test10g

# 15
echo >> $1.test10g
echo knuth_runs: >> $1.test10g
echo knuth_runs: 
echo rtest -x -f $1 -e $2 -p 2000 -q 2000 -n 40000 -t 7 -d 2 -r 5  >> $1.test10g
time rtest -x -f $1 -e $2 -p 2000 -q 2000 -n 40000 -t 7 -d 2 -r 5 >> $1.test10g

# 75
echo >> $1.test10g
echo rank32x32: >> $1.test10g
echo rank32x32:
echo rtest -x -f $1 -e $2 -p 5000 -q 5000 -n 5000 -t 11 -d 1 -r 1  >> $1.test10g
time rtest -x -f $1 -e $2 -p 5000 -q 5000 -n 5000 -t 11 -d 1 -r 1 >> $1.test10g

# 78
echo >> $1.test10g
echo ent_8_16: >> $1.test10g
echo ent_8_16: 
echo rtest -x -f $1 -e $2 -p 500 -q 500 -n 2000000 -t 9 -d 4 -r 1  >> $1.test10g
time rtest -x -f $1 -e $2 -p 500 -q 500 -n 2000000 -t 9 -d 4 -r 1 >> $1.test10g

# 82
echo >> $1.test10g
echo bitstream_n: >> $1.test10g
echo bitstream_n: 
echo rtest -x -f $1 -e $2 -p 800 -q 800 -n 41943040 -t 14 -d 1 -r 1 >> $1.test10g
time rtest -x -f $1 -e $2 -p 800 -q 800 -n 41943040 -t 14 -d 1 -r 1 >> $1.test10g


# 145
echo >> $1.test10g
echo sts_serial: >> $1.test10g
echo sts_serial: 
echo rtest -x -f $1 -e $2 -p 2500 -q 2500 -n 400000  -t 3 -d 60 -r 1 >> $1.test10g
time rtest -x -f $1 -e $2 -p 2500 -q 2500 -n 400000  -t 3 -d 60 -r 1 >> $1.test10g

# 148???
echo >> $1.test10g
echo opso: >> $1.test10g 
echo opso: 
echo rtest -x -f $1 -e $2 -p 500 -q 500 -n 2097153 -t 4 -d 23 -r 1 >> $1.test10g
time rtest -x -f $1 -e $2 -p 500 -q 500 -n 2097153 -t 4 -d 23 -r 1 >> $1.test10g

# 459???
echo >> $1.test10g
echo oqso: >> $1.test10g
echo oqso: 
echo rtest -x -f $1 -e $2 -p 500 -q 500 -n 2097155 -t 5 -d 28 -r 1 >> $1.test10g
time rtest -x -f $1 -e $2 -p 500 -q 500 -n 2097155 -t 5 -d 28 -r 1 >> $1.test10g

# 248
echo >> $1.test10g
echo osums: >> $1.test10g
echo osums: 
echo rtest -x -f $1 -e $2 -p 5000 -q 5000 -n 200 -t 8 -d 1 -r 100  >> $1.test10g
time rtest -x -f $1 -e $2 -p 5000 -q 5000 -n 200 -t 8 -d 1 -r 100 >> $1.test10g

# 333
echo >> $1.test10g
echo rank6x8: >> $1.test10g
echo rank6x8: 
echo rtest -x -f $1 -e $2 -p 2000 -q 2000 -n 40000 -t 12 -d 25 -r 1 >> $1.test10g
time rtest -x -f $1 -e $2 -p 2000 -q 2000 -n 40000 -t 12 -d 25 -r 1 >> $1.test10g

# 438 
echo >> $1.test10g
echo fftest: >> $1.test10g
echo fftest:
echo rtest -x -f $1 -e $2 -p 5000 -q 5000 -n 8192 -t 10 -d 1 -r 2  >> $1.test10g
time rtest -x -f $1 -e $2 -p 5000 -q 5000 -n 8192 -t 10 -d 1 -r 2 >> $1.test10g

# 446
echo >> $1.test10g
echo bitstream_o: >> $1.test10g
echo bitstream_o: 
echo rtest -x -f $1 -e $2 -p 5000 -q 5000 -n 2097171 -t 13 -d 1 -r 1 >> $1.test10g
time rtest -x -f $1 -e $2 -p 5000 -q 5000 -n 2097171 -t 13 -d 1 -r 1 >> $1.test10g

# 453
echo >> $1.test10g 
echo lz_split: >> $1.test10g
echo lz_split: 
echo rtest -x -f $1 -e $2 -p 2000 -q 2000 -n 400000 -t 15 -d 1 -r 1 >> $1.test10g
time rtest -x -f $1 -e $2 -p 2000 -q 2000 -n 400000 -t 15 -d 1 -r 1 >> $1.test10g

# 1442
echo >> $1.test10g
echo birthdays >> $1.test10g
echo birthdays:
echo rtest -x -f $1 -e $2 -p 500 -q 500 -n 500 -t 16 -d 32 -r 1 >> $1.test10g
time rtest -x -f $1 -e $2 -p 500 -q 500 -n 500 -t 16 -d 32 -r 1 >> $1.test10g
