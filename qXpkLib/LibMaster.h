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

#include "PathHelper.h"

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
	// for multi-file extraction
	QString m_outputPath;

	// TODO: change to support user-given buffer..?
	CIoContext *m_pInput;
	
	// change to use path and determine name automagically?
	CIoContext *m_pOutput;
	
	// temp, determine later if suitable way..
	// might use some common interface..?
	CXpkMaster *m_pXpkMaster;
	CXfdMaster *m_pXfdMaster;
	CXadMaster *m_pXadMaster;
	
	CFileType m_fileType;
	CPathHelper m_pathHelper;

	// might keep necessary information already here per archive..?
	QXpkLib::CArchiveInfo m_info;

	// status (display also or just internal?)
	XpkProgress *m_pProgress;
	
public:
    explicit CLibMaster(QObject *parent = 0);
	virtual ~CLibMaster(void);

	bool archiveInfo(QXpkLib::CArchiveInfo &info);
	
	bool archiveUnpack();

	// get unpacked result to user-buffer as-is
	CIOBuffer *getResult()
	{
		if (m_pOutput != nullptr)
		{
			return m_pOutput->getBuffer();
		}
		return nullptr;
	}

	// single-file output or no name/path known
	CIoContext *getInput()
	{
		return m_pInput;
	}
	
	// single-file output or no name/path known
	CIoContext *getOutput()
	{
		return m_pOutput;
	}

	// file name and path given:
	// give output for that
	CIoContext *getOutput(QString &filePath);

public slots:
	bool setInputBuffer(CIOBuffer *buffer);
	bool setInputFile(QString &szFile);
	
	// TODO: check what to do with these..
	bool setOutputBuffer(CIOBuffer *buffer);
	bool setOutputFile(QString &szFile);
	void setOutputPath(QString &szPath);
	
signals:
	// errors with exceptions, other with messages
	void message(QString);
	void warning(QString);

};

#endif // LIBMASTER_H
