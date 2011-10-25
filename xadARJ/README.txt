
xadARJ - ARJ unpack library

Based on Amiga OS PowerUP version (PPCUnARJ241) by: Andreas R. Kleinert.
Authors: Robert K. Jung (author), Mark Adler, Andreas R. Kleinert (porters)

This C++ library version by: Ilkka Prusi <ilkka.prusi@gmail.com>

Summary:
UnARJ implementation for qXpkLib to load.

Description:
This version is modified to be used as extension-library with qXpkLib
for unpacking ARJ-archives.

Also reuses plenty of code already made in other extension-libraries, including:
* crc (from xadACE) 
* "generic time" timestamp conversion (xadZOO, xadLha)
* file-handling and buffer-handling (qXpkLib)
* etc.

Modifications include:
* better portability
 - proper ISO-standard typedefs
* some 64-bit build support
* console input/output removed
 - no use in library code
* C to C++ modifications..

Status:
In progress..

