//////////////////////////////////
//
// xfdVDCO : VDCO (Virtual Dreams) decruncher
//

#ifndef XFDVDCO_H
#define XFDVDCO_H

#include "xfdVDCO_global.h"

#include <QObject>

// base for library interface
#include "xfdLibraryBase.h"

class XFDVDCOSHARED_EXPORT xfdVDCO : public xfdLibraryBase
{
public:
    xfdVDCO();
	virtual ~xfdVDCO();
	
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
XFDVDCOSHARED_EXPORT xfdLibraryBase *GetXpkInstance(void);

#ifdef __cplusplus
}
#endif

#endif // XFDVDCO_H
