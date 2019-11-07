Ozunshrink / Old ZIP Unshrink (ozunshrink.h) is a public domain or MIT
licensed single-header-file C/C++ library for decompressing ZIP "Shrink"
(method 1) compression.

Shrink compression was used by PKZIP versions 0.xx through 1.1x.

Website: <https://entropymine.com/oldunzip/>
Development: <https://github.com/jsummers/oldunzip>

=== Basic information about the library ===

Ozunshrink was developed by Jason Summers, based on public documentation of the
format, and parts of an old GIF decoder by the same author. The GIF decoder was
developed with the help of a forgotten GIF/LZW decompression tutorial.

For more information, refer to the comments in ozunshrink.h, and the technical
notes below.

=== Where to get ZIP files that use Shrink compression ===

Shrink compression is common in ZIP files created before 1993, though in most
cases it was only used for compressible files at most 320 bytes in size (the
manual says "smaller than 320 bytes", but that's off by 1).

To make Shrink-compressed files yourself, one way is to find a copy of PKZIP
1.10 for DOS (search for PKZ110.EXE), and run it in DOSBox
<https://www.dosbox.com/>. Run PKZIP.EXE with the -es option.

=== Technical notes ===

This section is a supplement to the information in the comments and the example
code in ozunshrink.h.

The library does not "#include" any header files, so your program will have to
do that, or otherwise define the symbols the library needs. Typically, that
means you need:

 #include <sys/types.h>
 #include <string.h>
 #include <stdint.h>

A Shrink decompressor only needs a fixed amount of memory, so to keep the
library simple and portable, it does not do any malloc-style memory allocation
of its own. Your program is responsible for allocating the one block of memory
that it needs, and initializing it to all zero bytes.

The decompression is streamable, but not suspendable. Your program must be able
to produce or consume an arbitrary number of bytes on demand.

The library does not do any CRC validation of the decompressed data. Your
program can, and probably should, implement such a feature.

At a minimum, this library is intended to support any Shrink-compressed file
that can be created by PKZIP 0.90 through 1.10 for DOS, assuming it can be
successfully decompressed by PK(UN)ZIP 1.10. Ideally it will support all valid
Shrink-compressed files, but it's not clear that the specification is detailed
enough to precisely define what is and is not a valid file.
