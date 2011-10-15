#include "xfdZCompress.h"

// from master-project for buffer-class
#include "AnsiFile.h"

// from master-project (fulfill fwd. decl.)
#include "XpkProgress.h"

// use helper for actual decrunching
#include "UnZCompress.h"


// (see header)
xfdZCompress g_Instance;
xfdLibraryBase *GetXpkInstance(void)
{
  // TODO: switch to: new xpkRLEN() when there are members..
	return &g_Instance;
}

xfdZCompress::xfdZCompress()
    : xfdLibraryBase()
{
}

xfdZCompress::~xfdZCompress()
{
}

// no packing support for this format
bool xfdZCompress::Crunch(XpkProgress *pProgress)
{
	return false;
}

// decrunching (unpacking) only supported for this format
bool xfdZCompress::Decrunch(XpkProgress *pProgress)
{
	// only whole file decompression for now..
	if (pProgress->xp_chunkIn < pProgress->xp_WholePackedFileSize)
	{
		return false;
	}

	CUnZCompress unz(pProgress->pInputBuffer, pProgress->pOutputBuffer);
	try
	{
		if (unz.isZCompress(pProgress->pInputBuffer->GetBegin(), pProgress->pInputBuffer->GetSize()) == false)
		{
			// unsupported format
			return false;
		}
	
		// actually, need to set some values to progress before exiting..
		if (unz.uncompress() == true)
		{
			// also, no exception? -> success
			size_t nUnpackedSize = pProgress->pOutputBuffer->GetCurrentPos();
			
			pProgress->xp_UnpackedSize = nUnpackedSize; // size of whole file as unpacked
			pProgress->xp_chunkOut = nUnpackedSize; // whole file as single chunk
			pProgress->xp_PackedProcessed += pProgress->xp_chunkIn;
			pProgress->xp_UnpackedProcessed += pProgress->xp_chunkOut;
			
			return true;
		}
	}
	catch (std::exception &exp) // catch by base-type
	{
		// debug..
		//std::cerr << "Exception: " << exp.what() << std::end;
	}
	return false;
}

