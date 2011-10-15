//////////////////////////////////////////////
//
// CLhArchive: code to handle LHA archive-file "commands":
// - list
// - extract (variations)
// - test
//
// Ilkka Prusi 2011
//

#include "LhArchive.h"

CLhArchive::CLhArchive(QLhALib *pParent, QString &szArchive)
	: QObject(pParent),
	m_szArchive(szArchive),
	m_nFileSize(0),
	m_ulTotalPacked(0),
	m_ulTotalUnpacked(0),
	m_ulTotalFiles(0),
	m_uiCrc(0),
	m_pHeaders(nullptr),
	m_pExtraction(nullptr)
{
	m_pHeaders = new CLhHeader(this);
	m_pExtraction = new CLhExtract(this);
	
	connect(m_pHeaders, SIGNAL(message(QString)), this, SIGNAL(message(QString)));
	connect(m_pHeaders, SIGNAL(warning(QString)), this, SIGNAL(warning(QString)));
	
	connect(m_pExtraction, SIGNAL(message(QString)), this, SIGNAL(message(QString)));
	connect(m_pExtraction, SIGNAL(warning(QString)), this, SIGNAL(warning(QString)));
}

CLhArchive::~CLhArchive(void)
{
	// TODO: 
	// don't destroy that which is destroyed by parent?
	
	if (m_pExtraction != nullptr)
	{
		delete m_pExtraction;
	}
	if (m_pHeaders != nullptr)
	{
		delete m_pHeaders;
	}
}

/////////////// protected methods

// in case same instance is used again (same or different archive)
// -> clear some added data
void CLhArchive::Clear()
{
	// same instance, called again?
	// (TODO: add better checks if info exists and is correct..)
	m_pHeaders->Clear();
	m_ulTotalPacked = 0;
	m_ulTotalUnpacked = 0;
	m_ulTotalFiles = 0;
	m_uiCrc = 0;
}

void CLhArchive::SeekHeader(CAnsiFile &ArchiveFile)
{
	//size_t nMaxSeekSize = 64 * 1024; // max seek size
	size_t nMaxSeekSize = 4096; // max seek size
	size_t nFileSize = ArchiveFile.GetSize();

	// should have at least 16 bytes for a header to exist..
	//
	if (m_nFileSize < 16)
	{
		throw ArcException("File too small", m_szArchive.toStdString());
	}

	// check if we have smaller file than expected
	// -> don't try reading more than we have
	if (nFileSize < nMaxSeekSize)
	{
		nMaxSeekSize = nFileSize;
	}
	
	CReadBuffer Buffer(nMaxSeekSize);
	if (ArchiveFile.Read(Buffer) == false)
	{
		throw IOException("Failed reading header");
	}
	
	if (m_pHeaders->IsValidLha(Buffer) == false)
	{
		throw ArcException("No supported header in file", m_szArchive.toStdString());
	}
	
	// just seek start.. 
	if (ArchiveFile.Seek(0, SEEK_SET) == false)
	{
		throw IOException("Failed seeking start");
	}
}

void CLhArchive::SeekContents(CAnsiFile &ArchiveFile)
{
	if (ArchiveFile.Open(m_szArchive.toStdString()) == false)
	{
		throw IOException("Failed opening archive");
	}
	m_nFileSize = ArchiveFile.GetSize();

	// check&parse archive info so we know 
	// how to extract files..
	//
	SeekHeader(ArchiveFile);
	
	// list file-headers (contents)
	// from the archive-file
	//
	m_pHeaders->ParseHeaders(ArchiveFile);
}


/////////////// public slots

void CLhArchive::SetConversionCodec(QTextCodec *pCodec)
{
	m_pHeaders->SetConversionCodec(pCodec);
}

void CLhArchive::SetExtractPath(QString &szExtractPath)
{
	m_pExtraction->SetExtractPath(szExtractPath);
}

bool CLhArchive::Extract()
{
	// same instance, called again
	// TODO: need better way to check when reopening same (unchanged) file
	Clear();
	
	// lookup each entry of file
	CAnsiFile ArchiveFile;

	// open and list contents
	SeekContents(ArchiveFile);

	// make user-given path where to extract (may be empty)
	CPathHelper::MakePath(m_pExtraction->GetExtractPath().toStdString());

	
	// decode and write each file from archive
	//
	auto it = m_pHeaders->m_HeaderList.begin();
	auto itEnd = m_pHeaders->m_HeaderList.end();
	while (it != itEnd)
	{
		LzHeader *pHeader = (*it);

		emit message(QString("Extracting.. ").append(pHeader->filename));
		
		// check path-ending&combine
		QString szTempPath = m_pExtraction->GetExtractPathToFile(pHeader->filename);
		
		// if it's directory-entry -> nothing more to do here
		// (usually has -lhd- compression method for "store only"?)
		// check packmethod also in case unix modeflags are not given?
		if (pHeader->isDir())
		{
			// make directory only
			CPathHelper::MakePath(szTempPath.toStdString());
			
			++it;
			continue;
		}
		
		// create path to file
		CPathHelper::MakePathToFile(szTempPath.toStdString());

		// decode from archive to output..
		// give parsed metadata and prepared output also
		//
		m_pExtraction->ToFile(ArchiveFile, pHeader);
		
		++it;
	}
	
	return true;
}

