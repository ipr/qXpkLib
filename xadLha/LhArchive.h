//////////////////////////////////////////////
//
// CLhArchive: code to handle LHA archive-file "commands":
// - list
// - extract (variations)
// - test
//
// Ilkka Prusi 2011
//

#ifndef LHARCHIVE_H
#define LHARCHIVE_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QTextCodec>

// use typedefs from parent
//#include <qlhalib.h>

#include "AnsiFile.h"
#include "LhaTypeDefs.h"
#include "LhHeader.h"
#include "LhExtract.h"


class CLhArchive : public QObject
{
	Q_OBJECT

public:
    CLhArchive(QObject *parent, QString &szArchive);
	virtual ~CLhArchive(void);
	
protected:
	QString m_szArchive;
	size_t m_nFileSize;

	// some statistics of archive
	unsigned long m_ulTotalPacked;
	unsigned long m_ulTotalUnpacked;
	unsigned long m_ulTotalFiles;
	
	// archive-file crc ?
	unsigned int m_uiCrc;

	// descriptions (headers) of each entry in archive
	CLhHeader *m_pHeaders;
	CLhExtract *m_pExtraction;
	
	void Clear();
	
	void SeekHeader(CAnsiFile &ArchiveFile);
	void SeekContents(CAnsiFile &ArchiveFile);
	
public slots:
	
	void SetConversionCodec(QTextCodec *pCodec);
	void SetExtractPath(QString &szExtractPath);
	
	// extract all files to specified path
	bool Extract();

	// extract only specified files to specified path
	bool ExtractSelected(QStringList &lstFiles);
	
	// extract single file from archive to user-buffer
	bool ExtractToUserBuffer(QString &szFileEntry, QByteArray &outArray);

	// list contents in archive
	bool List(QLhALib::tArchiveEntryList &lstArchiveInfo);

	// test archive integrity
	bool Test();

	
signals:
	// progress-status by signals, errors by exceptions
	void message(QString);
	void warning(QString);

public:
	// information about archive file itself
	QString GetArchiveFileName() const
	{
		return m_szArchive;
	}
	size_t GetArchiveFileSize() const
	{
		return m_nFileSize;
	}

	// statistical information access to caller
	unsigned long GetTotalSizeUnpacked() const
	{ 
		return m_ulTotalUnpacked; 
	}
	unsigned long GetTotalSizePacked() const
	{ 
		return m_ulTotalPacked; 
	}
	unsigned long GetTotalFileCount() const
	{ 
		return m_ulTotalFiles; 
	}
};

#endif // LHARCHIVE_H

