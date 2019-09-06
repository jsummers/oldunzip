"Old Unzip" - A collection a public domain or MIT licensed single-header-file
C/C++ libraries to help decompress ZIP files.

Currently, there is one library:
* unimplode6a.h: "implode"/"explode" compression (method 6)

For more information, see the individual readme file(s), and the .h file(s).

=== Other files in this project ===

ozdemo.c is a sample program that uses the libraries, and implements a minimal
ZIP parser. It will extract up to 25 supported member files from a ZIP file,
naming the output files "demo.???.out".
