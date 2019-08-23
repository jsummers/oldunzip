Unimplode6a (unimplode6a.h) is a public domain single-header-file C/C++ library
for decompressing ZIP method 6 ("Implode"/"Explode") compression.

Implode compression was used by PKZIP version 1.0x through 1.1x.

=== The big problem with this library ===

It does not support "Shrink" (method 1) decompression. It is not very useful
without that, because most ZIP files that use Implode also use Shrink, for
files less than or equal to 320 bytes in size.

If this project is named "oldunzip", that's because of the hope that it might
someday be expanded to include similar libraries for the Shrink, and/or Reduce, compression methods.

=== Basic information about the library ===

Unimplode6a was developed by Jason Summers, based on public domain code by Mark
Adler, from Info-ZIP UnZip v5.4. Note that 5.4 was the last version of UnZip in
which the code was public domain.

For more information, refer to the comments in unimplode6a.h.

=== Other files in this project ===

ui6ademo.c is a sample program that uses the library, and implements a minimal
ZIP parser. It will extract up to 25 Implode-compressed member files from a ZIP
file, naming the output files "demo.???.out".

=== Where to get ZIP files that use Implode compression ===

I don't know of a really good resource. You could try some of the DOS CDs at
<http://cd.textfiles.com/directory.html>.

Or you could make them, with PKZIP 1.10 (search for PKZ110.EXE) + DOSBox
<https://www.dosbox.com/>.

=== About the name "Unimplode6a" ===

I don't like the practice of using different names for a compression algorithm
and its corresponding decompression algorithm, so I went with "unimplode"
instead of "explode".

The "6" comes from ZIP compression method 6.

The "a" is an arbitrary suffix to improve the uniqueness of the name, and
prevent it from ending with a digit. Or it could be for Adler, if you want.

=== Technical information ===

This section is a supplement to the information in the comments and the example
code in unimplode6a.h.

The library does not "#include" any header files, so your program will have to
do that, or otherwise define the symbols the library needs. For example, you
must define either off_t (typically by #including <sys/types.h>) or UI6A_OFF_T.

The decompression is streamable, but not suspendable. Your program must be able
to produce or consume an arbitrary number of bytes on demand.

The library does not do any CRC validation of the decompressed data. Your
program can, and probably should, implement such a feature.

There are four variants of Implode compression, and no in-stream way to tell
which one was used. You must provide this information in the ->bit_flags field.
This can be a copy of the "general purpose bit flag" field from the local or
central directory information in the ZIP file. Any bits irrelevant to Implode
compression will be ignored.

The library is intended to be compatible with Imploded files supported by PKZIP
1.10 and later, but not necessarily with all the files supported by PKZIP 1.01
and 1.02. It seems that two of the four Implode variants were changed in PKZIP
1.10.

