
xpkIMPL - Amiga Imploder pack/unpack library


Author: Ilkka Prusi
Contact ilkka.prusi@gmail.com


Summary:
Amiga Imploder decrunching support library.

This supports decrunching the "standalone" packed file where Imploder has been used.
I don't think there is any case where Imploder-packing has been used within XPK "container"
but should be possible anyway..

Note that there are multiple variations of the format with different identifiers in file,
caller should detect those and load this library: supported variations detected in library also.

