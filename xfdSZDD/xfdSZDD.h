#ifndef XFDSZDD_H
#define XFDSZDD_H

#include "xfdSZDD_global.h"

#include <QObject>

// base for library interface
#include "xpkLibraryBase.h"

class XFDSZDDSHARED_EXPORT xfdSZDD : public xpkLibraryBase
{
public:
    xfdSZDD();
    virtual ~xfdSZDD();

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
XFDSZDDSHARED_EXPORT xpkLibraryBase *GetXpkInstance(void);

#ifdef __cplusplus
}
#endif


#endif // XFDSZDD_H
