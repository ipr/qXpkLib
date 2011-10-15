#ifndef XFDZCOMPRESS_H
#define XFDZCOMPRESS_H

#include "xfdZCompress_global.h"

#include <QObject>

// base for library interface
#include "xfdLibraryBase.h"

class XFDZCOMPRESSSHARED_EXPORT xfdZCompress : public xfdLibraryBase
{
public:
    xfdZCompress();
    virtual ~xfdZCompress();

	virtual bool Decrunch(XpkProgress *pProgress);

};

#ifdef __cplusplus
extern "C"
{
#endif

// need this for caller to locate instance dynamically,
// library needs to have one global instance of the cruncher (has no members anyway..)
//
XFDZCOMPRESSSHARED_EXPORT xfdLibraryBase *GetXpkInstance(void);

#ifdef __cplusplus
}
#endif


#endif // XFDZCOMPRESS_H
