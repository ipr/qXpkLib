#ifndef XADZAP_H
#define XADZAP_H

#include "xadZAP_global.h"

#include <QObject>

// base for library interface
#include "xadLibraryBase.h"


class XADZAPSHARED_EXPORT xadZAP : public xadLibraryBase
{
public:
    xadZAP();
    virtual ~xadZAP();

	// set path to uncompress files to
	//virtual bool setExtractPath(QString &szPath);
	
	// test archive integrity
	//virtual bool testArchive();
	
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
XADZAPSHARED_EXPORT xadLibraryBase *GetXpkInstance(void);


#ifdef __cplusplus
}
#endif


#endif // XADZAP_H
