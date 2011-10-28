#include "xadZOO.h"

#include "ZooStructures.h"
#include "UnZoo.h"

// (see header)
xadZOO g_Instance;
xadLibraryBase *GetXpkInstance(void)
{
  // TODO: switch to: new xpkRLEN() when there are members..
	return &g_Instance;
}


xadZOO::xadZOO()
 : xadLibraryBase()
 , m_pArchive(nullptr)
{
	// only to/from file(s) supported here
	m_XpkCaps.input.file = true;
	m_XpkCaps.output.file = true;
	m_XpkCaps.m_LibIdentifier = "ZOO";
	m_XpkCaps.m_LibDescription = "ZOO uncompressing implementation";
}

xadZOO::~xadZOO()
{
	if (m_pArchive != nullptr)
	{
		delete m_pArchive;
		m_pArchive = nullptr;
	}
}

// TODO:
bool xadZOO::setArchive(QString &file)
{
	if (m_pArchive != nullptr)
	{
		delete m_pArchive;
	}
	
	try
	{
		m_pArchive = new CUnZoo(file.toStdString());
		return m_pArchive->ListContents();
	}
	catch (std::exception &exp) // catch by base-type
	{
		//emit fatal_error(exp.what());
	}
	return false;
}

// list files in archive, get other metadata also..
bool xadZOO::archiveInfo(QXpkLib::CArchiveInfo &info)
{
	if (m_pArchive == nullptr)
	{
		return false;
	}
	
	info.m_bIsMultifile = true;
	info.m_archiveInfo.m_fileName = QString::fromStdString(m_pArchive->GetArchiveFileName());
	info.m_archiveInfo.m_ulFileSize = m_pArchive->GetArchiveFileSize();
	info.m_archiveInfo.m_ulPackedSize = m_pArchive->GetTotalSizePacked();
	info.m_archiveInfo.m_ulUnpackedSize = m_pArchive->GetTotalSizeUnpacked();
	info.m_archiveInfo.m_packing = "ZOO";
	
	ZooDescription *pDesc = m_pArchive->GetArchiveInfo();
	info.m_archiveInfo.m_szComment = QString::fromStdString(pDesc->comment);

	tEntryList entryList;
	if (m_pArchive->GetEntryList(entryList) == false)
	{
		return false;
	}

	auto it = entryList.begin();
	auto itEnd = entryList.end();
	while (it != itEnd)
	{
		// must make copy of information
		// from internal format for eventual user..
		//
		ZooEntry *pEntry = it->second;
		
		info.m_fileList.push_back(QXpkLib::CEntryInfo());
		QXpkLib::CEntryInfo &entry = info.m_fileList.back();
		
		entry.m_fileName = QString::fromStdString(pEntry->getName());
		entry.m_Stamp = pEntry->timestamp; // use time_t directly, converted already
		entry.m_ulUnpackedSize = pEntry->original_size;
		entry.m_ulPackedSize = pEntry->compressed_size;
		
		if (pEntry->method == PackCopyOnly)
		{
			entry.m_packing = "copy only";
		}
		else if (pEntry->method == PackLzd)
		{
			entry.m_packing = "Lzd";
		}
		else if (pEntry->method == PackLzh)
		{
			entry.m_packing = "Lzh";
		}

		entry.m_szComment = QString::fromStdString(pEntry->comment);
	
		++it;
	}
	return true;
}

// set path to uncompress files to
bool xadZOO::setExtractPath(QString &szPath)
{
	if (m_pArchive == nullptr)
	{
		return false;
	}

	return m_pArchive->SetExtractPath(szPath.toStdString());
}

// test archive integrity
bool xadZOO::testArchive()
{
	if (m_pArchive == nullptr)
	{
		return false;
	}

	try
	{
		return m_pArchive->TestArchive();
	}
	catch (std::exception &exp) // catch by base-type
	{
		//emit fatal_error(exp.what());
	}
	return false;
}

// unpack/decompress
bool xadZOO::Decrunch(XpkProgress *pProgress)
{
	if (m_pArchive == nullptr)
	{
		return false;
	}

	try
	{
		return m_pArchive->Extract();
	}
	catch (std::exception &exp) // catch by base-type
	{
		//emit fatal_error(exp.what());
	}
	return false;
}

