#ifndef XPKILZR_H
#define XPKILZR_H

#include "xpkILZR_global.h"

#include <QObject>

// base for library interface
#include "xpkLibraryBase.h"

class XPKILZRSHARED_EXPORT xpkILZR : public xpkLibraryBase
{
public:
    xpkILZR();
	virtual ~xpkILZR();

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
XPKILZRSHARED_EXPORT xpkLibraryBase *GetXpkInstance(void);

#ifdef __cplusplus
}
#endif

#endif // XPKILZR_H
