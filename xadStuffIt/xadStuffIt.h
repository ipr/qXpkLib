#ifndef XADSTUFFIT_H
#define XADSTUFFIT_H

#include "xadStuffIt_global.h"

#include <QObject>
#include <QString>

// base for library interface
#include "xadLibraryBase.h"

#include "qxpklib.h"

class XADSTUFFITSHARED_EXPORT xadStuffIt : public xadLibraryBase
{
public:
    xadStuffIt();
    virtual ~xadStuffIt();

	// list files and retrieve metadata
	virtual bool archiveInfo(QXpkLib::CArchiveInfo &info);

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
XADSTUFFITSHARED_EXPORT xadLibraryBase *GetXpkInstance(void);

#ifdef __cplusplus
}
#endif

#endif // XADSTUFFIT_H
