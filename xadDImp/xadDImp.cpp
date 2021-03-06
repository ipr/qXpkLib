#include "xadDImp.h"

// (see header)
xadDImp g_Instance;
xadLibraryBase *GetXpkInstance(void)
{
  // TODO: switch to: new lib() when there are members..
	return &g_Instance;
}

xadDImp::xadDImp()
 : xadLibraryBase()
{
	// so far only to/from file(s) supported here,
	// may add more (in progress)
	m_XpkCaps.input.file = true;
	//m_XpkCaps.input.buffer = true;
	m_XpkCaps.output.file = true;
	//m_XpkCaps.output.buffer = true;
	m_XpkCaps.m_LibIdentifier = "DImp";
	m_XpkCaps.m_LibDescription = "Amiga Disk-Imploder unpacking";
}

xadDImp::~xadDImp()
{
}

// unpack/decompress
bool xadDImp::Decrunch(XpkProgress *pProgress)
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

