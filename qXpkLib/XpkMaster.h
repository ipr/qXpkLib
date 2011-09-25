///////////////////////////////////
//
// Actual decoder handling:
// can load additional decoders as necessary,
// see xpkLibraryBase
//
// CXpkMaster : main decrunch handler
// CXpkLibrarian : sub-library loader&handler
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
#include "IoContext.h"

// pure virtual interface
// for lower-level library (level 0)
//
#include "xpkLibraryBase.h"
#include "XpkProgress.h"

// temp..
#include "XpkTags.h"


// container/loader of derunching libraries?
//
class CXpkLibrarian : public QObject
{
protected:
	typedef xpkLibraryBase *GetXpkInstance();
	
public:
	static QList<QString> availableLibraries();
	
	//static xpkLibraryBase *getDecruncher(QString szType, QLibrary &lib);
	static xpkLibraryBase *getDecruncher(std::string &szType, QLibrary &lib);
};


class CXpkMaster : public QObject
{
	Q_OBJECT

private:
	//CIoContext m_Input;
	QString m_InputName;
	CReadBuffer m_InputBuffer;
	size_t m_nInputFileSize;
	
	CIoContext m_Output;
	
	// wrapper for loading/unloading
	QLibrary m_SubLib;
	
	// base-pointer (virtual) 
	// -> load proper by file/packer type
	xpkLibraryBase *m_pSubLibrary;
	
	// some flags (no idea what..)
	//bool m_bIgnore;
	//bool m_bNoClobber;

	// temp for testing
	XpkTags m_Tags;
	
protected:
	std::string getCruncherType(CReadBuffer *pInputBuffer) const;
	void PrepareUnpacker(std::string &subType);

	bool ForeignDecrunch(XpkProgress *pProgress);
	bool OwnDecrunch(XpkProgress *pProgress);
	
public:
    CXpkMaster(QObject *parent = 0);
	virtual ~CXpkMaster(void);
	
	bool xpkInfo(QXpkLib::CXpkFileInfo &info);
	
	bool xpkUnpack(XpkProgress *pProgress);

	// get unpacked result to user-buffer as-is
	CReadBuffer *getResult()
	{
		return m_Output.GetBuffer();
	}
	
public slots:
	// TODO: only on instance-creation?
	void setInputFile(QString &szFile) 
	{
		// TODO: read metadata when this is set or later only?
		m_InputName = szFile;
	}
	void setOutputFile(QString &szFile) 
	{
		m_Output.setName(szFile);
	}
	
signals:
	// errors with exceptions, other with messages
	void message(QString);
	void warning(QString);
	
};

#endif // XPKMASTER_H
