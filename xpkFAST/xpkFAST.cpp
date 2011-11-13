/////////////////////////////////////////////////////////
//
// Rewrite into somewhat-C-like code
// from xpkFAST.a and decompress.a (M68k-assembler)
// as found in XPK source package.
//
// Original author unknown,
// assuming: Dirk Stöcker <stoecker@amigaworld.com>
//
// C-like rewrite by: Ilkka Prusi <ilkka.prusi@gmail.com>
//
// Some interesing info found on module:
// FastName:	dc.b	'fasTcinator',0
// LongName:	dc.b	'FAST alias WOOP or LZRW++',0
// Description:	dc.b	'LZ77-family, windowsize: 4095, no quotecount, 2 streams',0
//

#include "xpkFAST.h"

// from master-project for buffer-class
#include "AnsiFile.h"

// from master-project (fulfill fwd. decl.)
#include "XpkProgress.h"

// our helper for decrunch-conversion,
// reuse some code for ASM-to-C conversion..
#include "FAST.h"

////////////////// xpkFAST : library interface

//TODO: when ready, we can add tracking of created instances
// (when library needs members per-user..)

// (see header)
xpkFAST g_Instance;
xpkLibraryBase *GetXpkInstance(void)
{
  // TODO: switch to: new xpkRLEN() when there are members..
	return &g_Instance;
}

xpkFAST::xpkFAST()
    : xpkLibraryBase()
{
	// only to/from buffer supported here
	m_XpkCaps.input.buffer = true;
	m_XpkCaps.output.buffer = true;
	m_XpkCaps.m_LibIdentifier = "FAST";
	m_XpkCaps.m_LibDescription = "XPK (FAST) implementation";
}

xpkFAST::~xpkFAST()
{
}

/* 
compress.a and compress2.a available..
(which are just "fast" and "slow" alternatives of same packing)
bool xpkFAST::Crunch(XpkProgress *pProgress)
{
	return false;
}
*/

bool xpkFAST::Decrunch(XpkProgress *pProgress)
{
	CFAST fast; // pre-chunk decrunching -> no need to keep as member

/*
	unpack-call in xpkFAST.a is just setup and call 
	to decompress in decompress.a -> setup and 
	start code there in XpkFAST::decrunch()
		
	_XpksUnpackChunk:
		move.l	xsp_OutBuf(a0),a1
		move.l	xsp_InLen(a0),d0
		move.l	xsp_InBuf(a0),a0
	
		bsr	decompress
		moveq.l #XPKERR_OK,d0
		rts
*/	
	
	// we assume master-library allocated suitable buffer
	// for unpacking chunk (known by XPK metadata)
	return fast.decrunch(pProgress->pInputBuffer, pProgress->pOutputBuffer, pProgress->xp_chunkIn, pProgress->xp_chunkOut);
}

