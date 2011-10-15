
XPK/XFD decruncher library in Qt&C++.


Author: Ilkka Prusi
Contact ilkka.prusi@gmail.com


Summary:
Reusable XPK/XFD decrunching library in Qt&C++.
Additional decruncher plugins planned (some technical difficulties currently).
More of a rewrite and redesign than just a port from Amiga/Unix versions.


Description:
Library to support XPK and XFD decrunching (various algorithms).

XPK-library was originally created on Amiga for transparent decrunching support
of various packers and files (such as sound modules, images, text and executables).

XFD-library was made for same goals but slightly different: where XPK mostly used
it's own chunk-based fileformat layout (independently of crunchers) XFD is more of
support for other crunchers with their own fileformats (or so I gather..).

Support on other platforms for either of these is rather limited (read: non-existant)
and this is attempt to make (extensible) decrunching possible on other platforms also.
Plans include support for crunchers native to other platforms such as Unix-compress, GZIP,
Bzip2 and so on..

Credits:
Original XPK-libraries were made by Dirk Stöcker and others, see: http://www.dstoecker.eu/
The Unix-version made by Vesa Halttunen, see: http://www.jormas.com/~vesuri/xpk/
This version made by Ilkka Prusi (ilkka.prusi@gmail.com).

Licensing:
GPL v2, see COPYING.txt

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
* many placeholders to be written for new library handling (much needed changes..)
* ...

