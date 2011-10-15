#ifndef XADTAR_H
#define XADTAR_H

#include "xadTar_global.h"

#include <QObject>

// base for library interface
#include "xadLibraryBase.h"


class XADTARSHARED_EXPORT xadTar : public xadLibraryBase
{
public:
    xadTar();
    virtual ~xadTar();

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
XADTARSHARED_EXPORT xadLibraryBase *GetXpkInstance(void);


#ifdef __cplusplus
}
#endif

#endif // XADTAR_H
