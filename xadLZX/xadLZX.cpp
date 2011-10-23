#include "xadLZX.h"

#include "UnLzx.h"

// (see header)
xadLZX g_Instance;
xadLibraryBase *GetXpkInstance(void)
{
  // TODO: switch to: new lib() when there are members..
	return &g_Instance;
}

xadLZX::xadLZX()
 : xadLibraryBase()
 , m_pArchive(nullptr)
{
	// only to/from file(s) supported here
	m_XpkCaps.input.file = true;
	m_XpkCaps.output.file = true;
	m_XpkCaps.m_LibIdentifier = "LZX";
	m_XpkCaps.m_LibDescription = "Amiga LZX unpacking implementation";
}

xadLZX::~xadLZX()
{
	if (m_pArchive != nullptr)
	{
		delete m_pArchive;
	}
}

// TODO: something like this needed?
virtual bool xadLZX::setArchive(QString &file)
{
	if (m_pArchive != nullptr)
	{
		delete m_pArchive;
	}
	
	try
	{
		m_pArchive = new CUnLzx(file.toStdString());
		return m_pArchive->View(); // get list of contents for later
	}
	catch (std::exception &exp)
	{
		//emit fatal_error(exp.what());
	}
	return false;
}

bool xadLZX::archiveInfo(QXpkLib::CArchiveInfo &info)
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
	info.m_archiveInfo.m_packing = "LZX";
	
	tArchiveEntryList entryList;
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
		CArchiveEntry *pEntry = it->second;
		
		info.m_fileList.push_back(QXpkLib::CEntryInfo());
		QXpkLib::CEntryInfo &entry = info.m_fileList.back();
		
		entry.m_fileName = QString::fromStdString(pEntry->m_szFileName);
		entry.m_ulUnpackedSize = pEntry->m_ulUnpackedSize;
		if (pEntry->m_bPackedSizeAvailable == true) // not merged
		{
			entry.m_ulPackedSize = pEntry->m_ulPackedSize;
		}
		else
		{
			entry.m_ulPackedSize = 0;
		}
		
		if (pEntry->m_PackMode == tLzxArchiveHeader::HDR_PACK_STORE)
		{
			entry.m_packing = "store";
		}
		else if (pEntry->m_PackMode == tLzxArchiveHeader::HDR_PACK_NORMAL)
		{
			if (pEntry->m_bPackedSizeAvailable == false)
			{
				entry.m_packing = "(merged)";
			}
			else
			{
				entry.m_packing = "normal";
			}
		}

		// TODO: other information also (this needs conversion from struct..)
		//entry.m_Stamp = pEntry->m_Timestamp
		
		entry.m_szComment = QString::fromStdString(pEntry->m_szComment);
	
		++it;
	}
	return true;
}

// set path to uncompress files to
bool xadLZX::setExtractPath(QString &szPath)
{
	if (m_pArchive == nullptr)
	{
		return false;
	}
	m_pArchive->SetExtractPath(szPath.toStdString());
	return true;
}

// test archive integrity
bool xadLZX::testArchive()
{
	// not implemented yet
	return false;
}

// unpack/decompress
bool xadLZX::Decrunch(XpkProgress *pProgress)
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

