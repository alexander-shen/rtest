# extract.h, extract.c -- library for applying 2-source extractor

# extractor.h extractor.c -- combines two files and outputs the extracted bits

extractor: extractor.c extractor.h extract.c extract.h; \
  gcc -o extractor extractor.c extract.c
  
install: extractor; \
  sudo cp extractor /usr/local/bin/

clean: ; \
  rm *~ *.o extractor  
