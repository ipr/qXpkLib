///////////////////////////////////
//
// Actual decoder handling:
// can load additional decoders as necessary,
// see xpkLibraryBase
//

#ifndef XPKMASTER_H
#define XPKMASTER_H

#include <QObject>
#include <QString>
#include <QList>
#include <QByteArray>
#include <QLibrary>

#include "AnsiFile.h"

// pure virtual interface
// for lower-level library (level 0)
//
#include "xpkLibraryBase.h"
#include "XpkProgress.h"



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
	QString m_InputName;
	CReadBuffer m_InputBuffer;
	size_t m_nInputFileSize;
	
	QString m_OutputName;
	CReadBuffer m_OutputBuffer;
	
	// wrapper for loading/unloading
	QLibrary m_SubLib;
	
	// base-pointer (virtual) 
	// -> load proper by file/packer type
	xpkLibraryBase *m_pSubLibrary;
	
	// some flags (no idea what..)
	//bool m_bIgnore;
	//bool m_bNoClobber;

protected:
	void PrepareUnpacker();
	void PreparePacker();
	
	
public:
    CXpkMaster(QObject *parent = 0);
	virtual ~CXpkMaster(void);
	
	bool xpkPack(XpkProgress *pProgress);
	bool xpkUnpack(XpkProgress *pProgress);

	CReadBuffer *getResult()
	{
		return &m_OutputBuffer;
	}
	
public slots:
	// TODO: only on instance-creation?
	void setInputFile(QString &szFile) 
	{
		m_InputName = szFile;
	}
	void setOutputFile(QString &szFile) 
	{
		m_OutputName = szFile;
	}
	
signals:
	// errors with exceptions, other with messages
	void message(QString);
	void warning(QString);
	//void error(QString);
	
};

#endif // XPKMASTER_H
