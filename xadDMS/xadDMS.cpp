#include "xadDMS.h"

#include "UnDMS.h"

// from master-project (fulfill fwd. decl.)
#include "XpkProgress.h"
#include "IoContext.h"

// (see header)
xadDMS g_Instance;
xadLibraryBase *GetXpkInstance(void)
{
  // TODO: switch to: new lib() when there are members..
	return &g_Instance;
}

xadDMS::xadDMS()
 : xadLibraryBase()
{
	// only whole file decrunching
	// -> master-library may read/map to memory for us
	m_XpkCaps.whole_file_only = true;
	
	// so far only to/from file(s) supported here,
	// may add more (in progress)
	m_XpkCaps.input.file = true;
	//m_XpkCaps.input.buffer = true;
	m_XpkCaps.output.file = true;
	//m_XpkCaps.output.buffer = true;
	m_XpkCaps.m_LibIdentifier = "DMS";
	m_XpkCaps.m_LibDescription = "DMS disk-image unpacking";
}

xadDMS::~xadDMS()
{
}

// unpack/decompress
bool xadDMS::Decrunch(XpkProgress *pProgress)
{
	CUnDMS unDms(pProgress->pInputBuffer, pProgress->pOutputBuffer);
	try
	{
		// TODO: set final output size to progress
		// for writing correct amount..
		return unDms.unpack();
	}
	catch (std::exception &exp) // catch by base-type
	{
		//emit fatal_error(exp.what());
	}
	return false;
}

