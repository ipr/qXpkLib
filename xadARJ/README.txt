
xadARJ - ARJ unpack library

Based on Amiga OS PowerUP version (PPCUnARJ241) by: Andreas R. Kleinert,
authors: Robert K. Jung (author), Mark Adler, Andreas R. Kleinert (porters)

This C++ library version by: Ilkka Prusi <ilkka.prusi@gmail.com>

Reuses qLhaLib and xadLHA classes, based on Unix LHa: lha-1.14i-ac20040929
Copyrights:
 LHarc    for UNIX  V 1.02  Copyright(C) 1989  Y.Tagawa
 LHx      for MSDOS V C2.01 Copyright(C) 1990  H.Yoshizaki
 LHx(arc) for OSK   V 2.01  Modified     1990  Momozou
 LHa      for UNIX  V 1.00  Copyright(C) 1992  Masaru Oki
 LHa      for UNIX  V 1.14  Modified     1995  Nobutaka Watazaki
 LHa      for UNIX  V 1.14i Modified     2000  Tsugio Okamoto
                    Autoconfiscated 2001-2004  Koji Arai
qLhaLib and xadLha copyright(c) 2011 Ilkka Prusi

Summary:
UnARJ implementation for qXpkLib to load.

Description:
This version is modified to be used as extension-library with qXpkLib
for unpacking ARJ-archives.

Also reuses plenty of code already made in other extension-libraries, including:
* crc (from xadACE) 
* "generic time" timestamp conversion (xadZOO, xadLha)
* file-handling and buffer-handling (qXpkLib)
* Huffman-tree, BitIO is exactly same in LHa, reuse (xadLha, qLhaLib)
* etc.

ARJ is very much like Lha (both created based on Haruhiko Okumura' AR)
so more code reuse seems possible, only fileformat and metadata seems different between archivers.
So rewriting fileformat-handling to this library 
and reusing rest of code from other libraries (xadLHA) might be best plan to implement this.

Modifications include:
* better portability
 - proper ISO-standard typedefs
* some 64-bit build support
* console input/output removed
 - no use in library code
* C to C++ modifications..

Status:
In progress..

