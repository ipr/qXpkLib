#include "xadStuffIt.h"

#include "UnStuffIt.h"

// (see header)
xadStuffIt g_Instance;
xadLibraryBase *GetXpkInstance(void)
{
  // TODO: switch to: new lib() when there are members..
	return &g_Instance;
}

xadStuffIt::xadStuffIt()
 : xadLibraryBase()
 , m_pArchive(nullptr)
{
	// so far only to/from file(s) supported here,
	// may add more (in progress)
	m_XpkCaps.input.file = true;
	m_XpkCaps.output.file = true;
	m_XpkCaps.m_LibIdentifier = "StuffIt";
	m_XpkCaps.m_LibDescription = "Mac StuffIt unpacking/extracting";
}

xadStuffIt::~xadStuffIt()
{
	if (m_pArchive != nullptr)
	{
		delete m_pArchive;
		m_pArchive = nullptr;
	}
}

bool xadStuffIt::setArchive(QString &file)
{
	if (m_pArchive != nullptr)
	{
		delete m_pArchive;
	}
	m_pArchive = new CUnStuffIt(file.toStdString());
	return m_pArchive->List();
}

bool xadStuffIt::archiveInfo(QXpkLib::CArchiveInfo &info)
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
	info.m_archiveInfo.m_packing = "StuffIt";
	
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
bool xadStuffIt::setExtractPath(QString &szPath)
{
	if (m_pArchive == nullptr)
	{
		return false;
	}
	return m_pArchive->SetExtractPath(szPath.toStdString());
}

// test archive integrity
bool xadStuffIt::testArchive()
{
	if (m_pArchive == nullptr)
	{
		return false;
	}

	// not implemented yet
	return m_pArchive->TestArchive();
}

// unpack/decompress
bool xadStuffIt::Decrunch(XpkProgress *pProgress)
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

