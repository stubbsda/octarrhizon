CXX_FLAGS += -I$(BIBLIOTHEK)/include -L$(BIBLIOTHEK)/lib

LIBS       = -lsynarmosma -lpugixml -lm

#CXX_FLAGS += $(CXX_OPT)
CXX_FLAGS += $(DEBUG)

CXX_FLAGS += -Wl,-rpath $(BIBLIOTHEK)/lib

octarrhizon : octarrhizon.cxx
	$(CXX) $(CXX_FLAGS) -o octarrhizon octarrhizon.cxx $(LIBS)

clean:
	rm -f *~
	rm -f octarrhizon











