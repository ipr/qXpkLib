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
#include <QDateTime>
#include <QList>


//// fwd. decl.
class CLibMaster;

class QXPKLIBSHARED_EXPORT QXpkLib : public QObject
{
	Q_OBJECT

private:
	CLibMaster *m_pMaster;
	
protected:
	void PrepareMaster();
	
public:
    QXpkLib(QObject *parent = 0);
	virtual ~QXpkLib();

	// info to caller for each file
	// included in compressed file (archive)
	//
	class CEntryInfo
	{
	public:
		CEntryInfo() 
			: m_fileName()
			, m_Stamp()
			, m_ulFileSize(0)
			, m_ulPackedSize(0)
			, m_ulUnpackedSize(0)
			, m_packing()
			, m_szComment()
		{}
		
		QString m_fileName; // name&path of file

		// "last modified" time usually
		QDateTime m_Stamp;

		// for entry packed to archive, packed size should be same as file size,
		// for archive itself, metadata has overhead over actual data size.
		// -> does it matter to us/user at all?
		//
		unsigned long m_ulFileSize; // actual file size (inc. metadata)
		unsigned long m_ulPackedSize; // size of compressed data
		unsigned long m_ulUnpackedSize; // uncompressed data size
		
		// file cruncher/packing type,
		// may have different methods per-file
		QString m_packing;
		
		// file-related comment from archive (if any)
		// (usually in Amiga-packed files)
		QString m_szComment;
		
		// if merged-entry in archive (LZX)
		// or spanning multiple volumes (ACE,RAR)
		// for when single-file extraction cannot be done?
		//bool m_bIsPartial;
		
		// if entry versioning is supported (ZOO),
		// the version of the entry ?
		//m_version
		
		// other contained file metadata?
		//filetype (module/image/executable..?);
	};
	
	// general information on archive/compressed file
	// (metadata such as cruncher, packing mode, list of files etc.)
	//
	class CArchiveInfo
	{
	public:
		CArchiveInfo() 
			: m_bIsMultifile(false)
			, m_bIsMultiVolume(false)
			, m_archiveInfo()
			, m_fileList()
		{}
		
		// for archive-files, may be larger than 1
		// for single-file packers, at most 1
		//unsigned long m_ulFileCount; // files in archive
		
		// if is multifile-archive (collection of files):
		// may or may not have information on contained file(s)
		bool m_bIsMultifile;
		
		// file is part of multi-volume archive
		// (collection of archive-files, each with collection of files):
		// not many archivers that have this ??
		bool m_bIsMultiVolume;
		
		// for convenience of single-file crunching only?
		// (obscure XFD-cruncher might not have any metadata..)
		CEntryInfo m_archiveInfo;

		// for each contained file 
		// when multi-file and information available
		//
		QList<CEntryInfo> m_fileList;
		
		// TODO: something like this for linking
		// multi-volume archive files ?
		// (need to access each part in sequence..)
		//
		//CArchiveInfo *m_prev;
		//CArchiveInfo *m_next;
	};
	
	
	// get information on selected file
	// (compression, type etc.)
	bool xpkInfo(QXpkLib::CArchiveInfo &info);

	// pack/unpack to/from given input&output
	bool xpkPack();
	bool xpkUnpack();

	// if user wants result to buffer? 
	void getToBuffer(QByteArray &Array);

	// if so, selection needed for multi-file support..
	void getToBuffer(QString &entry, QByteArray &Array);
	
signals:
	void message(QString);
	void warning(QString);
	void error(QString);

	//void status(type?);
	
public slots:
	
	// set filename and path
	void setInputFile(QString &szFile);
	void setOutputFile(QString &szFile);

	// for multi-file archive extraction
	void setOutputPath(QString &szPath);
};

#endif // QXPKLIB_H
