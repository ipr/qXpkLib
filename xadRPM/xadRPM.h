#ifndef XADRPM_H
#define XADRPM_H

#include "xadRPM_global.h"

#include <QObject>

// base for library interface
#include "xadLibraryBase.h"

class XADRPMSHARED_EXPORT xadRPM : public xadLibraryBase
{
public:
    xadRPM();
    virtual ~xadRPM();

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
XADRPMSHARED_EXPORT xadLibraryBase *GetXpkInstance(void);

#ifdef __cplusplus
}
#endif

#endif // XADRPM_H
