///////////////////////////////////
//
// Actual decoder handling:
// can load additional decoders as necessary,
// see xpkLibraryBase
//
// CXpkMaster : main decrunch handler
//
// Ilkka Prusi
// ilkka.prusi@gmail.com
//

#ifndef XPKMASTER_H
#define XPKMASTER_H

#include <QObject>
#include <QString>
#include <QList>
#include <QByteArray>
#include <QLibrary>

// use typedefs from parent
#include <qxpklib.h>

#include "AnsiFile.h"
#include "FileType.h"

// pure virtual interface
// for lower-level library (level 0)
//
#include "xpkLibraryBase.h"
#include "XpkProgress.h"

// temp.. move here?
#include "XpkTags.h"



class CXpkMaster : public QObject
{
	Q_OBJECT

private:
	//CIoContext m_Input;
	QString m_InputName;
	CReadBuffer m_InputBuffer;
	size_t m_nInputFileSize;
	
	// wrapper for loading/unloading
	QLibrary m_SubLib;

	// base-pointer (virtual) 
	// -> load proper by file/packer type
	xpkLibraryBase *m_pSubLibrary;
	
	// temp for testing
	XpkTags m_Tags;
	
protected:
	void PrepareUnpacker(std::string &subType);
	
public:
    CXpkMaster(QObject *parent = 0);
	virtual ~CXpkMaster(void);

    bool isSupported(CReadBuffer *pInputBuffer, CFileType &type);
	bool decrunch(XpkProgress *pProgress);
	
	bool xpkUnpack(XpkProgress *pProgress);

	
public slots:
	
signals:
	// errors with exceptions, other with messages
	void message(QString);
	void warning(QString);
	
};

#endif // XPKMASTER_H
