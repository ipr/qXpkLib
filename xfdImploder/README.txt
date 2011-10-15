
xfdImploder - Amiga Imploder pack/unpack library

Based on Public Domain code by: Stuart Caie 

Reworked to C++ library by: Ilkka Prusi, ilkka.prusi@gmail.com

This code is in Public Domain.


Summary:
Amiga Imploder decrunching support library.

This supports decrunching the "standalone" packed file where Imploder has been used.
I don't think there is any case where Imploder-packing has been used within XPK "container"
but should be possible anyway..

Note that there are multiple variations of the format with different identifiers in file,
caller should detect those and load this library: supported variations detected in library also.

Currently library gives access to one global instance of decruncher, which creates
helper-object on-stack: no need to synchronize/reset members in library
but it also expects whole file to be decrunched in one call.

