#ifndef XADZOO_H
#define XADZOO_H

#include "xadZOO_global.h"

#include <QObject>
#include <QString>

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

    virtual bool isSupported(CIOBuffer *pInputBuffer);
	virtual bool archiveInfo(CIoContext *pInput, QXpkLib::CArchiveInfo &info);
	virtual bool testArchive(CIoContext *pInput);
	
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
