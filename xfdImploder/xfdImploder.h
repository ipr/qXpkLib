//////////////////////////////////
//
// xfdImploder : Amiga Imploder supporting library
// for qXpkLib
//

#ifndef XFDIMPLODER_H
#define XFDIMPLODER_H

#include "xfdImploder_global.h"

#include <QObject>

// base for library interface
#include "xfdLibraryBase.h"


class XFDIMPLOLDERSHARED_EXPORT xfdImploder : public xfdLibraryBase
{
public:
    xfdImploder();
	virtual ~xfdImploder();
	
	virtual bool Decrunch(XpkProgress *pProgress);
	
};

#ifdef __cplusplus
extern "C"
{
#endif

// need this for caller to locate instance dynamically,
// library needs to have one global instance of the cruncher (has no members anyway..)
//
XFDIMPLOLDERSHARED_EXPORT xfdLibraryBase *GetXpkInstance(void);


#ifdef __cplusplus
}
#endif


#endif // XFDIMPLODER_H
