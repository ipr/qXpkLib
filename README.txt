
XPK/XFD/XAD style decruncher library in Qt&C++.


Author: Ilkka Prusi
Contact ilkka.prusi@gmail.com


Summary:
Reusable XPK/XFD/XAD style decrunching library in Qt&C++.
Additional decruncher plugins planned for extendability (some technical difficulties currently).

This is more of a rewrite and redesign than just a port from Amiga/Unix versions,
large parts of previous code uses old-style C or M68k assembler or the implementation 
is just unnecessarily complex for half-way modern systems.

This implementation is much more rewrite from scratch (at least on master-libraries)
and code reuse is mainly in decrunching/unpacking extension-libraries.

Credits and thanks for original effort and design belong to: Dirk Stöcker <stoecker@amigaworld.com>

Description:
Library to support XPK and XFD decrunching (various algorithms).
Also archive-file (multiple-entry files) unpacking with XAD-style libraries (various fileformats).

XPK-library was originally created on Amiga for transparent decrunching support
of various packers and files (such as sound modules, images, text and executables).

XFD-library was made for same goals but slightly different: where XPK mostly used
it's own chunk-based fileformat layout (independently of crunchers) XFD is more of
support for other crunchers with their own fileformats (or so I gather..).

XAD-library was created on Amiga to handle unpacking archive-files of various types
with same library/interface to all programs. Different libraries were possible
for practically any type of fileformats.

Support on other platforms for any of these is rather limited (read: non-existant)
and this is attempt to make (extensible) decrunching possible on other platforms also.
Plans include support for crunchers native to other platforms such as Unix-compress, GZIP,
Bzip2 and so on..

Credits:
* Amiga XPK-libraries written by Dirk Stöcker <stoecker@amigaworld.com> 
 and others, see: http://www.dstoecker.eu/

* various authors per extension-library, see respective library details

* UNIX-version made by Vesa Halttunen <vesuri@jormas.com>, 
 see: http://www.jormas.com/~vesuri/xpk/

* Qt&C++ version made by Ilkka Prusi <ilkka.prusi@gmail.com>.

Licensing:
GPL v2, see COPYING.txt

Implementation, design:
* qXpkLib implements master-library levels 2 and 1 of original XPK design
 - includes XPK, XFD and XAD master libraries
 - most of file-IO, type detection and sub-library loading is in master-library
* extension sub-libraries implement only level 0 of original design
 - decrunch/decompress/extraction only in level 0
* sub-libraries depend at compile-time on code and definitions in master-library 
 - these may be considered "derived" from master in places (shared code)
* master library does not depend on sub-libraries at compile-time
 - sub-libraries can be replaced with others by same name implementing same base-interface defined in master-library
 - only naming and interface are defined in master-library, actual decompression only in sub-libraries

Current status:
* XPK file format chunks read (seems to be correct..)
* initial support for loading libraries
 - may need to change interface from C++ style to C-style for correct/simpler loading..
 - at least Windows DLLs are a pain to deal with when exporting C++ classes from library
 - problems occur when trying dynamic linking without knowing the sub-library at build-time of master-library 
  (which is needed for extensibility)
 - name decorations/symbol export of C++ code from Windows DLL are the pain:
  this does not become issue if we can link DLL-stub to master-library instead of loading it entirely dynamically..
* some "foreign" external decrunchers are prepared (PowerPacker, Imploder), not yet tested with new master-library
* the "native" XPK crunchers need rewrite to work with new master-library
* some initial XFD "alien" format decrunching (needs conversions from M68k-asm to C/C++ for portability..)
* some support for loading archive-unpackers in XAD-style
* many placeholders to be written for new library handling (much needed changes..)
* combination of file/buffer input/output needs some more work
 - also possible "chaining" of libraries (such as GZIP -> Tar -> actual file..)
* plan includes support for 64-bit builds eventually..
* ...

Sub-library status, see respective README for details:
* LhA/Lzh (xadLha): 
 - various platforms, multi-file archives
 - mostly working, not tested with new interface (was separate as qLhaLib with specific interface)
 - interface may need some fixes as continuing development
 - extracting files only supported (for now)
* LZX (xadLZX) :
 - originally on Amiga only, multi-file archives
 - mostly working, not tested with new interface (was separate as qLZXLib with specific interface)
 - interface may need some fixes as continuing development
 - extracting files only supported (for now)
* ZOO (xadZOO) :
 - originally? unknown, various platforms have used it.. multi-file archives
 - in progress of updating and modifying code for better platform-indenpendency
 - also fitting code to work in extension library (without console)
 - extracting files only supported (for now)
* ACE (xadACE) :
 - multi-file archives
 - in progress of modifying code fit in extension library (without console)
 - extracting files only supported (for now)
* ISO-9660 (xadISO) :
 - multi-file CD/DVD images
 - not actually archive but can be considered collection of files anyway..
 - extracting files only planned
 - in progress of coding..
* BZip2 (xadBzip2) :
 - in progress.. changes to fit in extension library (without console etc.)
* Imploder (xfdImploder) :
 - originally on Amiga
 - single compressed file only
 - should be working, not tested
 - uncompressing/unpacking only supported (for now)
* PowerPacker (xfdPowerPacker) :
 - originally on Amiga
 - single compressed file only
 - should be working, not tested
 - uncompressing/unpacking only supported (for now)
* Z (xfdZCompress) :
 - originally on Un*x variants
 - single compressed file only
 - should be working, not tested
 - uncompressing/unpacking only supported (for now)
* SZDD (xfdSZDD) :
 - originally on Windows
 - single compressed file only
 - should be working, not tested
 - uncompressing/unpacking only supported (for now)
* FAST (xpkFAST) :
 - originally on Amiga
 - single compressed file only
 - in progress, converting M68k-ASM to C/C++ ..
* NONE (xpkNONE) :
 - originally on Amiga
 - no compression (input same as output)
 - for testing use only
* NUKE (xpkNUKE) :
 - not done, placeholder for now
* DUKE (xpkDUKE) :
 - not done, placeholder for now
* RLEN (xpkRLEN) :
 - originally on Amiga
 - simple runlength-compression of files
 - uncompressing/unpacking only supported (for now)
 - should be working, not tested
* SQSH (xpkSQSH) :
 - originally on Amiga
 - single compressed file only
 - uncompressing/unpacking only supported (for now)
 - not tested, should be working..?
* ZAP (xadZAP) :
 - originally on Amiga
 - trackdisk-cruncher (disk image)
 - placeholder only yet..
* DMS (xadDMS) :
 - originally on Amiga
 - trackdisk-cruncher (disk image)
 - placeholder only yet..
* Tar (xadTar) :
 - originally on Un*x variants, standardized later
 - multi-file collection only, separate compression
 - placeholder only yet..
* RPM (xadRPM) :
 - originally on Linux by RedHat
 - multi-file archive
 - placeholder only yet..

* various other libraries are planned, some have placeholders already
* there are several problems in implementing support for various formats
 - many formats have only very little documentation or none at all
 - some formats have all sources or only some code fragments (in M68k assembler usually)
 - some formats exist only as executable binaries (Amiga OS usually)
* there more than 20 packers for XPK alone
* there are literally much over 40 crunchers for XFD (including variants)
* many trackdisk-crunchers (disk image)
* and many archive formats on various platforms (Amiga, Atari, Un*x variants, Mac, Windows etc.)

