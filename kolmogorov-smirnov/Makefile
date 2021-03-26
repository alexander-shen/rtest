# ks2mp.c: long integer precise computation of ks-value using gmp library (-l gmp needed)
#           headers in ks2.h
# ks2.c: dynamic programming computation with C-long integers (for small inputs); 
#        given only for comparison with long integer version; headers in ks2.h
# ksmirnov.c: adapted code from existing libraries (headers in ksmirnov.h)
#
# graph_pipe.h, graph_pipe.c simple tools for drawing using GL


# Utilities:

# ks2compare: comparing three methods to compute ks-values: (1) exact computation via ks2mp.c
#             (2) floating point version, and  (3) asymptotic formula (both 2 and 3 taken from ksmirnov.c)

# ks2val: reads the string (with two different characters) from the standard input and computes
#         Kolmogorov-Smirnov p-value using exact computation

# ks2vala: reads the string (with two different characters) from the standard input and computes
#         Kolmogorov-Smirnov p-value using long double computation

# ks2draw: does the same as ks2val but also draws the path from 0,0 to 1,1 corresponding to the string

# ksstring.c: compute ks2 pvalue for a string composed of two letters 
#   (indicating elements from both samples after ordering)
# ks2val.c: compute ks pvalue for a string composed of two letters that is in a file
# ks2vala.c: the same with double real approximate inductive computation
# ks2draw: draws the path and computes KS-value
# histog2.c: draw two distribution graphs for samples in two files (each line starts with a real value)
# k1smirnov.c: Marsaglia-Wang-Tsang code for KS test with one sample
# k1smirnov.h: definitions for it


# ksindep.h, ksindep.c: using Kolmogorov-Smirnov test with two samples for checking independence

# ks2files.py: reads two files with two samples and produces a string for KS-analysis

ks2compare: ks2mp.c ks2.h ks2compare.c ksmirnov.c; \
  gcc -o ks2compare ks2mp.c ks2compare.c ksmirnov.c -lgmp -lm
  
ks2val:  ks2mp.c ks2.h ks2val.c; \
  gcc -o ks2val ks2mp.c ks2val.c -lgmp

ks2vala: ks2.h ks2vala.c ksmirnov.c ksmirnov.h; \
  gcc -o ks2vala ks2vala.c ksmirnov.c -lm  
  
ks2draw:  ks2mp.c ks2.h ks2draw.c graph_pipe.c graph_pipe.h; \
  gcc -o ks2draw ks2mp.c ks2draw.c graph_pipe.c -lgmp -lglut -lGL -lGLU  

ks2indep: ks2indep.c ks2indep.h ksmirnov.c ksmirnov.h; \
  gcc -o ks2indep ks2indep.c ksmirnov.c -lm
    
distribs: graph_pipe.c graph_pipe.h distribs.c; \
    gcc -o distribs distribs.c graph_pipe.c -lgmp -lglut -lGL -lGLU    
    
install: ks2compare ks2val ks2vala ks2draw distribs ks2indep ks2files.py; \
  sudo cp ks2compare ks2val ks2vala ks2draw distribs ks2indep ks2files.py /usr/local/bin/
  
uninstall: ; \
  sudo rm /usr/local/bin/ks2compare /usr/local/bin/ks2val /usr/local/bin/ks2vala /usr/local/bin/ks2draw; \
  sudo rm /usr/local/bin/distribs /usr/local/bin/ks2indep /usr/local/bin/ks2files.py  
  
clean: ;\
     rm *.o *~ ks2compare ks2val ks2vala ks2draw distribs ks2indep
