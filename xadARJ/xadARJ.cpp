#include "xadARJ.h"

#include "UnARJ.h"

// (see header)
xadARJ g_Instance;
xadLibraryBase *GetXpkInstance(void)
{
  // TODO: switch to: new lib() when there are members..
	return &g_Instance;
}

xadARJ::xadARJ()
 : xadLibraryBase()
 , m_pArchive(nullptr)
{
	// so far only to/from file(s) supported here,
	// may add more (in progress)
	m_XpkCaps.input.file = true;
	m_XpkCaps.output.file = true;
	m_XpkCaps.m_LibIdentifier = "ARJ";
	m_XpkCaps.m_LibDescription = "ARJ unpacking";
}

xadARJ::~xadARJ()
{
	if (m_pArchive != nullptr)
	{
		delete m_pArchive;
		m_pArchive = nullptr;
	}
}

bool xadARJ::setArchive(QString &file)
{
	if (m_pArchive != nullptr)
	{
		delete m_pArchive;
	}
	m_pArchive = new CUnARJ(file.toStdString());
	return m_pArchive->List();
}

bool xadARJ::archiveInfo(QXpkLib::CArchiveInfo &info)
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
	info.m_archiveInfo.m_packing = "ARJ";
	
	tArchiveEntryList entryList;
	m_pArchive->GetEntryList(entryList);
	
	auto it = entryList.begin();
	auto itEnd = entryList.end();
	while (it != itEnd)
	{
		//AceEntry *pEntry = (*it);

		info.m_fileList.push_back(QXpkLib::CEntryInfo());
		QXpkLib::CEntryInfo &entry = info.m_fileList.back();
		
		// TODO: contents to caller info..
		// 
		//entry.m_fileName = pEntry->fileName;
	
		++it;
	}
	return true;
}

// set path to uncompress files to
bool xadARJ::setExtractPath(QString &szPath)
{
	if (m_pArchive == nullptr)
	{
		return false;
	}
	return m_pArchive->SetExtractPath(szPath.toStdString());
}

// test archive integrity
bool xadARJ::testArchive()
{
	if (m_pArchive == nullptr)
	{
		return false;
	}

	// not implemented yet
	return m_pArchive->TestArchive();
}

// unpack/decompress
bool xadARJ::Decrunch(XpkProgress *pProgress)
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

