#ifndef XPKHUFF_H
#define XPKHUFF_H

#include "xpkHUFF_global.h"

#include <QObject>

// base for library interface
#include "xpkLibraryBase.h"

class XPKHUFFSHARED_EXPORT xpkHUFF : public xpkLibraryBase
{
public:
    xpkHUFF();
    virtual ~xpkHUFF();

	virtual bool Crunch(XpkProgress *pProgress);
	
	virtual bool Decrunch(XpkProgress *pProgress);
	
};

#ifdef __cplusplus
extern "C"
{
#endif

// need this for caller to locate instance dynamically,
// library needs to have one global instance of the cruncher (has no members anyway..)
//
XPKHUFFSHARED_EXPORT xpkLibraryBase *GetXpkInstance(void);

#ifdef __cplusplus
}
#endif

#endif // XPKHUFF_H
