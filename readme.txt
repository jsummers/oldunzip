"Old Unzip" - A collection a public domain or MIT licensed single-header-file
C/C++ libraries to help decompress ZIP files.

There are three libraries:
* ozunshrink.h: "shrink" compression (method 1)
* ozunreduce.h: "reduce"/"expand" compression (methods 2-5)
* unimplode6a.h: "implode"/"explode" compression (method 6)

For more information, see the individual readme files, and the .h files.

=== Other files in this project ===

ozdemo.c is a sample program that uses the libraries, and implements a minimal
ZIP parser. It will extract up to 25 supported member files from a ZIP file,
naming the output files "demo.???.out".

testexamplecode.c helps verify that the #ifdefed-out example code in the
libraries works.

ozft.c makes it easier to do fuzz testing with afl-fuzz.
