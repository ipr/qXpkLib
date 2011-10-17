
xadISO - ISO 9660 file extract library


Author: Ilkka Prusi
Contact ilkka.prusi@gmail.com


Summary:
ISO-image handling implementation for qXpkLib to load.

Description:
While not actually archive-file, ISO-image (CD-ROM image file)
can be technically considered "store-only" archive of files.

So we can also use it as such without burning to physical disk
assuming we know the data layout in the image.

Yes, there are ways to mount CD-images as drives.
However, this library could be used for different requirements
such as extracting files from "batch" of CD-images or something similar.

Licensing:
GPL v2, see ../COPYING.txt

Status:
Not completed, in progress..

Implementation plan:
* step 1: "base" ISO 9660 - in progress
* step 2: RockRidge/Joliet extensions
* step 3: HFS, UDF etc.

