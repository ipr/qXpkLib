//////////////////////////////////
//
// xfdGZIP : GNU-Zip support library for qXpkLib
//

#ifndef XFDGZIP_H
#define XFDGZIP_H

#include "xfdGZIP_global.h"

#include <QObject>

// base for library interface
#include "xfdLibraryBase.h"


class XFDGZIPSHARED_EXPORT xpkGZIP : public xfdLibraryBase
{
public:
    xfdGZIP();
    virtual ~xfdGZIP();

	virtual bool Decrunch(XpkProgress *pProgress);

};

#ifdef __cplusplus
extern "C"
{
#endif

// need this for caller to locate instance dynamically,
// library needs to have one global instance of the cruncher (has no members anyway..)
//
XFDGZIPSHARED_EXPORT xfdLibraryBase *GetXpkInstance(void);

#ifdef __cplusplus
}
#endif

#endif // XFDGZIP_H