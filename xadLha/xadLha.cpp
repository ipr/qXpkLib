#include "xadLha.h"

#include "LhArchive.h"
#include "LhHeader.h"


// (see header)
xadLha g_Instance;
xadLibraryBase *GetXpkInstance(void)
{
  // TODO: switch to: new lib() when there are members..
	return &g_Instance;
}

xadLha::xadLha()
 : xadLibraryBase()
 , m_pArchive(nullptr)
{
	// only from file supported here
	m_XpkCaps.input.file = true;
	
	// output (optionally) to buffer
	m_XpkCaps.output.file = true;
	m_XpkCaps.output.buffer = true;
	
	m_XpkCaps.m_LibIdentifier = "LhA";
	m_XpkCaps.m_LibDescription = "LhA unpacking implementation";
}

xadLha::~xadLha()
{
	if (m_pArchive != nullptr)
	{
		delete m_pArchive;
		m_pArchive = nullptr;
	}
}

// TODO: something like this needed?
bool xadLha::setArchive(QString &file)
{
	if (m_pArchive == nullptr)
	{
		delete m_pArchive;
	}
	m_pArchive = new CLhArchive(file);
	return true;
}

bool xadLha::archiveInfo(QXpkLib::CArchiveInfo &info)
{
	if (m_pArchive == nullptr)
	{
		return false;
	}
	
	QList<LzHeader*> lstArchiveInfo;
	m_pArchive->List(lstArchiveInfo);
	
	info.m_bIsMultifile = true;
	info.m_archiveInfo.m_fileName = m_pArchive->GetArchiveFileName();
	info.m_archiveInfo.m_ulFileSize = m_pArchive->GetArchiveFileSize();
	info.m_archiveInfo.m_packing = "LhA";
	
	auto it = lstArchiveInfo.begin();
	auto itEnd = lstArchiveInfo.end();
	while (it != itEnd)
	{
		LzHeader *pHeader = (*it);
		
		// if it's directory-entry -> nothing more to do here
		if (pHeader->isDir())
		{
			++it;
			continue;
		}
		
		info.m_fileList.push_back(QXpkLib::CEntryInfo());
		
		QXpkLib::CEntryInfo &Entry = info.m_fileList.back();
		Entry.m_fileName = pHeader->filename;
		Entry.m_ulPackedSize = pHeader->packed_size;
		Entry.m_ulUnpackedSize = pHeader->original_size;
		Entry.m_packing = pHeader->pack_method;
		Entry.m_Stamp = pHeader->last_modified_stamp;
		Entry.m_szComment = pHeader->file_comment;
		
		// update archive statistics
		info.m_archiveInfo.m_ulPackedSize += pHeader->packed_size;
		info.m_archiveInfo.m_ulUnpackedSize += pHeader->original_size;
		
		++it;
	}
	
	return true;	
}

// set path to uncompress files to
bool xadLha::setExtractPath(QString &szPath)
{
	if (m_pArchive == nullptr)
	{
		return false;
	}

	m_pArchive->SetExtractPath(szPath);
	return true;
}

// test archive integrity
bool xadLha::testArchive()
{
	if (m_pArchive == nullptr)
	{
		return false;
	}

	try
	{
		return m_pArchive->Test();
	}
	catch (std::exception &exp)
	{
		//emit fatal_error(exp.what());
	}
	return false;
}

// unpack/decompress
bool xadLha::Decrunch(XpkProgress *pProgress)
{
	if (m_pArchive == nullptr)
	{
		return false;
	}

	try
	{
		return m_pArchive->Extract();
	}
	catch (std::exception &exp)
	{
		//emit fatal_error(exp.what());
	}
	return false;
}

