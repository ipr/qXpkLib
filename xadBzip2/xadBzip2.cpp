#include "xadBzip2.h"

// (see header)
xadBzip2 g_Instance;
xadLibraryBase *GetXpkInstance(void)
{
  // TODO: switch to: new xpkRLEN() when there are members..
	return &g_Instance;
}

xadBzip2::xadBzip2()
 : xadLibraryBase()
{
}

xadBzip2::~xadBzip2()
{
}

// set path to uncompress files to
bool xadBzip2::setExtractPath(QString &szPath)
{
	//m_pArchive->SetExtractPath(szPath.toStdString());
	return true;
}

// test archive integrity
bool xadBzip2::testArchive()
{
	// not implemented yet
	return false;
}

// unpack/decompress
bool xadBzip2::Decrunch(XpkProgress *pProgress)
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

