//////////////////////////////////
//
// xfdCrunchMania : decruncher for Amiga Crunch-Mania
//

#ifndef XFDCRUNCHMANIA_H
#define XFDCRUNCHMANIA_H

#include "xfdCrunchMania_global.h"

#include <QObject>

// base for library interface
#include "xfdLibraryBase.h"

class XFDCRUNCHMANIASHARED_EXPORT xfdCrunchMania : public xfdLibraryBase
{
public:
    xfdCrunchMania();
	virtual ~xfdCrunchMania();
	
    virtual bool isSupported(CReadBuffer *pInputBuffer);

	virtual bool archiveInfo(QXpkLib::CArchiveInfo &info);
	
	virtual bool Decrunch(XpkProgress *pProgress);
};

#ifdef __cplusplus
extern "C"
{
#endif

// need this for caller to locate instance dynamically,
// library needs to have one global instance of the cruncher (has no members anyway..)
//
XFDCRUNCHMANIASHARED_EXPORT xfdLibraryBase *GetXpkInstance(void);

#ifdef __cplusplus
}
#endif

#endif // XFDCRUNCHMANIA_H
