#ifndef XADZOOM_H
#define XADZOOM_H

#include "xadZoom_global.h"

#include <QObject>

// base for library interface
#include "xadLibraryBase.h"

class XADZOOMSHARED_EXPORT xadZoom : public xadLibraryBase
{
public:
    xadZoom();
    virtual ~xadZoom();

	// unpack/decompress
	virtual bool Decrunch(XpkProgress *pProgress);
};


#ifdef __cplusplus
extern "C"
{
#endif

// need this for caller to locate instance dynamically,
// library needs to have one global instance of the cruncher (has no members anyway..)
//
XADZOOMSHARED_EXPORT xadLibraryBase *GetXpkInstance(void);

#ifdef __cplusplus
}
#endif

#endif // XADZOOM_H
