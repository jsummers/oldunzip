
default: ozdemo
all: ozdemo _ozdemocpp testexamplecode ozft
.PHONY: default all clean

CFLAGS ?= -Wall -O2
CXXFLAGS ?= -Wall -O2
AFLGCC ?= afl-gcc
HDRS:= ../unimplode6a.h ../ozunreduce.h ../ozunshrink.h

ozdemo: ozdemo.c $(HDRS) ozcrc32.h
	$(CC) $(CFLAGS) -I.. -o $@ $<

# Test that it can be compiled as C++.
_ozdemocpp.cpp: ozdemo.c
	cp $< $@

_ozdemocpp: _ozdemocpp.cpp $(HDRS) ozcrc32.h
	$(CXX) $(CXXFLAGS) -I.. -o $@ $<

# Test that the example code in the libraries compiles.
testexamplecode: testexamplecode.c $(HDRS)
	$(CC) $(CFLAGS) -I.. -o $@ $<

# A utility to help with fuzz testing
ozft: ozft.c $(HDRS)
	$(CC) $(CFLAGS) -I.. -o $@ $<

ozft-afl: ozft.c $(HDRS)
	$(AFLGCC) $(CFLAGS) -I.. -o $@ $<

clean:
	rm -f ozdemo demo.*.out \
 _ozdemocpp _ozdemocpp.cpp \
 testexamplecode example-i.out example-r.out example-s.out \
 ozft ozft-afl
