#include "xpkdummy.h"

// from master-project for buffer-class
#include "AnsiFile.h"

// from master-project (fulfill fwd. decl.)
#include "XpkProgress.h"


xpkDummy::xpkDummy()
 : xpkLibraryBase()
{
}

xpkDummy::~xpkDummy()
{
}

// this is dummy: output is same as input
bool xpkDummy::Decrunch(XpkProgress *pProgress)
{
	// get size of chunk in read-buffer
	size_t nChunkSize = pProgress->pInputBuffer->GetCurrentPos();
	
	// copy input to output and set status
	unsigned char *pSource = pProgress->pInputBuffer->GetBegin();
	
	// append after existing data, keep old,
	// grow buffer if necessary
	//
	pProgress->pOutputBuffer->Append(pSource, nChunkSize);

	// update status and statistics etc.
	// for master-library
	//
	pProgress->xp_PackedProcessed += nChunkSize;
	pProgress->xp_UnpackedProcessed += nChunkSize;
	if (pProgress->xp_UnpackedProcessed < pProgress->xp_UnpackedSize)
	{
		pProgress->xp_Type = XPKPROG_MID;
	}
	else
	{
		pProgress->xp_Type = XPKPROG_END;
		pProgress->xp_PercentageDone = 100;
	}
	
	return true;
}

