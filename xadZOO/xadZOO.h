#ifndef XADZOO_H
#define XADZOO_H

#include "xadZOO_global.h"

#include <QObject>

// base for library interface
#include "xadLibraryBase.h"

#include "qxpklib.h"

// fwd. decl.
class CUnZoo;

class XADZOOSHARED_EXPORT xadZOO : public xadLibraryBase
{
public:
    xadZOO();
    virtual ~xadZOO();

	// TODO: something like this needed to interface-base?    
	virtual bool setArchive(QString &file);

	// list files in archive, get other metadata also..
	virtual bool archiveInfo(QXpkLib::CArchiveInfo &info);

	// set path to uncompress files to
	virtual bool setExtractPath(QString &szPath);
	
	// test archive integrity
	virtual bool testArchive();
	
	// unpack/decompress
	virtual bool Decrunch(XpkProgress *pProgress);
	
private:
	CUnZoo *m_pArchive;
};


#ifdef __cplusplus
extern "C"
{
#endif

// need this for caller to locate instance dynamically,
// library needs to have one global instance of the cruncher (has no members anyway..)
//
XADZOOSHARED_EXPORT xadLibraryBase *GetXpkInstance(void);


#ifdef __cplusplus
}
#endif


#endif // XADZOO_H
