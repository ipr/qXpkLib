#include "xfdVice.h"

// from master-project for buffer-class
#include "AnsiFile.h"

// from master-project (fulfill fwd. decl.)
#include "XpkProgress.h"

// use decrunching helper for actual decoding of data
#include "Vice.h"

//TODO: when ready, we can add tracking of created instances
// (when library needs members per-user..)

// (see header)
xfdVice g_Instance;
xfdLibraryBase *GetXpkInstance(void)
{
  // TODO: switch to: new xpkRLEN() when there are members..
	return &g_Instance;
}

xfdVice::xfdVice()
	: xfdLibraryBase()
{
	// only to/from buffer supported here
	m_XpkCaps.input.buffer = true;
	m_XpkCaps.output.buffer = true;
	m_XpkCaps.m_LibIdentifier = "Vice"; //
	m_XpkCaps.m_LibDescription = "Vice decrunching";
}

xfdVice::~xfdVice()
{
}

bool xfdVice::isSupported(CReadBuffer *pInputBuffer)
{
	if (::memcmp(pInputBuffer->GetBegin(), "Vice", 4) == 0)
	{
		// Vice crunched file
		return true;
	}
	return false;
}

// decrunching (unpacking) only supported for this format
bool xfdVice::Decrunch(XpkProgress *pProgress)
{
	// only whole file decompression for now..
	if (pProgress->xp_chunkIn < pProgress->xp_WholePackedFileSize)
	{
		return false;
	}
	
	try
	{
		// create decruncher for unpacking
		// (tODO: keep somewhere..)
		Vice *pSlave = new Vice(pProgress->pInputBuffer);
		bool bRes = pSlave->decrunch(pProgress->pOutputBuffer);
		if (bRes == true)
		{
			// need to update status for master-library
			// so it knows how much of data is in buffer..
			//pProgress->xp_chunkOut = ;
		}
		return bRes;
	}
	catch (std::exception &exp) // catch by base-type
	{
		// debug..
		//std::cerr << "Exception: " << exp.what() << std::end;
	}
	return false;
}

