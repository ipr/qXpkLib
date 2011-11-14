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
	// only to/from buffer supported here
	m_XpkCaps.input.buffer = true;
	m_XpkCaps.output.buffer = true;
	m_XpkCaps.m_LibIdentifier = "Z";
	m_XpkCaps.m_LibDescription = "Uncompress Un*x Z-compressed files";
}

xfdZCompress::~xfdZCompress()
{
}

bool xfdZCompress::isSupported(CReadBuffer *pInputBuffer)
{
}

bool xfdZCompress::archiveInfo(QXpkLib::CArchiveInfo &info)
{
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
		size_t nResultSize = unz.uncompress();
		if (nResultSize > 0)
		{
			// also, no exception? -> success
			//size_t nUnpackedSize = pProgress->pOutputBuffer->GetCurrentPos();
			
			pProgress->xp_UnpackedSize = nResultSize; // size of whole file as unpacked
			pProgress->xp_chunkOut = nResultSize; // whole file as single chunk
			return true;
		}
		return false; // not needed (fallthrough)
	}
	catch (std::exception &exp) // catch by base-type
	{
		// debug..
		//std::cerr << "Exception: " << exp.what() << std::end;
	}
	return false;
}

