#ifndef XADLHA_H
#define XADLHA_H

#include "xadLha_global.h"

#include <QObject>

// base for library interface
#include "xadLibraryBase.h"


class XADLHASHARED_EXPORT xadLha : public xadLibraryBase
{
public:
    xadLha();
    virtual ~xadLha();

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
XADLHASHARED_EXPORT xadLibraryBase *GetXpkInstance(void);


#ifdef __cplusplus
}
#endif


#endif // XADLHA_H
