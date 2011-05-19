//////////////////////////////////
//
// xpkGZIP : GNU-Zip support library for qXpkLib
//

#ifndef XPKGZIP_H
#define XPKGZIP_H

#include "xpkGZIP_global.h"

#include <QObject>

// base for library interface
#include "xpkLibraryBase.h"


class XPKGZIPSHARED_EXPORT xpkGZIP : public xpkLibraryBase
{
public:
    xpkGZIP();
    virtual ~xpkGZIP();

	virtual bool Crunch(XpkProgress *pProgress);
	virtual bool Decrunch(XpkProgress *pProgress);

};

// need this for caller to locate instance dynamically,
// library needs to have one global instance of the cruncher (has no members anyway..)
//
XPKGZIPSHARED_EXPORT xpkLibraryBase *GetXpkInstance(void);


#endif // XPKGZIP_H
