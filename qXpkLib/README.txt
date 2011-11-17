
XPK/XFD/XAD style decruncher library in Qt&C++.

Author: Ilkka Prusi
Contact ilkka.prusi@gmail.com

Summary:
qXpkLib is reusable XPK/XFD/XAD style decrunching library in Qt&C++.
Additional decruncher plugins are used for extendability.

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

Credits/authors:
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
* combination of file/buffer input/output needs some more work
 - also possible "chaining" of libraries 
* plan includes support for 64-bit builds also
* ...

