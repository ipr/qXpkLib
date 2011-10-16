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
	//CIoContext m_Input;
	QString m_InputName;
	CReadBuffer m_InputBuffer;
	size_t m_nInputFileSize;
	
	// for multi-file extraction
	QString m_outputPath;
	
	// change to use path and determine name automagically?
	CIoContext m_Output;
	
	// wrapper for loading/unloading
	QLibrary m_SubLib;

	// temp, determine later if suitable way..
	CXpkMaster *m_pXpkMaster;
	CXfdMaster *m_pXfdMaster;
	CXadMaster *m_pXadMaster;

	
protected:
	std::string getCruncherType(CReadBuffer *pInputBuffer);
	void PrepareUnpacker(std::string &subType);

	//bool ForeignDecrunch(XpkProgress *pProgress);
	bool OwnDecrunch(XpkProgress *pProgress);
    
public:
    explicit CLibMaster(QObject *parent = 0);
	virtual ~CLibMaster(void);

	bool xpkInfo(QXpkLib::CArchiveInfo &info);
	
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
