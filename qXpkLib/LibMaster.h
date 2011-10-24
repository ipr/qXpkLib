///////////////////////////////////
//
// CLibMaster : handler of various decrunchers and files,
// uses XPK/XFD/XAD masters for additional decrunchers as needed.
//
// Ilkka Prusi
// ilkka.prusi@gmail.com
//


#ifndef LIBMASTER_H
#define LIBMASTER_H

#include <QObject>

#include <QObject>
#include <QString>
#include <QList>
#include <QByteArray>
#include <QLibrary>

// use typedefs from parent
#include <qxpklib.h>

#include "AnsiFile.h"
#include "IoContext.h"

// reuse type detection,
// multiple variations etc.
//
#include "FileType.h"

// XPK-support
#include "XpkMaster.h"

// XFD-support
#include "XfdMaster.h"

// XAD-support
#include "XadMaster.h"

// pure virtual interface
// for lower-level library (level 0)
//
#include "xpkLibraryBase.h"
#include "XpkProgress.h"


class CLibMaster : public QObject
{
    Q_OBJECT
    
private:
	QString m_InputName;
	CReadBuffer m_InputBuffer;
	size_t m_nInputFileSize;
	
	// for multi-file extraction
	QString m_outputPath;

	// TODO: change to support user-given buffer..?
	CIoContext m_Input;
	
	// change to use path and determine name automagically?
	CIoContext m_Output;
	
	// wrapper for loading/unloading
	QLibrary m_SubLib;

	// temp, determine later if suitable way..
	CXpkMaster *m_pXpkMaster;
	CXfdMaster *m_pXfdMaster;
	CXadMaster *m_pXadMaster;

	// might keep necessary information already here per archive..?
	//QXpkLib::CArchiveInfo m_info;
	
protected:
	std::string getCruncherType(CReadBuffer *pInputBuffer);
	void PrepareUnpacker(std::string &subType);

	//bool ForeignDecrunch(XpkProgress *pProgress);
	bool OwnDecrunch(XpkProgress *pProgress);
    
public:
    explicit CLibMaster(QObject *parent = 0);
	virtual ~CLibMaster(void);

	bool archiveInfo(QXpkLib::CArchiveInfo &info);
	
	bool archiveUnpack(XpkProgress *pProgress);

	// get unpacked result to user-buffer as-is
	CReadBuffer *getResult()
	{
		return m_Output.GetBuffer();
	}

public slots:
	bool setInputBuffer(CReadBuffer *buffer);

	bool setInputFile(QString &szFile);
	
	// TODO: check what to do with these..
	void setOutputFile(QString &szFile) 
	{
		m_Output.setName(szFile);
	}
	void setOutputPath(QString &szPath) 
	{
		m_outputPath = szPath;
	}
	
signals:
	// errors with exceptions, other with messages
	void message(QString);
	void warning(QString);

};

#endif // LIBMASTER_H
