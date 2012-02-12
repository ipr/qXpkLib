/////////////////////////////////////////////////////////
//
// Rewrite into somewhat-C-like code
// from xpkFAST.a and decompress.a (M68k-assembler)
// as found in XPK source package.
//
// Original author unknown,
// assuming: Dirk Stöcker <stoecker@amigaworld.com>
//
// M68k asm to C-like rewrite by: Ilkka Prusi <ilkka.prusi@gmail.com>
//
// Some interesing info found on module:
// FastName:	dc.b	'fasTcinator',0
// LongName:	dc.b	'FAST alias WOOP or LZRW++',0
// Description:	dc.b	'LZ77-family, windowsize: 4095, no quotecount, 2 streams',0
//


#include "FAST.h"

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

// replace assembler-macro with method
//
void CFAST::DOCOPY()
{
DOCOPY_MACRO:
	D4.w = A4.wM(); //move.w	-(a4),d4	;Grab the copy item description
	
	D3.l = 0x0F;	//moveq	#$0F,d3
	
	//and.b	d4,d3		;extract length.
	D3.b &= D4.b;
	
	//add.b	d3,d3		;adjust length to length*2
	D3.b += D3.b;
	
	//lsr.w	#4,d4		;extract offset
	D4.w >>= 4;

	//move.l	a1,a2		;Subtract the offset yielding the
	A2.src = A1.src;
	//suba.l	d4,a2		;address from which we copy.
	A2.src -= D4.l;
	
	//jmp	MOVE\@(PC,d3.w)	;jump to the right place
//MOVE\@	

	// just copy&increment both -> loop for similarity below..
	//move.b	(a2)+,(a1)+	;0
	//move.b	(a2)+,(a1)+	;1
	//move.b	(a2)+,(a1)+	;2
	//move.b	(a2)+,(a1)+	;3
	//move.b	(a2)+,(a1)+	;4
	//move.b	(a2)+,(a1)+	;5
	//move.b	(a2)+,(a1)+	;6
	//move.b	(a2)+,(a1)+	;7
	//move.b	(a2)+,(a1)+	;8
	//move.b	(a2)+,(a1)+	;9
	//move.b	(a2)+,(a1)+	;10
	//move.b	(a2)+,(a1)+	;11
	//move.b	(a2)+,(a1)+	;12
	//move.b	(a2)+,(a1)+	;13
	//move.b	(a2)+,(a1)+	;14
	//move.b	(a2)+,(a1)+	;15
	//move.b	(a2)+,(a1)+
	//move.b	(a2)+,(a1)+
	
	// C-like version,
	// actually we could just memcpy() but use loop for now..
	// (are they overlapping regions?)
	//
	for (int i = 0; i <= 18; i++)
	{
		// auto-increment both and set
		A1.setb(A2);
	}
	
    //ENDM
}

bool CFAST::decrunch(CReadBuffer *pIn, CReadBuffer *pOut, const uint32_t chunkIn, const uint32_t chunkOut)
{
	// less typing this way (instead of R[n]..)
	// -> inherited members now..
	//datareg D0,D1,D2,D3,D4,/*D5,D6,D7*/;
	//addrreg A0,A1,A2,/*A3,*/A4,/*A5,*/A6,A7; // SP == A7
	
	A0.src = pIn->GetBegin();
	D0.l = chunkIn;
	
	// we assume master-library allocated suitable buffer
	// for unpacking chunk (known by XPK metadata)
	D1.l = chunkOut;
	A1.src = pOut->GetBegin();
	
//decompress:
	//movem.l	a0-a2/a4-a5/d1-d5,-(sp) // keep stack

	//move.l	d0,a4 // <- 32-bit into to address? -> replace
	//add.l	a0,a4		;a4:=behind the end of input block
	A4.src += D0.l; // <- count pointer to end of input

	//moveq.l	#0,d4		;d4:=0
	D4.l = 0;


/* // 
DOLITERAL_MACRO:
	move.b	(a0)+,(a1)+	;Literal item means copy a byte.
	-> only two places using, replace with macro body
    ENDM
*/

    //;This loop processes one 16-item item group per iteration.
    //;This loop terminates when we definitely have decompressed all
    //;16-item blocks.  It may overwrite 15+17 innocent bytes AFTER the
    //;end of outbuf.

	
	D1.w = 0x8000;	//move.w	#$8000,d1
	D1.w += D1.w;	// add.w	d1,d1
	
oloop:	

	D1.w = A4.wM(); // auto-decrement //move.w	-(a4),d1
	
	TODO: handle addx
	addx.w	d1,d1 ; add extended, source+dest+x-bit
	bcs.s	Copy0 // carry-set branch -> goto Copy0;


Lit0:	
	//DOLITERAL // 	move.b	(a0)+,(a1)+	;Literal item means copy a byte.
	A1.setb(A0); // (macro inlined)

	D1.w += D1.w;	//add.w	d1,d1
	
	bcc.W	Lit1 // carry-clear branch -> goto Lit1;

Copy1:	
	DOCOPY(); // <- method call replacing assembler macro
	
	D1.w += D1.w;	//add.w	d1,d1
	
	bcc.s	Lit0 // carry-clear branch -> goto Lit0;
	bne.s	Copy0 // not equal branch -> goto Copy0;
	
	cmp.l	a4,a0 // decrement and test compare
	; if ((A0.src - A4.src) == 0 ?? (carry clear..)
	; goto finish;
	bcc.s	finish // carry clear -> goto finish;
	
	D1.w = A4.wM(); // auto-decrement //move.w	-(a4),d1
	
	TODO: handle addx
	addx.w	d1,d1 ; add extended, source+dest+x-bit
	bcc.s	Lit0 // carry clear -> goto Lit0;

Copy0:	
	DOCOPY(); // <- method call replacing assembler macro
	
	; uint32_t d1tmp = (D1.w + D1.w);
	D1.w += D1.w; //add.w	d1,d1
	; if ((d1tmp >> 16) > 0)
	bcs.s	Copy1 // carry-set branch -> goto Copy1;
	
Lit1:	
	//DOLITERAL // 	move.b	(a0)+,(a1)+	;Literal item means copy a byte.
	A1.setb(A0); // (macro inlined)

	D1.w += D1.w;	//add.w	d1,d1
	bcc.W	Lit0 // carry-clear branch -> goto Lit0;
	// ?? if D1.w <= MAXWORD ??
	bne.s	Copy0 // not equal branch -> goto Copy0;
	// ?? if D1.w != 0 ??

	/*
	// check of end of input
	//cmp.l	a4,a0
	if ((A0.src - A4.src) < 0)
	{
		//bcs.W	oloop // carry-set branch -> resume loop
		goto oloop;
	}
	*/
	// simplified: position before end of input -> resume
	if (A0.src < A4.src)
	{
		goto oloop;
	}

finish:	
//	movem.l	(sp)+,a0-a2/a4-a5/d1-d5 // restore stack -> no need
//	rts
	return true;

err:
	return false;
}
