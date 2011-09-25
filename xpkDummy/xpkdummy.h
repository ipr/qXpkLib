#ifndef XPKDUMMY_H
#define XPKDUMMY_H

#include "xpkDummy_global.h"

#include <QObject>

// base for library interface
#include "xpkLibraryBase.h"


class XPKDUMMYSHARED_EXPORT xpkDummy : public xpkLibraryBase
{
public:
	xpkDummy();
	virtual ~xpkDummy();
	
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
XPKDUMMYSHARED_EXPORT xpkLibraryBase *GetXpkInstance(void);

#ifdef __cplusplus
}
#endif

#endif // XPKDUMMY_H
