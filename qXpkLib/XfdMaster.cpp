///////////////////////////////////
//
// XFD-decrunching support:
// should load additional decoders as necessary.
//
// May be called from XpkMaster when XFD-decruncher is needed.
//
// Ilkka Prusi
// ilkka.prusi@gmail.com
//

#include "XfdMaster.h"

// reuse librarian for loading decrunchers
#include "XpkLibrarian.h"


//////// protecte methods

void CXfdMaster::loadDecruncher(/*type*/)
{
}


//////// public methods

CXfdMaster::CXfdMaster()
{
}

// detect XFD-supported decrunching from buffer..
//
bool CXfdMaster::isXfdSupported(CReadBuffer *pInputBuffer) const
{
	// TODO: implement..
	// some stuff which may be added (libraries? just classes?)
	// are "decrunch-slaves" for some "alien" crunchers (non-XPK/XFD fileformat)
	//
	// needs huge work to rewrite all XFD-supported from M68k ASM to portable C/C++
	// so might take a while.. (being optimistic here)
	//
	// note:
	// PowerPacker and Imploder "standalone" format 
	// already supported in XPK-style library, are they needed here as well..? 
	//
	// - ByteKiller (1.3, 2.0, 3.0, Pro 1.0..)
	// - CrunchMania (...)
	// - TetraPack (...)
	// - MasterCruncher?
	// ... etc.
	// 
	
	
	if (::memcmp(pInputBuffer->GetBegin(), "LPAK", 4) == 0)
	{
		// "LPAK data cruncher", 
		// similar to "SZDD" except less headers etc.
	}
	else if (::memcmp(pInputBuffer->GetBegin(), "SZDD", 4) == 0)
	{
		// "Microsoft SZDD data cruncher",
		// The compressed data format is basically LZSS
	}
	else if (::memcmp(pInputBuffer->GetBegin(), "CRUN", 4) == 0)
	{
		// ByteKiller clones
	}
	else if (::memcmp(pInputBuffer->GetBegin(), "CRND", 4) == 0)
	{
		// ByteKiller clones
	}
	/* // more ByteKiller clones..
		cmp.l	#'MARC',(A0)
		beq.b	.OK
		cmp.l	#'TMB!',(A0)
		beq.b	.OK
		cmp.l	#'TXIC',(A0)
		beq.b	.OK
		cmp.l	#'SCC!',(A0)
		beq.b	.OK
		cmp.l	#'ons ',(A0)
		bne.b	.Exit
		cmp.l	#' Sym',-(A0)
		bne.b	.Exit
		cmp.l	#'Gary',-(A0)
		bne.b	.Exit
		... and so on..
	*/
	else if (::memcmp(pInputBuffer->GetBegin(), "Vice", 4) == 0)
	{
		// Vice cruncher
	}
	else if (::memcmp(pInputBuffer->GetBegin(), "ZbL!", 4) == 0)
	{
		// one of Zeeball's filetypes (many more it seems..)
	}
	/*
	else if (::memcmp(pInputBuffer->GetAt(0), "TPWM", 4) == 0)
	{
	}
	*/
	// .. and so on ..
	
	return false;
}
