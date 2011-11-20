#ifndef XADLZX_H
#define XADLZX_H

#include "xadLZX_global.h"

#include <QObject>

// base for library interface
#include "xadLibraryBase.h"

#include "qxpklib.h"

// fwd. decl.
class CUnLzx;

class XADLZXSHARED_EXPORT xadLZX : public xadLibraryBase
{
public:
    xadLZX();
    virtual ~xadLZX();

    virtual bool isSupported(CIOBuffer *pInputBuffer);
	virtual bool archiveInfo(CIoContext *pInput, QXpkLib::CArchiveInfo &info);
	virtual bool testArchive(CIoContext *pInput);
	
	// unpack/decompress
	virtual bool Decrunch(XpkProgress *pProgress);
	
private:
	CUnLzx *m_pArchive;
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
