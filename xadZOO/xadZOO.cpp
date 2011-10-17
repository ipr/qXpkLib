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
{
}

xadZOO::~xadZOO()
{
}

// set path to uncompress files to
bool xadZOO::setExtractPath(QString &szPath)
{
	//m_pArchive->SetExtractPath(szPath.toStdString());
	return true;
}

// test archive integrity
bool xadZOO::testArchive()
{
	// not implemented yet
	return false;
}

// unpack/decompress
bool xadZOO::Decrunch(XpkProgress *pProgress)
{
	CUnZoo unZoo;
	try
	{
		return unZoo.Extract();
	}
	catch (std::exception &exp) // catch by base-type
	{
		//emit fatal_error(exp.what());
	}
	return false;
}

