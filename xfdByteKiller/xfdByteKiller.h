//////////////////////////////////
//
// xfdByteKiller : decruncher for Amiga ByteKiller and variants
//

#ifndef XFDBYTEKILLER_H
#define XFDBYTEKILLER_H

#include "xfdByteKiller_global.h"

#include <QObject>

// base for library interface
#include "xfdLibraryBase.h"

class XFDBYTEKILLERSHARED_EXPORT xfdByteKiller : public xfdLibraryBase
{
public:
    xfdByteKiller();
	virtual ~xfdByteKiller();

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
XFDBYTEKILLERSHARED_EXPORT xfdLibraryBase *GetXpkInstance(void);

#ifdef __cplusplus
}
#endif


#endif // XFDBYTEKILLER_H
