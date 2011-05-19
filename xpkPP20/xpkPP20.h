//////////////////////////////////
//
// xpkPP20 : Amiga PowerPacker supporting library
// for qXpkLib
//

#ifndef XPKPP20_H
#define XPKPP20_H

#include "xpkPP20_global.h"

#include <QObject>

// base for library interface
#include "xpkLibraryBase.h"


class XPKPP20SHARED_EXPORT xpkPP20 : public xpkLibraryBase
{
public:
    xpkPP20();
    virtual ~xpkPP20();
	
	virtual bool Crunch(XpkProgress *pProgress);
	
	virtual bool Decrunch(XpkProgress *pProgress);
};

// need this for caller to locate instance dynamically,
// library needs to have one global instance of the cruncher (has no members anyway..)
//
XPKPP20SHARED_EXPORT xpkLibraryBase *GetXpkInstance(void);

#endif // XPKPP20_H

