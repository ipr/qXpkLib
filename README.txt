
XPK/XFD/XAD style decruncher library in Qt&C++.

Author: Ilkka Prusi
Contact ilkka.prusi@gmail.com

Summary:
qXpkLib is reusable XPK/XFD/XAD style decrunching library in Qt&C++.
Additional decruncher plugins are used for extendability.

See further details in qXpkLib/README.txt

Credits/authors:
* Amiga XPK-libraries written by Dirk Stöcker <stoecker@amigaworld.com> 
 and others, see: http://www.dstoecker.eu/

* various authors per extension-library, see respective library details

* UNIX-version made by Vesa Halttunen <vesuri@jormas.com>, 
 see: http://www.jormas.com/~vesuri/xpk/

* Qt&C++ version made by Ilkka Prusi <ilkka.prusi@gmail.com>.

Licensing:
Generally GPL v2, see COPYING.txt
Note extension-libraries may have different requirements, 
see each respective library README.txt for details.

qXpkLib status, see respective README for details:
* XPK file format chunks read (seems to be correct..)
* initial support for loading libraries
 - may need to change interface from C++ style to C-style for correct/simpler loading..
 - at least Windows DLLs are a pain to deal with when exporting C++ classes from library
 - problems occur when trying dynamic linking without knowing the sub-library at build-time of master-library 
  (which is needed for extensibility)
 - name decorations/symbol export of C++ code from Windows DLL are the pain:
  this does not become issue if we can link DLL-stub to master-library instead of loading it entirely dynamically..
* combination of file/buffer input/output needs some more work
 - also possible "chaining" of libraries
* plan includes support for 64-bit builds also
* ...

qXpkTool status:
* mostly done
* just simple GUI to use, test and demonstrate integrating with the library
* some improvements and features planned, maybe later..

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

