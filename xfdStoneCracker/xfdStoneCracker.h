//////////////////////////////////
//
// xfdStoneCracker : decruncher for Amiga StoneCracker
//

#ifndef XFDSTONECRACKER_H
#define XFDSTONECRACKER_H

#include "xfdStoneCracker_global.h"

#include <QObject>

// base for library interface
#include "xfdLibraryBase.h"

class XFDSTONECRACKERSHARED_EXPORT xfdStoneCracker : public xfdLibraryBase
{
public:
    xfdStoneCracker();
	virtual ~xfdStoneCracker();
	
    virtual bool isSupported(CReadBuffer *pInputBuffer);
    
	virtual bool Decrunch(XpkProgress *pProgress);
};

#ifdef __cplusplus
extern "C"
{
#endif

// need this for caller to locate instance dynamically,
// library needs to have one global instance of the cruncher (has no members anyway..)
//
XFDSTONECRACKERSHARED_EXPORT xfdLibraryBase *GetXpkInstance(void);

#ifdef __cplusplus
}
#endif

#endif // XFDSTONECRACKER_H
