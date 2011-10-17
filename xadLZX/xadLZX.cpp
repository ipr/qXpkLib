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
	//m_pArchive = new CUnLzx(szArchive.toStdString());
}

xadLZX::~xadLZX()
{
	if (m_pArchive != nullptr)
	{
		delete m_pArchive;
	}
}

// set path to uncompress files to
bool xadLZX::setExtractPath(QString &szPath)
{
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

