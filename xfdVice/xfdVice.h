//////////////////////////////////
//
// xfdVice : Vice cruncher decrunching
//

#ifndef XFDVICE_H
#define XFDVICE_H

#include "xfdVice_global.h"

#include <QObject>

// base for library interface
#include "xfdLibraryBase.h"

class XFDVICESHARED_EXPORT xfdVice : public xfdLibraryBase
{
public:
    xfdVice();
	virtual ~xfdVice();
	
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
XFDVICESHARED_EXPORT xfdLibraryBase *GetXpkInstance(void);

#ifdef __cplusplus
}
#endif

#endif // XFDVICE_H
