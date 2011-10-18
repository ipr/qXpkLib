#include "xadZOO.h"

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
	m_pArchive = new CUnZoo(file.toStdString());
	return m_pArchive->ListContents();
}

// list files in archive, get other metadata also..
bool xadZOO::archiveInfo(QXpkLib::CArchiveInfo &info)
{
	if (m_pArchive == nullptr)
	{
		return false;
	}

	//m_pArchive->GetEntryList();
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

	return m_pArchive->TestArchive();
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

