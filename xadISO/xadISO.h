#ifndef XADISO_H
#define XADISO_H

#include "xadISO_global.h"

#include <QObject>
#include <QString>

// base for library interface
#include "xadLibraryBase.h"

// fwd. decl.
class QFile;

class XADISOSHARED_EXPORT xadISO : public xadLibraryBase
{
public:
    xadISO();
    virtual ~xadISO();

	// set path to uncompress files to
	virtual bool setExtractPath(QString &szPath);
	
	// test archive integrity
	virtual bool testArchive();
	
	// unpack/decompress
	virtual bool Decrunch(XpkProgress *pProgress);
	
private:
	QString m_filename;
	QFile *m_pFile;
	
};


#ifdef __cplusplus
extern "C"
{
#endif

// need this for caller to locate instance dynamically,
// library needs to have one global instance of the cruncher (has no members anyway..)
//
XADISOSHARED_EXPORT xadLibraryBase *GetXpkInstance(void);


#ifdef __cplusplus
}
#endif


#endif // XADISO_H
