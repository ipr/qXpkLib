
xfdPowerPacker - Amiga PowerPacker pack/unpack library

Based on PP-Tools by: David Tritscher, davidtritscher@dodo.com.au

Reworked to C++ library by: Ilkka Prusi, ilkka.prusi@gmail.com

Unlimited distribution.

Summary:
Amiga PowerPacker decrunching implementation for qXpkLib to load.

This actually decrunches the original "stand-alone" format used by PowerPacker.

I believe there is also PowerPacker-supporting sub-library on Amiga for XPK ("PWPK")
which uses XPK chunk-format on the file instead of PowerPacker's own,
but that isn't yet supported in this.. 
Should add separate XPK chunk-supporting library for "PWPK", 
this is mainly for the "standalone" PowerPacker format.

Uses code from my previous conversion, see: https://github.com/ipr/PowerPacker-decrunch

Currently library gives access to one global instance of decruncher, which creates
helper-object on-stack: no need to synchronize/reset members in library
but it also expects whole file to be decrunched in one call.

