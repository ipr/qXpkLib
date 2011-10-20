#include "xadARC.h"

// (see header)
xadARC g_Instance;
xadLibraryBase *GetXpkInstance(void)
{
  // TODO: switch to: new lib() when there are members..
	return &g_Instance;
}

xadARC::xadARC()
 : xadLibraryBase()
{
}

xadARC::~xadARC()
{
}

// set path to uncompress files to
bool xadARC::setExtractPath(QString &szPath)
{
	//m_pArchive->SetExtractPath(szPath.toStdString());
	return true;
}

// test archive integrity
bool xadARC::testArchive()
{
	// not implemented yet
	return false;
}

// unpack/decompress
bool xadARC::Decrunch(XpkProgress *pProgress)
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

