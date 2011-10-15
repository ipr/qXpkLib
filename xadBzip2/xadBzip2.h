#ifndef XADBZIP2_H
#define XADBZIP2_H

#include "xadBzip2_global.h"

#include <QObject>

// base for library interface
#include "xadLibraryBase.h"


class XADBZIP2SHARED_EXPORT xadBzip2 : public xadLibraryBase
{
public:
    xadBzip2();
    virtual ~xadBzip2();

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
XADBZIP2SHARED_EXPORT xadLibraryBase *GetXpkInstance(void);


#ifdef __cplusplus
}
#endif

#endif // XADBZIP2_H
