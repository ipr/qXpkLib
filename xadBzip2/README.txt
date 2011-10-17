
xadBzip2 - Bzip2 unpack library

Uses bzip2/libbzip2 code:
Copyright (C) 1996-2010 Julian Seward <jseward@bzip.org>

Modified to use in C++ library:
Author: Ilkka Prusi
Contact ilkka.prusi@gmail.com


"   This program is free software; you can redistribute it and/or modify"
"   it under the terms set out in the LICENSE file, which is included"
"   in the bzip2-1.0.6 source distribution."


Summary:
Bzip2 implementation for qXpkLib to load.

Notes:
This library has multiple changes to original code.
* no console-output
 - it would not make sense when library is used in GUI-app
* use exceptions instead of exit() or similar
 - crashing in some library is not a good idea -> allow chance to recover gracefully
* "extern" variable is not really usable in such way with C++
 - there are better alternatives anyway..
* reduce preprocessor-macros and define's
 - "inline" generally does better job and we get better type-detection for compiler etc.
* prefer ISO-standard typedefs whenever possible
 - if your compiler doesn't include <stdint.h> either upgrade or add them yourself
* some other platform/librar stuff..

Status:
In progress..

