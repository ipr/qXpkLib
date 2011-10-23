#ifndef XADDMS_H
#define XADDMS_H

#include "xadDMS_global.h"

#include <QObject>

// base for library interface
#include "xadLibraryBase.h"

class XADDMSSHARED_EXPORT xadDMS : public xadLibraryBase
{
public:
    xadDMS();
    virtual ~xadDMS();

	// set path to uncompress files to
	virtual bool setExtractPath(QString &szPath);
	
	// test archive integrity
	virtual bool testArchive();
	
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
XADDMSSHARED_EXPORT xadLibraryBase *GetXpkInstance(void);

#ifdef __cplusplus
}
#endif

#endif // XADDMS_H
