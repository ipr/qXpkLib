#ifndef XPKDUKE_H
#define XPKDUKE_H

#include "xpkDUKE_global.h"

#include <QObject>

// base for library interface
#include "xpkLibraryBase.h"

class XPKDUKESHARED_EXPORT xpkDUKE : public xpkLibraryBase
{
public:
    xpkDUKE();
    virtual ~xpkDUKE();

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
XPKDUKESHARED_EXPORT xpkLibraryBase *GetXpkInstance(void);

#ifdef __cplusplus
}
#endif

#endif // XPKDUKE_H
