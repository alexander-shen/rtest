
example: example.cpp spectral_test.cpp specgraph.cpp rand.cpp; \
    g++ -o example example.cpp spectral_test.cpp specgraph.cpp rand.cpp

clean:; \
    rm *~ example a.out
    
install: example; \
    sudo cp example /usr/local/bin/; sudo cp scripts/* /usr/local/bin/
    
uninstall: ; \
    sudo rm /usr/local/bin/example; \
    sudo rm /usr/local/bin/spec-*    
        
