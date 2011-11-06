
xadADF - Amiga ADF disk-image unpacking


Author: Ilkka Prusi
Contact ilkka.prusi@gmail.com


Summary:
ADF floppy image handling implementation for qXpkLib to load.

Description:
While not actually archive-file, ADF-image (Amiga floppy disk image)
can be technically considered "store-only" archive of files.

So we can also use it as such without writing to physical disk
assuming we know the data layout in the image.


Licensing:
GPL v2, see ../COPYING.txt

Status:
Not completed, in progress..

Implementation plan:
* step 1: "base" ADF - in progress (OFS)
* step 2: hard-disk image support (OFS, FFS)
* step 3: other filesystems? (SFS, PFS..)

