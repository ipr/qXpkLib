#ifndef XPKNUKE_H
#define XPKNUKE_H

#include "xpkNUKE_global.h"

#include <QObject>

// base for library interface
#include "xpkLibraryBase.h"


class XPKNUKESHARED_EXPORT xpkNUKE : public xpkLibraryBase
{
public:
    xpkNUKE();
    virtual ~xpkNUKE();

	virtual bool Crunch(XpkProgress *pProgress);
	
	virtual bool Decrunch(XpkProgress *pProgress);
	
};

// need this for caller to locate instance dynamically,
// library needs to have one global instance of the cruncher (has no members anyway..)
//
XPKNUKESHARED_EXPORT xpkLibraryBase *GetXpkInstance(void);

#endif // XPKNUKE_H
