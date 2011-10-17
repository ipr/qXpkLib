
xadLha - Reusable library in Qt&C++ for LhA-archive support.

Author: Ilkka Prusi
Contact: ilkka.prusi@gmail.com

Based on Unix LHa 1.14i (lha-1.14i-ac20040929),
autoconfiscated version.

See: http://lha.sourceforge.jp/
and: http://www2m.biglobe.ne.jp/~dolphin/lha/lha-unix.htm
Also: http://homepage1.nifty.com/dangan/en/Content/Program/Java/jLHA/Notes/Notes.html
Original ANSI-C sources also in: http://gnuwin32.sourceforge.net/packages/lha.htm

xadLha is a Qt-DLL based on based on lha-1.14i-ac20040929.
xadLha should be relatively easily reusable on most platforms where Qt can be compiled
(with differences regarding compilers..).

This version uses "xad" interface for supporting use from qXpkLib.

lha-1.14i-ac20040929 copyrights:
 LHarc    for UNIX  V 1.02  Copyright(C) 1989  Y.Tagawa
 LHx      for MSDOS V C2.01 Copyright(C) 1990  H.Yoshizaki
 LHx(arc) for OSK   V 2.01  Modified     1990  Momozou
 LHa      for UNIX  V 1.00  Copyright(C) 1992  Masaru Oki
 LHa      for UNIX  V 1.14  Modified     1995  Nobutaka Watazaki
 LHa      for UNIX  V 1.14i Modified     2000  Tsugio Okamoto
                    Autoconfiscated 2001-2004  Koji Arai
xadLha copyright(c) 2011 Ilkka Prusi

This is free software. 
You can use, distribute and/or modify this software and source code free-of-charge.
No warranties. Copyrights should remain.

How is this different from original implementation?
* First, no need for console input/output (GUI-apps, code libraries..)
* Compiles on newer C++ compilers (see notes in qlhalib.h)
* Hopefully easier to read/follow/debug/extend..?
* Buffering/file-IO changes, should be more efficient now
 - prefer larger chunks instead of single byte read/write
* No conversion of text-file encoding: all files handled as "binary blobs" as-is
 - if there is need to change encoding there are tons of other tools for it already..
 - codec for pathname encoding can be set but that's it

Status:
* Should be working now, no bugs known currently
* Fixed pathname conversion to Unicode/QString
* Added support for Amiga-style file comments (NULL-terminator within filename in archive)
* Suppors LhA and Lzh packing methods: -lh0- .. -lh7-, -lhd-, -lzs-, -lz4-, -lz5-

Future plans?
* Joe Jared extensions? (packing methods)
 ** -lh8- -lh9- -lha- -lhb- -lhc- -lhe-
* LArc extensions? (packing methods)
 ** -lz7- -lz8- -lz2- -lz3-
* CP/M variations? (packing methods)
 ** -pc1- -pm?-
* other features?
* extended-header types?

