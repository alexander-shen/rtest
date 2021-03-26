#
# ent16 < filename : find the entropy of 16-bits chunks

# entdep filename1 filename2: get byte pairs from both files; compute the entropy of
#       corresponding distribution of pairs (=16 bit strings) and the entropies of its margin distributions


ent16: ent16.c; \
  gcc -o ent16 ent16.c -lm
  
entdep: entdep.c; \
  gcc -o entdep entdep.c -lm 
  
install: ent16 entdep; \
  sudo cp ent16 /usr/local/bin/; sudo cp entdep /usr/local/bin/
  
clean:; \
  rm *~ ent16 entdep

