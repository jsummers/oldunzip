Unimplode6a (unimplode6a.h) is a public domain or MIT licensed single-header-
file C/C++ library for decompressing ZIP method 6 ("Implode"/"Explode")
compression.

Implode compression was used by PKZIP version 1.0x through 1.1x.

Website: <https://entropymine.com/oldunzip/>
Development: <https://github.com/jsummers/oldunzip>

=== The big problem with this library ===

... is that it does not support "Shrink" (method 1) decompression. It is not
very useful without that, because most ZIP files that use Implode also use
Shrink, for files less than or equal to 320 bytes in size.

=== Basic information about the library ===

Unimplode6a was developed by Jason Summers, based on public domain code by Mark
Adler, from Info-ZIP UnZip v5.4. Note that 5.4 was the last version of UnZip in
which the code was public domain.

For more information, refer to the comments in unimplode6a.h, and the technical
notes below.

=== Where to get ZIP files that use Implode compression ===

Implode was used by most ZIP files created in 1992 or earlier. I don't know of
a really good resource. You could try some of the DOS CDs at
<http://cd.textfiles.com/directory.html>.

Or you could make them, with PKZIP 1.10 (search for PKZ110.EXE) + DOSBox
<https://www.dosbox.com/>. Suggest using the -ei option documented in the
ADDENDUM.DOC file.

=== About the name "Unimplode6a" ===

I don't like the practice of using different names for a compression algorithm
and its corresponding decompression algorithm, so I went with "unimplode"
instead of "explode".

The "6" comes from ZIP compression method 6.

The "a" is an arbitrary suffix to improve the uniqueness of the name, and
prevent it from ending with a digit. Or it could be for Adler, if you want.

=== Technical notes ===

This section is a supplement to the information in the comments and the example
code in unimplode6a.h.

The library does not "#include" any header files, so your program will have to
do that, or otherwise define the symbols the library needs. For example, you
must define either off_t (typically by #including <sys/types.h>) or UI6A_OFF_T.

There are four variants of Implode compression, and no in-stream way to tell
which one was used. You must provide this information in the ->bit_flags field.
This can be a copy of the "general purpose bit flag" field from the local or
central directory information in the ZIP file. Any bits irrelevant to Implode
compression will be ignored.

The decompression is streamable, but not suspendable. Your program must be able
to produce or consume an arbitrary number of bytes on demand.

The library does not do any CRC validation of the decompressed data. Your
program can, and probably should, implement such a feature.

The library is intended to be compatible with Imploded files that can be
decompressed by PKZIP 1.10 and later. See the comment in the example code for
a note about compatibility with PKZIP 1.01 and 1.02.
