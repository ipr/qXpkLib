#ifndef XADLZX_H
#define XADLZX_H

#include "xadLZX_global.h"

#include <QObject>

// base for library interface
#include "xadLibraryBase.h"


class XADLZXSHARED_EXPORT xadLZX : public xadLibraryBase
{
public:
    xadLZX();
    virtual ~xadLZX();

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
XADLZXSHARED_EXPORT xadLibraryBase *GetXpkInstance(void);


#ifdef __cplusplus
}
#endif


#endif // XADLZX_H
