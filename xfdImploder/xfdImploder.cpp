#include "xfdImploder.h"

// from master-project for buffer-class
#include "AnsiFile.h"

// from master-project (fulfill fwd. decl.)
#include "XpkProgress.h"

// use helper for actual decrunching
#include "ImploderExploder.h"

//TODO: when ready, we can add tracking of created instances
// (when library needs members per-user..)

// (see header)
xfdImploder g_Instance;
xpkLibraryBase *GetXpkInstance(void)
{
  // TODO: switch to: new xpkRLEN() when there are members..
	return &g_Instance;
}

xfdImploder::xfdImploder()
	: xpkLibraryBase()
{
}

xfdImploder::~xfdImploder()
{
}

// no packing support for this format
bool xfdImploder::Crunch(XpkProgress *pProgress)
{
	return false;
}

// decrunching (unpacking) only supported for this format
bool xfdImploder::Decrunch(XpkProgress *pProgress)
{
	// TODO: may be possible that this is used as sub-type of XPK
	// or entirely stand-alone (some sub-type library exists on Amiga?)
	// -> usually stand-alone version anyway..

	/*
	if (pProgress->IsXpkFile() == true)
	{
		// use chunk-nodes (code from master-library actually..)
		//m_pTagList = new XpkTags();
		//m_pTagList->ParseToNodeList(*pProgress->pInputBuffer);
		
		// TODO: process by chunks (without the chunk-info..)
	}
	*/
	
	// only whole file decompression for now..
	if (pProgress->xp_chunkIn < pProgress->xp_WholePackedFileSize)
	{
		return false;
	}
	
	// just use the stand-alone unpacker
	// when not used as sub-type of XPK..
	//
	CImploderExploder Impl;
	try
	{
		// load from existing buffer and unpack to temporary..
		//
		// (TODO: use directly input-buffer instead of copy..
		// also give output for direct use..)
		//
		Impl.UnpackBuffer(pProgress->pInputBuffer->GetBegin(), pProgress->xp_WholePackedFileSize);
		
		// no exception -> keep unpacked data
		//
		// (TODO: decrunch directly to output-buffer..)
		//
		pProgress->pOutputBuffer->Append(Impl.GetUnpackedData(), Impl.GetUnpackedSize());

		pProgress->xp_UnpackedSize = Impl.GetUnpackedSize(); // size of whole file as unpacked
		pProgress->xp_chunkOut = Impl.GetUnpackedSize(); // whole file as single chunk
		pProgress->xp_PackedProcessed += pProgress->xp_chunkIn;
		pProgress->xp_UnpackedProcessed += pProgress->xp_chunkOut;
		
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

