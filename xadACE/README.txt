
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
* platform-related stuff wrapped in classes
* some preparations for 64-bit build support

Status:
In progress..

