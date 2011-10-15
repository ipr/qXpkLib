#ifndef XADCAB_H
#define XADCAB_H

#include "xadCAB_global.h"

#include <QObject>

// base for library interface
#include "xadLibraryBase.h"


class XADCABSHARED_EXPORT xadCAB : public xadLibraryBase
{
public:
    xadCAB();
    virtual ~xadCAB();

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
XADCABSHARED_EXPORT xadLibraryBase *GetXpkInstance(void);


#ifdef __cplusplus
}
#endif

#endif // XADCAB_H
