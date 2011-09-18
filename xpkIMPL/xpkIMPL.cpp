#include "xpkIMPL.h"

// from master-project for buffer-class
#include "AnsiFile.h"

// from master-project (fulfill fwd. decl.)
#include "XpkProgress.h"

// use helper for actual decrunching
#include "PowerPacker.h"

//TODO: when ready, we can add tracking of created instances
// (when library needs members per-user..)

// (see header)
xpkIMPL g_Instance;
xpkLibraryBase *GetXpkInstance(void)
{
  // TODO: switch to: new xpkRLEN() when there are members..
	return &g_Instance;
}

xpkIMPL::xpkIMPL()
	: xpkLibraryBase()
{
}

xpkIMPL::~xpkIMPL()
{
}

// no packing support for this format
bool xpkIMPL::Crunch(XpkProgress *pProgress)
{
	return false;
}

// decrunching (unpacking) only supported for this format
bool xpkIMPL::Decrunch(XpkProgress *pProgress)
{
	// TODO: may be possible that this is used as sub-type of XPK
	// or entirely stand-alone (some sub-type library exists on Amiga?)
	// -> usually stand-alone version anyway..

	/*
	if (pProgress->IsXpkFile() == true)
	{
		// use chunk-nodes (code from master-library actually..)
		m_pTagList = new XpkTags();
		m_pTagList->ParseToNodeList(*pProgress->pInputBuffer);
		
		// TODO: process by chunks (without the chunk-info..)
	}
	*/
	
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
		Impl.UnpackBuffer(pProgress->pInputBuffer->GetBegin(), pProgress->pInputBuffer->GetCurrentPos());
		
		// no exception -> keep unpacked data
		//
		// (TODO: get directly to output-buffer..)
		//
		pProgress->pOutputBuffer->Append(Impl.GetUnpackedData(), Impl.GetUnpackedSize());
		
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

