
default: ui6ademo
all: ui6ademo _ui6ademocpp testexamplecode
.PHONY: default all clean

CFLAGS ?= -Wall
CXXFLAGS ?= -Wall

ui6ademo: ui6ademo.c ../unimplode6a.h
	$(CC) $(CFLAGS) -I.. -o $@ $<

# Test that it can be compiled as C++.
_ui6ademocpp.cpp: ui6ademo.c
	cp $< $@

_ui6ademocpp: _ui6ademocpp.cpp ../unimplode6a.h
	$(CXX) $(CXXFLAGS) -I.. -o $@ $<

# Test that the example code in unimplode6a.h compiles.
testexamplecode: testexamplecode.c ../unimplode6a.h
	$(CC) $(CFLAGS) -I.. -o $@ $<

clean:
	rm -f ui6ademo demo.*.out \
 _ui6ademocpp _ui6ademocpp.cpp \
 testexamplecode example.out
