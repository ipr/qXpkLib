#include "xadCPIO.h"

// (see header)
xadCPIO g_Instance;
xadLibraryBase *GetXpkInstance(void)
{
  // TODO: switch to: new lib() when there are members..
	return &g_Instance;
}

xadCPIO::xadCPIO()
 : xadLibraryBase()
 //, m_pArchive(nullptr)
{
	// so far only to/from file(s) supported here,
	// may add more (in progress)
	m_XpkCaps.input.file = true;
	m_XpkCaps.output.file = true;
	m_XpkCaps.m_LibIdentifier = "CPIO";
	m_XpkCaps.m_LibDescription = "Un*x CPIO unpacking implementation";
}

xadCPIO::~xadCPIO()
{
	if (m_pArchive != nullptr)
	{
		delete m_pArchive;
		m_pArchive = nullptr;
	}
}

bool xadCPIO::setArchive(QString &file)
{
	if (m_pArchive != nullptr)
	{
		delete m_pArchive;
	}
	//return m_pArchive->List();
}

bool xadCPIO::archiveInfo(QXpkLib::CArchiveInfo &info)
{
	if (m_pArchive == nullptr)
	{
		return false;
	}
	
	return true;
}

// set path to uncompress files to
bool xadCPIO::setExtractPath(QString &szPath)
{
	if (m_pArchive == nullptr)
	{
		return false;
	}
	//return m_pArchive->SetExtractPath(szPath.toStdString());
}

// test archive integrity
bool xadCPIO::testArchive()
{
	if (m_pArchive == nullptr)
	{
		return false;
	}

	// not implemented yet
	//return m_pArchive->TestArchive();
}

// unpack/decompress
bool xadCPIO::Decrunch(XpkProgress *pProgress)
{
	if (m_pArchive == nullptr)
	{
		return false;
	}

	try
	{
		//return m_pArchive->Extract();
	}
	catch (std::exception &exp) // catch by base-type
	{
		//emit fatal_error(exp.what());
	}
	return false;
}

