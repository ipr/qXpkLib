#ifndef XPKRLEN_H
#define XPKRLEN_H

#include "xpkRLEN_global.h"

#include <QObject>

// base for library interface
#include "xpkLibraryBase.h"


class XPKRLENSHARED_EXPORT xpkRLEN : public xpkLibraryBase
{
public:
    xpkRLEN();
	virtual ~xpkRLEN();

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
XPKRLENSHARED_EXPORT xpkLibraryBase *GetXpkInstance(void);

#ifdef __cplusplus
}
#endif

#endif // XPKRLEN_H
