#ifndef XPKNONE_H
#define XPKNONE_H

#include "xpkNONE_global.h"

#include <QObject>

// base for library interface
#include "xpkLibraryBase.h"


class XPKNONESHARED_EXPORT xpkNONE : public xpkLibraryBase
{
public:
	xpkNONE();
	virtual ~xpkNONE();

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
XPKNONESHARED_EXPORT xpkLibraryBase *GetXpkInstance(void);

#ifdef __cplusplus
}
#endif

#endif // XPKNONE_H
