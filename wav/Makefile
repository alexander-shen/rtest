# readwav -m/-s/-y filename.wav : shows pairs taken from wav file
# (-m mono, -s stereo, -y stereo with Yamaha MW10 right channel lag correction)

# getdatawav [-s -y -m] source.wav dest1 [dest2] : extracts raw data from source.wav
# and produces one (for mono) or two (for stereo) files dest1 and dest 2 
# with no changes in the byte sequence (LSB..MSB is the same as in the wav file)
# For [-y] the 1-sample lag in the right channel that happens in Yamaha MW10 
# is taken into account and corrected

# drawpair file1 file2 takes raw data from file1 and file2 and shows 
# pairs of readings as points in 2D


readwav: readwav.c wave.h graph_pipe.c graph_pipe.h; \
   gcc -fcommon -o readwav graph_pipe.c readwav.c -lglut -lGL -lGLU

getdatawav: getdatawav.c wave.h; \
  gcc -fcommon -o getdatawav getdatawav.c
  
drawpair: drawpair.c graph_pipe.c graph_pipe.h; \
  gcc -fcommon -o drawpair drawpair.c graph_pipe.c -lglut -lGL -lGLU  

clean: ; \
   rm *~ *.o; rm readwav getdatawav drawpair

install: readwav getdatawav drawpair; \
   sudo cp readwav /usr/local/bin/; sudo cp getdatawav /usr/local/bin/; sudo cp drawpair /usr/local/bin/
