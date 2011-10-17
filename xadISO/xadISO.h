////////////////////////////////////////
//
// xadISO - unpacking extension library for qXpkLib
//
// Allows "extracting" files from ISO-image of CD/DVD rom
// without mounting as virtual-device.
//
// Note: currently planning only "base" ISO 9660 support,
// RockRidge/Joliet/HFS/UDF not planned for now..
//
// Note2: will not include CDDA, use other tools for that.
//
// Author: Ilkka Prusi, 2011
// ilkka.prusi@gmail.com
//

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
	QString m_destPath;
	QString m_filename;
	
	QFile *m_pFile;
	
	uchar *findDescriptorIdentifier(uchar *pos, const uchar *end, const char *identifier);
	
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
