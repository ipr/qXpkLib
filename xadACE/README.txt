
xadACE - ACE unpack library

Based on Amiga PowerPC port of
public UNACE v1.2b January 2004 
copyright by: Marcel Lemke <mlemke@winace.com>
which was ported to Amiga by: 
Wilfred van Velzen <wilfred@aobh.xs4all.nl>

Under GPL license.

Modified to be used in C++ library 
by: Ilkka Prusi <ilkka.prusi@gmail.com>


Summary:
UnACE implementation for qXpkLib to load.

Notes:
This has same restrictions as code this was based on:
* no v2.0 decompression
* no EMS/XMS support (whatever that means..)
* no need to worry about 16-bit support, right? ;)
* no decryption

Basically this is just modified implementation 
so that it can be used by qXpkLib to unpack ACE-archives.

Modifications:
* removed console input/output
 - it's library and can be used GUI-app also
* remove "extern" and "static" globals 
 - these don't work well in C++ libraries
* platform-related stuff wrapped in classes
 - use ISO-standard typedefs whenever available
 - in general, use fixed-size typedef where value must be fixed-size
  -- such as reading 16-bit int from file -> use int16_t/uint16_t
 - if value size is allowed to change in build use size_t
  -- for example, file length can be 32-bit/64-bit int depending on build
 - you can easily define these if not included with compiler yet
 - this simplifies different builds of same sources
 - for example, Mac OS X and Linux uses LLP64 and Windows uses LP64,
   where (sizeof(long) is 8 or 4 bytes, pointer 8 in both)
* some preparations for 64-bit build support
 - see typedef related notes above

Status:
In progress..

