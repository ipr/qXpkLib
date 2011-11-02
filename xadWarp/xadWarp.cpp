#include "xadWarp.h"

#include "UnWarp.h"

// (see header)
xadWarp g_Instance;
xadLibraryBase *GetXpkInstance(void)
{
  // TODO: switch to: new lib() when there are members..
	return &g_Instance;
}

xadWarp::xadWarp()
 : xadLibraryBase()
{
	// so far only to/from file(s) supported here,
	// may add more (in progress)
	m_XpkCaps.input.file = true;
	//m_XpkCaps.input.buffer = true;
	m_XpkCaps.output.file = true;
	//m_XpkCaps.output.buffer = true;
	m_XpkCaps.m_LibIdentifier = "Warp";
	m_XpkCaps.m_LibDescription = "Warp disk-image unpacking";
}

xadWarp::~xadWarp()
{
}

// unpack/decompress
bool xadWarp::Decrunch(XpkProgress *pProgress)
{
	CUnWarp unWarp(pProgress->pInputBuffer, pProgress->pOutputBuffer);
	try
	{
		// TODO: set final output size to progress
		// for writing correct amount..
		return unWarp.unpack();
	}
	catch (std::exception &exp) // catch by base-type
	{
		//emit fatal_error(exp.what());
	}
	return false;
}

