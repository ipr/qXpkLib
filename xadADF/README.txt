
xadADF - Amiga ADF disk-image unpacking

Based on afdlib 0.7.11, multiple authors, 
see: http://lclevy.free.fr/adflib/ and AUTHORS.txt
Modified code for use in C++ library by: Ilkka Prusi <ilkka.prusi@gmail.com>

Licensing: GPL v2, see COPYING.txt

Summary:
ADF floppy image handling implementation for qXpkLib to load.

Description:
While not actually archive-file, ADF-image (Amiga floppy disk image)
can be technically considered "store-only" archive of files.

So we can also use it as such without writing to physical disk
assuming we know the data layout in the image.

Code from afdlib has been used as base for this library
with following modifications (not limited to these):
* no console output/input in library
* no file-IO (uses parent-library for that)
* also no trackdisk-handling (use separate tool for that..)
 - trackdisk handling is highly specialized code for each platform
 - each OS and hardware may or may not support necessary stuff
 - use platform-related specialized tools for actual trackdisk-IO if necessary
* etc.

Status:
Not completed, in progress..

Implementation plan:
* step 1: "base" ADF - in progress (OFS)
* step 2: hard-disk image support (OFS, FFS)
* step 3: other filesystems? (SFS, PFS..)

