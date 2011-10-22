#include "xfdSZDD.h"

// from master-project for buffer-class
#include "AnsiFile.h"

// from master-project (fulfill fwd. decl.)
#include "XpkProgress.h"

// use helper for actual decrunching
#include "SzddExpand.h"


// (see header)
xfdSZDD g_Instance;
xfdLibraryBase *GetXpkInstance(void)
{
  // TODO: switch to: new xpkRLEN() when there are members..
	return &g_Instance;
}

xfdSZDD::xfdSZDD()
    : xfdLibraryBase()
{
	// only to/from buffer supported here
	m_XpkCaps.input.buffer = true;
	m_XpkCaps.output.buffer = true;
	m_XpkCaps.m_LibIdentifier = "SZDD";
	m_XpkCaps.m_LibDescription = "SZDD uncompressing implementation";
}

xfdSZDD::~xfdSZDD()
{
}

// no packing support for this format
bool xfdSZDD::Crunch(XpkProgress *pProgress)
{
	return false;
}

// decrunching (unpacking) only supported for this format
bool xfdSZDD::Decrunch(XpkProgress *pProgress)
{
	// only whole file decompression for now..
	if (pProgress->xp_chunkIn < pProgress->xp_WholePackedFileSize)
	{
		return false;
	}

	// just use the stand-alone unpacker
	// when not used as sub-type of XPK..
	//
	CSzddExpand szdd;
	try
	{
		// load from existing buffer and unpack to temporary..
		//
		// (TODO: use directly input-buffer instead of copy..
		// also give output for direct use..)
		//
		if (szdd.UnpackBuffer(pProgress->pInputBuffer->GetBegin(), pProgress->xp_WholePackedFileSize) == false)
		{
			// TODO: change to throw exception instead
			return false;
		}
		
		// no exception -> keep unpacked data
		//
		// (TODO: decrunch directly to output-buffer..)
		//
		pProgress->pOutputBuffer->Append(szdd.GetUnpackedData(), szdd.GetUnpackedSize());
		
		pProgress->xp_UnpackedSize = szdd.GetUnpackedSize(); // size of whole file as unpacked
		pProgress->xp_chunkOut = szdd.GetUnpackedSize(); // whole file as single chunk
		
		// no exception -> success
		return true;
	}
	catch (std::exception &exp) // catch by base-type
	{
		// debug..
		//std::cerr << "Exception: " << exp.what() << std::end;
	}
	return false;
}

