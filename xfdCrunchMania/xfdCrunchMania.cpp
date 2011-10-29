/////////////////
//
// interface of library, handling of tasks for master-library
// and pass-through to actual decoding decruncher
//

#include "xfdCrunchMania.h"

// from master-project for buffer-class
#include "AnsiFile.h"

// from master-project (fulfill fwd. decl.)
#include "XpkProgress.h"

// use decrunching helper for actual decoding of data
#include "DeCrunchMania.h"


//TODO: when ready, we can add tracking of created instances
// (when library needs members per-user..)

// (see header)
xfdCrunchMania g_Instance;
xfdLibraryBase *GetXpkInstance(void)
{
  // TODO: switch to: new xpkRLEN() when there are members..
	return &g_Instance;
}

xfdCrunchMania::xfdCrunchMania()
	: xfdLibraryBase()
{
	// only to/from buffer supported here
	m_XpkCaps.input.buffer = true;
	m_XpkCaps.output.buffer = true;
	m_XpkCaps.m_LibIdentifier = "Crunch-Mania"; //
	m_XpkCaps.m_LibDescription = "Amiga Crunch-Mania decrunching";
}

xfdCrunchMania::~xfdCrunchMania()
{
}

bool xfdCrunchMania::isSupported(CReadBuffer *pInputBuffer)
{
	if (::memcmp(pInputBuffer->GetBegin(), "CrM!", 4) == 0)
	{
		// Amiga CrunchMania packed file
		return true;
	}
	return false;
}

// decrunching (unpacking) only supported for this format
bool xfdCrunchMania::Decrunch(XpkProgress *pProgress)
{
	// only whole file decompression for now..
	if (pProgress->xp_chunkIn < pProgress->xp_WholePackedFileSize)
	{
		return false;
	}
	
	try
	{
		// create decruncher for unpacking
		DeCrunchMania *pSlave = DeCrunchMania(pProgress->pInputBuffer);
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

