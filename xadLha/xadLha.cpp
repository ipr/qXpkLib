#include "xadLha.h"

#include "LhArchive.h"

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
}

xadLha::~xadLha()
{
	if (m_pArchive != nullptr)
	{
		delete m_pArchive;
	}
}

// TODO: something like this needed?
void xadLha::setArchive(QString &file)
{
	m_pArchive = new CLhArchive(this, file);
}

/*
bool xadLha::archiveInfo(QXpkLib::CArchiveInfo &info)
{
	//m_pArchive->getHeaderList();
}
*/

// set path to uncompress files to
bool xadLha::setExtractPath(QString &szPath)
{
	m_pArchive->SetExtractPath(szPath.toStdString());
	return true;
}

// test archive integrity
bool xadLha::testArchive()
{
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

