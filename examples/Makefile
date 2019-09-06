
default: ozdemo
all: ozdemo _ozdemocpp testexamplecode
.PHONY: default all clean

CFLAGS ?= -Wall
CXXFLAGS ?= -Wall

ozdemo: ozdemo.c ../unimplode6a.h
	$(CC) $(CFLAGS) -I.. -o $@ $<

# Test that it can be compiled as C++.
_ozdemocpp.cpp: ozdemo.c
	cp $< $@

_ozdemocpp: _ozdemocpp.cpp ../unimplode6a.h
	$(CXX) $(CXXFLAGS) -I.. -o $@ $<

# Test that the example code in unimplode6a.h compiles.
testexamplecode: testexamplecode.c ../unimplode6a.h
	$(CC) $(CFLAGS) -I.. -o $@ $<

clean:
	rm -f ozdemo demo.*.out \
 _ozdemocpp _ozdemocpp.cpp \
 testexamplecode example.out
