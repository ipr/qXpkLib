#ifndef XPKSQSH_H
#define XPKSQSH_H

#include "xpkSQSH_global.h"

#include <QObject>

// base for library interface
#include "xpkLibraryBase.h"


class XPKSQSHSHARED_EXPORT xpkSQSH : public xpkLibraryBase
{
public:
    xpkSQSH();
    virtual ~xpkSQSH();

	virtual bool Crunch(XpkProgress *pProgress);
	
	virtual bool Decrunch(XpkProgress *pProgress);

};

// need this for caller to locate instance dynamically,
// library needs to have one global instance of the cruncher (has no members anyway..)
//
XPKSQSHSHARED_EXPORT xpkLibraryBase *GetXpkInstance(void);


#endif // XPKSQSH_H
