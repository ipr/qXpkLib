//////////////////////////////////
//
// xfdPowerPacker : Amiga PowerPacker supporting library
// for qXpkLib
//

#ifndef XFDPOWERPACKER_H
#define XFDPOWERPACKER_H

#include "xfdPowerPacker_global.h"

#include <QObject>

// base for library interface
#include "xfdLibraryBase.h"


class XFDPOWERPACKERSHARED_EXPORT xfdPowerPacker : public xfdLibraryBase
{
public:
    xfdPowerPacker();
    virtual ~xfdPowerPacker();
	
	virtual bool Decrunch(XpkProgress *pProgress);
};


#ifdef __cplusplus
extern "C"
{
#endif

// need this for caller to locate instance dynamically,
// library needs to have one global instance of the cruncher (has no members anyway..)
//
XFDPOWERPACKERSHARED_EXPORT xfdLibraryBase *GetXpkInstance(void);


#ifdef __cplusplus
}
#endif

#endif // XFDPOWERPACKER_H

