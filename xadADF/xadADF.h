#ifndef XADADF_H
#define XADADF_H

#include "xadADF_global.h"

#include <QObject>

// base for library interface
#include "xadLibraryBase.h"

#include "qxpklib.h"


class XADADFSHARED_EXPORT xadADF : public xadLibraryBase
{
public:
    xadADF();
    virtual ~xadADF();

	// TODO: something like this needed to interface-base?    
	//virtual bool setArchive(QString &file);

	// list files in archive, get other metadata also..
	virtual bool archiveInfo(QXpkLib::CArchiveInfo &info);

	// set path to uncompress files to
	//virtual bool setExtractPath(QString &szPath);
	
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
XADADFSHARED_EXPORT xadLibraryBase *GetXpkInstance(void);

#ifdef __cplusplus
}
#endif


#endif // XADADF_H
