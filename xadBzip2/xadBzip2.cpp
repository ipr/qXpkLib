#include "xadBzip2.h"

#include "CBzip2.h"

// (see header)
xadBzip2 g_Instance;
xadLibraryBase *GetXpkInstance(void)
{
  // TODO: switch to: new lib() when there are members..
	return &g_Instance;
}

xadBzip2::xadBzip2()
 : xadLibraryBase()
 , m_pArchive(nullptr)
{
}

xadBzip2::~xadBzip2()
{
	if (m_pArchive != nullptr)
	{
		delete m_pArchive;
		m_pArchive = nullptr;
	}
}

bool xadBzip2::setArchive(QString &szArchive)
{
	if (m_pArchive != nullptr)
	{
		delete m_pArchive;
	}
	m_pArchive = new CBzip2(szArchive);
	return true;
}


// set path to uncompress files to
bool xadBzip2::setExtractPath(QString &szPath)
{
	if (m_pArchive == nullptr)
	{
		return false;
	}

	//m_pArchive->SetExtractPath(szPath.toStdString());
	return true;
}

// test archive integrity
bool xadBzip2::testArchive()
{
	if (m_pArchive == nullptr)
	{
		return false;
	}

	// not implemented yet
	return false;
}

// unpack/decompress
bool xadBzip2::Decrunch(XpkProgress *pProgress)
{
	if (m_pArchive == nullptr)
	{
		return false;
	}

	try
	{
	}
	catch (std::exception &exp) // catch by base-type
	{
		//emit fatal_error(exp.what());
	}
	return false;
}

