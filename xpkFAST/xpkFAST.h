#ifndef XPKFAST_H
#define XPKFAST_H

#include "xpkFAST_global.h"

#include <QObject>

// base for library interface
#include "xpkLibraryBase.h"

// fwd. decl. for internal decrunch-helper
class XfdFAST;

class XPKFASTSHARED_EXPORT xpkFAST : public xpkLibraryBase
{
public:
    xpkFAST();
    virtual ~xpkFAST();

	//virtual bool Crunch(XpkProgress *pProgress);
	
	virtual bool Decrunch(XpkProgress *pProgress);
	
private:
	XfdFAST *m_pFast;
};

#ifdef __cplusplus
extern "C"
{
#endif

// need this for caller to locate instance dynamically,
// library needs to have one global instance of the cruncher (has no members anyway..)
//
XPKFASTSHARED_EXPORT xpkLibraryBase *GetXpkInstance(void);

#ifdef __cplusplus
}
#endif

#endif // XPKFAST_H
