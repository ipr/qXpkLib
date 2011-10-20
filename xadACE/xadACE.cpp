#include "xadACE.h"

// (see header)
xadACE g_Instance;
xadLibraryBase *GetXpkInstance(void)
{
  // TODO: switch to: new lib() when there are members..
	return &g_Instance;
}

xadACE::xadACE()
 : xadLibraryBase()
{
}

xadACE::~xadACE()
{
}

// set path to uncompress files to
bool xadACE::setExtractPath(QString &szPath)
{
	//m_pArchive->SetExtractPath(szPath.toStdString());
	return true;
}

// test archive integrity
bool xadACE::testArchive()
{
	// not implemented yet
	return false;
}

// unpack/decompress
bool xadACE::Decrunch(XpkProgress *pProgress)
{
	try
	{
	}
	catch (std::exception &exp) // catch by base-type
	{
		//emit fatal_error(exp.what());
	}
	return false;
}

