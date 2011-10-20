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


#include "XfdFAST.h"

// start in decompress.a, xpkFAST.a is just setup for information to it.
//
// Comments:
/*
;;;
;;;				Remarks
;;;				=======
;;;
;;;   * This code relies on XPK_MARGIN being >= 32 !!!
;;;     If only one Bit in the last ControlWord is used, the remaining 15
;;;     will be filled with 0.  This will result in 15 LiteralItems on
;;;     decompression.  In Addition to this there is NO CHECK, if a match
;;;     matches more than the input.  This can result in a match which is
;;;     17 Bytes to long giving a maximum total of 32 Bytes garbled after
;;;     the outputblock after decompresion.
;;;       (XPK_MARGIN is 256 in xpkmaster.library V2.0)
;;;

;	XDEF	decompress

;                                ;Register Map
;                                ;============
;  a0    Points to next source      byte.
;  a1    Points to next destination byte.
;  a2    Temporary.
;  a3    Unused.
;  a4    Pointer for the input word stream.
;  a5    Unused.
;  a6    Stackframe linkage register. Don't touch!
;  a7    Stack pointer.

;  d0    Length of the compressed.
;  d1    control Word
;  d2    Counter for halfunrolled decompress_16_items_loop.
;  d3    Temporary used to calculate copy item length.
;  d4    Temporary used to calculate copy item offset.
;  d5    Unused.
;  d6    Unused.
;  d7    Unused.
*/
//
bool XfdFAST::decrunch(CReadBuffer *pIn, CReadBuffer *pOut, const uint32_t chunkIn, const uint32_t chunkOut)
{
	// less typing this way (instead of R[n]..)
	//
	datareg D0,D1,D2,D3,D4,/*D5,D6,D7*/;
	addrreg A0,A1,A2,/*A3,*/A4,/*A5,*/A6,A7; // SP == A7
	
	A0.src = pIn->GetBegin();
	D0.l = chunkIn;
	
	// we assume master-library allocated suitable buffer
	// for unpacking chunk (known by XPK metadata)
	D1.l = chunkOut;
	A1.src = pOut->GetBegin();
	
//decompress:
	//movem.l	a0-a2/a4-a5/d1-d5,-(sp) // stack

	//move.l	d0,a4
	//add.l	a0,a4		;a4:=behind the end of input block
	A4.src += D0.l;

	//moveq.l	#0,d4		;d4:=0
	D4.l = 0;

	

	return true;
	
err:
	return false;
}
