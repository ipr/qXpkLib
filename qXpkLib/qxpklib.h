///////////////////////////////////
//
// QXpkLib:
// just the library entry-point/interface 
// as Qt-object for loading into an application.
//
// Original idea was just a thin wrapper for original code
// but most of everything seems to become entirely 
// redesigned and rewritten from scratch now.
// So only "common" thing with old XPK-library
// is the ability to read same files..
//
//
// Ilkka Prusi
// ilkka.prusi@gmail.com
//

#ifndef QXPKLIB_H
#define QXPKLIB_H

#include "qXpkLib_global.h"

#include <QObject>
#include <QByteArray>


//// fwd. decl.
class CXpkMaster;
//class CxpkProgress; // TODO: see about what to do..
struct XpkProgress;


class QXPKLIBSHARED_EXPORT QXpkLib : public QObject
{
	Q_OBJECT

private:
	CXpkMaster *m_pMaster;
	
	// current pack/unpack related item
	//TagItem *m_pItem;
	
	// status (display also or just internal?)
	XpkProgress *m_pProgress;
	
protected:
	void PrepareMaster();
	
public:
    QXpkLib(QObject *parent = 0);
	virtual ~QXpkLib();

	// info to caller about compressed file
	// (metadata such as cruncher etc.)
	class CXpkFileInfo
	{
	public:
		CXpkFileInfo() {}
		
		unsigned long m_ulFileSize; // actual file size
		unsigned long m_ulPackedDataSize; // size of compressed data
		unsigned long m_ulUnpackedSize; // uncompressed size
		
		QString m_CompressionType; // cruncher type
	};
	
	// get information on selected file
	// (compression, type etc.)
	bool xpkInfo(QXpkLib::CXpkFileInfo &info);

	// pack/unpack to/from given input&output
	bool xpkPack();
	bool xpkUnpack();

	// if user wants result to buffer?
	void getToBuffer(QByteArray &Array);
	
signals:
	void message(QString);
	void warning(QString);
	void error(QString);

	//void status(type?);
	
public slots:
	
	// set filename and path
	void setInputFile(QString &szFile);
	void setOutputFile(QString &szFile);

};

#endif // QXPKLIB_H
