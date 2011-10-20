#include "xadARJ.h"

// (see header)
xadARJ g_Instance;
xadLibraryBase *GetXpkInstance(void)
{
  // TODO: switch to: new lib() when there are members..
	return &g_Instance;
}

xadARJ::xadARJ()
 : xadLibraryBase()
{
}

xadARJ::~xadARJ()
{
}

// set path to uncompress files to
bool xadARJ::setExtractPath(QString &szPath)
{
	//m_pArchive->SetExtractPath(szPath.toStdString());
	return true;
}

// test archive integrity
bool xadARJ::testArchive()
{
	// not implemented yet
	return false;
}

// unpack/decompress
bool xadARJ::Decrunch(XpkProgress *pProgress)
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

