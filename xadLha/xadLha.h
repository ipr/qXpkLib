#ifndef XADLHA_H
#define XADLHA_H

#include "xadLha_global.h"

#include <QObject>

// base for library interface
#include "xadLibraryBase.h"

// fwd. decl.
class CLhArchive;

class XADLHASHARED_EXPORT xadLha : public xadLibraryBase
{
public:
    xadLha();
    virtual ~xadLha();

	//virtual bool fileList(QList<archiveEntry> &entryList);

	// set path to uncompress files to
	virtual bool setExtractPath(QString &szPath);

	// test archive integrity
	virtual bool testArchive();
	
	// unpack/decompress
	virtual bool Decrunch(XpkProgress *pProgress);
	
private:
	CLhArchive *m_pArchive;
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
