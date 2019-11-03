Ozunreduce / Old ZIP Unreduce (ozunreduce.h) is a public domain or MIT
licensed single-header-file C/C++ library for decompressing ("expanding") ZIP
"Reduce" (methods 2 through 5) compression.

Reduce compression was used by PKZIP versions 0.xx (0.80, 0.90, 0.92).

Website: <https://entropymine.com/oldunzip/>
Development: <https://github.com/jsummers/oldunzip>

=== Basic information about the library ===

Ozunreduce was developed by Jason Summers, based on public documentation of
the format.

For more information, refer to the comments in ozunreduce.h, and the technical
notes below.

=== Where to get ZIP files that use Reduce compression ===

There might be some listed at the Oldunzip/Ozunreduce website. They're not
common, but they do exist in the wild. They might be more common in ZIP files
that are inside of other ZIP files, on the theory that such internal files were
less likely to be updated by the maintainers of software collections.

To make them yourself, one way is to find a copy of PKZIP 0.92 (search for
PKZ092.EXE), and run it in DOSBox <https://www.dosbox.com/>.

Use both the -ea4 and -eb4 options (replace "4" with "1", "2", or "3" if
desired). Files apparently have to be at least 2048 bytes in size in order for
PKZIP to choose the Reduce method.

=== Technical notes ===

This section is a supplement to the information in the comments and the example
code in ozunreduce.h.

The library does not "#include" any header files, so your program will have to
do that, or otherwise define the symbols the library needs. Mainly, you must
define either off_t (typically by #including <sys/types.h>) or OZUR_OFF_T.

A Reduce decompressor only needs a fixed amount of memory, so to keep the
library simple and portable, it does not do any malloc-style memory allocation
of its own. Your program is responsible for allocating the one block of memory
that it needs, and initializing it to all zero bytes.

There are four variants ("compression factors") of Reduce compression, and no
in-stream way to tell which one was used. You must provide this information in
the ->cmpr_factor field. In a ZIP file, the compression factor (1-4) is one
less than the ZIP compression method (2-5).

The decompression is streamable, but not suspendable. Your program must be able
to produce or consume an arbitrary number of bytes on demand.

The library does not do any CRC validation of the decompressed data. Your
program can, and probably should, implement such a feature.
