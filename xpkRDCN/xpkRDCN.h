#ifndef XPKRDCN_H
#define XPKRDCN_H

#include "xpkRDCN_global.h"

#include <QObject>

// base for library interface
#include "xpkLibraryBase.h"

class XPKRDCNSHARED_EXPORT xpkRDCN : public xpkLibraryBase
{
public:
    xpkRDCN();
	virtual ~xpkRDCN();

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
XPKRDCNSHARED_EXPORT xpkLibraryBase *GetXpkInstance(void);

#ifdef __cplusplus
}
#endif

#endif // XPKRDCN_H
