#!/usr/bin/python3

# reads two files from command line that contain sample data
# produces a string made of characters 1 2 that indicate the 
# sequence of origins of the sorted samples
# each line of a file is a point; it starts with a decimal real (value)
# the rest of the line is used to resolve ties
# if two lines are exactly equal, program fails

import sys
import re
from operator import itemgetter

if len(sys.argv) !=3:
  print("Usage:", sys.argv[0], "sample_file_1 sample_file_2")
  sys.exit(1)
file1 = sys.argv[1]
file2 = sys.argv[2]
# print("Files with samples:", file1, file2)
values=[]  
# list of (sample_value, rest_of_the_line, source)
# source = 1 or 2 depending on the source file
with open (file1) as f1:  # open for reading
  n0= 0
  for line in f1: # reads next line with terminating \n
    m= re.search(r'[-+]?(\d+(\.\d*)?|\.\d+)([eE][-+]?\d+)?', line)
    if m:
      v=float(m.group(0))
      (l,r)=m.span(0)
      values.append((v,line[r:len(line)-1],1)) # last char is newline
      n0= n0+1
    else:
      print ("Warning: line does not start with a number in file", file1)
with open (file2) as f2:  # open for reading
  n1= 0
  for line in f2: # reads next line with terminating \n
    m= re.search(r'[-+]?(\d+(\.\d*)?|\.\d+)([eE][-+]?\d+)?', line)
    if m:
      v=float(m.group(0))
      (l,r)=m.span(0)
      values.append((v,line[r:len(line)-1],2))
      n1= n0+1
    else:
      print ("Warning: line does not start with a number in file",file2)     
# print("Samples of", n0, "and", n1, "elements")
# sort by value, if equal, use key 
sorted= sorted (values,key=itemgetter(0,1)) 
# print(sorted)
last=[False,False]
for i in sorted:
  if i[0]==last[0] and i[1]==last[1]:
    print('')
    print("Error: tie happened")
    sys.exit(1)
  print (i[2],end='')
  last= i  
print('')  