// extract listed files from archive to disk
bool CLhArchive::ExtractSelected(QStringList &lstFiles)
{
	if (lstFiles.isEmpty())
	{
		emit warning(QString("No file(s) selected, nothing to extract.."));
		return false;
	}

	// same instance, called again
	// TODO: need better way to check when reopening same (unchanged) file
	Clear();
	
	// lookup each entry of file
	CAnsiFile ArchiveFile;

	// open and list contents
	SeekContents(ArchiveFile);

	// make user-given path where to extract (may be empty)
	CPathHelper::MakePath(m_pExtraction->GetExtractPath().toStdString());
	
	auto it = m_pHeaders->m_HeaderList.begin();
	auto itEnd = m_pHeaders->m_HeaderList.end();
	while (it != itEnd)
	{
		LzHeader *pHeader = (*it);
		if (lstFiles.contains(pHeader->filename) == false)
		{
			// skipping file, not included in user selection..
			++it;
			continue;
		}

		emit message(QString("Extracting.. ").append(pHeader->filename));
		
		// check path-ending&combine
		QString szTempPath = m_pExtraction->GetExtractPathToFile(pHeader->filename);
		
		// if it's directory-entry -> nothing more to do here
		// (usually has -lhd- compression method for "store only"?)
		// check packmethod also in case unix modeflags are not given?
		if (pHeader->isDir())
		{
			// make directory only
			CPathHelper::MakePath(szTempPath.toStdString());
			
			++it;
			continue;
		}
		
		// create path to file
		CPathHelper::MakePathToFile(szTempPath.toStdString());

		// decode from archive to output..
		// give parsed metadata and prepared output also
		//
		m_pExtraction->ToFile(ArchiveFile, pHeader);
		
		++it;
	}
	
	return true;
}

// extract single file from archive to user-buffer
bool CLhArchive::ExtractToUserBuffer(QString &szFileEntry, QByteArray &outArray)
{
	// same instance, called again
	// TODO: need better way to check when reopening same (unchanged) file
	Clear();
	
	// lookup each entry of file
	CAnsiFile ArchiveFile;

	// open and list contents
	SeekContents(ArchiveFile);

	auto it = m_pHeaders->m_HeaderList.begin();
	auto itEnd = m_pHeaders->m_HeaderList.end();
	while (it != itEnd)
	{
		LzHeader *pHeader = (*it);
		
		if (pHeader->isDir())
		{
			if (pHeader->filename == szFileEntry)
			{
				emit warning(QString("Wanted file %1 is a directory").arg(szFileEntry));
				return false;
			}
			++it;
			continue;
		}
	
		// is wanted file?
		if (pHeader->filename == szFileEntry)
		{
			m_pExtraction->ToUserBuffer(ArchiveFile, pHeader, outArray);
			return true;
		}
		
		++it;
	}
	
	// throw exception instead? (user-input was crap -> not our fault)
	emit warning(QString("File %1 was not found in archive").arg(szFileEntry));
	return false;
}

bool CLhArchive::List(QLhALib::tArchiveEntryList &lstArchiveInfo)
{
	// same instance, called again
	// TODO: need better way to check when reopening same (unchanged) file
	Clear();

	// auto-close file (on leaving scope),
	// wrap some handling
	CAnsiFile ArchiveFile;
	
	// lookup each entry of file,
	// throws exception on error
	SeekContents(ArchiveFile);
	
	// information to caller
	auto it = m_pHeaders->m_HeaderList.begin();
	auto itEnd = m_pHeaders->m_HeaderList.end();
	while (it != itEnd)
	{
		LzHeader *pHeader = (*it);
		
		// if it's directory-entry -> nothing more to do here
		if (pHeader->isDir())
		{
			++it;
			continue;
		}
		
		lstArchiveInfo.push_back(QLhALib::CArchiveEntry());
		
		QLhALib::CArchiveEntry &Entry = lstArchiveInfo.back();
		Entry.m_szFileName = pHeader->filename;
		Entry.m_uiCrc = pHeader->getFileCrc();
		Entry.m_ucHeaderLevel = pHeader->header_level;
		Entry.m_ulPackedSize = pHeader->packed_size;
		Entry.m_ulUnpackedSize = pHeader->original_size;
		Entry.m_szPackMode = pHeader->pack_method;
		Entry.m_Stamp = pHeader->last_modified_stamp;
		Entry.m_extendType = pHeader->GetOSTypeName();
		Entry.m_szComment = pHeader->file_comment;
		
		if (pHeader->os_type == EXTEND_UNIX)
		{
			LzUnixHeader *pUxh = pHeader->GetUnixHeader();
			Entry.m_szUser = pUxh->unix_user;
			Entry.m_szGroup = pUxh->unix_group;
			Entry.m_unix_uid = pUxh->unix_uid;
			Entry.m_unix_gid = pUxh->unix_gid;
		}
		
		// file attributes/protection flags? which ones?
		// unix? msdos? (amiga?) 
		// -> depends on where packed..?
		// -> give all supported..?
		
		// update archive statistics
		m_ulTotalPacked += pHeader->packed_size;
		m_ulTotalUnpacked += pHeader->original_size;
		m_ulTotalFiles++;
		
		++it;
	}
	
	return true;
}

// test archive without writing out
// (simulate extraction)
bool CLhArchive::Test()
{
	// same instance, called again
	// TODO: need better way to check when reopening same (unchanged) file
	Clear();
	
	// lookup each entry of file
	CAnsiFile ArchiveFile;

	// open and list contents
	SeekContents(ArchiveFile);

	// decode each file from archive
	//
	auto it = m_pHeaders->m_HeaderList.begin();
	auto itEnd = m_pHeaders->m_HeaderList.end();
	while (it != itEnd)
	{
		LzHeader *pHeader = (*it);

		emit message(QString("Testing.. ").append(pHeader->filename));
		
		// if it's directory-entry -> nothing more to do here
		// (usually has -lhd- compression method for "store only"?)
		if (pHeader->isDir())
		{
			++it;
			continue;
		}
		
		// decode from archive to output..
		// give parsed metadata and prepared output also
		//
		m_pExtraction->Test(ArchiveFile, pHeader);
		
		++it;
	}
	
	return true;
}

