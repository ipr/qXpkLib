/////////////////
//
// interface of library, handling of decrunchers for ByteKiller variants
//
// Ilkka Prusi <ilkka.prusi@gmail.com>
//

#include "xfdByteKiller.h"

// from master-project for buffer-class
#include "AnsiFile.h"

// from master-project (fulfill fwd. decl.)
#include "XpkProgress.h"

// decrunch-slaves for ByteKiller-variants
#include "ByteKiller.h"


//TODO: when ready, we can add tracking of created instances
// (when library needs members per-user..)

// (see header)
xfdByteKiller g_Instance;
xfdLibraryBase *GetXpkInstance(void)
{
  // TODO: switch to: new xpkRLEN() when there are members..
	return &g_Instance;
}

xfdByteKiller::xfdByteKiller()
	: xfdLibraryBase()
{
	// only to/from buffer supported here
	m_XpkCaps.input.buffer = true;
	m_XpkCaps.output.buffer = true;
	m_XpkCaps.m_LibIdentifier = "ByteKiller"; // multiple variants
	m_XpkCaps.m_LibDescription = "Amiga ByteKiller decrunching";
}

xfdByteKiller::~xfdByteKiller()
{
}

bool xfdByteKiller::isSupported(CReadBuffer *pInputBuffer)
{
	// TODO: enumerate each instance, check if supported..
	// need another way without allocations before knowing it's supported..
	//
	ByteKiller *base = new crun(pInputBuffer);
	if (base->isSupported() == true)
	{
		return true;
	}

}

// decrunching (unpacking) only supported for this format
bool xfdByteKiller::Decrunch(XpkProgress *pProgress)
{
	// only whole file decompression for now..
	if (pProgress->xp_chunkIn < pProgress->xp_WholePackedFileSize)
	{
		return false;
	}
	
	try
	{
		// TODO: instance according to type (see isSupported() here)
		//
		ByteKiller *base = new crun(pProgress->pInputBuffer);
		
		// no exception -> success
		bool bRes = base->decrunch(pProgress->pOutputBuffer);
		if (bRes == true)
		{
			// update&tell master how large output data is (buffer may be larger..)
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

