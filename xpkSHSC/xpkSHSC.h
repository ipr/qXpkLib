/* 
xpkSHSC -- smart delta with HA (ASC/HSC)
Author: Harri Hirvola and chinoclast@softhome.net (Gaelan Griffin)

Modified for C++ library version by Ilkka Prusi <ilkka.prusi@gmail.com>
for qXpkLib project.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/


#ifndef XPKSHSC_H
#define XPKSHSC_H

#include "xpkSHSC_global.h"

#include <QObject>

// base for library interface
#include "xpkLibraryBase.h"

class XPKSHSCSHARED_EXPORT xpkSHSC : public xpkLibraryBase
{
public:
    xpkSHSC();
    virtual ~xpkSHSC();

	//virtual bool Crunch(XpkProgress *pProgress);
	
	virtual bool Decrunch(XpkProgress *pProgress);
};

#ifdef __cplusplus
extern "C"
{
#endif

// need this for caller to locate instance dynamically,
// library needs to have one global instance of the cruncher (has no members anyway..)
//
XPKSHSCSHARED_EXPORT xpkLibraryBase *GetXpkInstance(void);

#ifdef __cplusplus
}
#endif

#endif // XPKSHSC_H
