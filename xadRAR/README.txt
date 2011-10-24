
xadRAR - RAR unpack library

Based on Portable UnRAR version (AmigaOS4 port).
Authors: Alexander Roshal, Nicolas Mendoza, Joachim Birging, Stephan Matzke, Joerg Strohmayer

This C++ library version by: Ilkka Prusi <ilkka.prusi@gmail.com>

Summary:
UnRAR implementation for qXpkLib to load.

Description:
This version is modified to be used as extension-library with qXpkLib
for unpacking RAR-archives.

Modifications include:
* better portability
 - proper ISO-standard typedefs
* some 64-bit build support
* console input/output removed
 - no use in library code

Legal stuff (from original readme):

   Unrar source may be used in any software to handle RAR archives
   without limitations free of charge, but cannot be used to re-create
   the RAR compression algorithm, which is proprietary. Distribution
   of modified Unrar source in separate form or as a part of other
   software is permitted, provided that it is clearly stated in
   the documentation and source comments that the code may not be used
   to develop a RAR (WinRAR) compatible archiver.

   More detailed license text is available in license.txt.

Status:
In progress..

