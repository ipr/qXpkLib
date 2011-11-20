/* 
xadHA -- HA archive unpacker
Author: Harri Hirvola 

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

#ifndef XADHA_H
#define XADHA_H

#include "xadHA_global.h"

#include <QObject>

// base for library interface
#include "xadLibraryBase.h"

#include "qxpklib.h"

// fwd. decl.
class CHA;

class XADHASHARED_EXPORT xadHA : public xadLibraryBase
{
public:
    xadHA();
    virtual ~xadHA();

    virtual bool isSupported(CIOBuffer *pInputBuffer);
	virtual bool archiveInfo(CIoContext *pInput, QXpkLib::CArchiveInfo &info);
	virtual bool testArchive(CIoContext *pInput);
	
	// unpack/decompress
	virtual bool Decrunch(XpkProgress *pProgress);
	
private:
	CHA *m_pArchive;
};

#ifdef __cplusplus
extern "C"
{
#endif

// need this for caller to locate instance dynamically,
// library needs to have one global instance of the cruncher (has no members anyway..)
//
XADHASHARED_EXPORT xadLibraryBase *GetXpkInstance(void);

#ifdef __cplusplus
}
#endif

#endif // XADHA_H
