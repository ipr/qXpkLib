///////////////////////////
//
// xpk "NUKE" compression decruncher
//
// Based on M68k-assembler code
// by: Dirk Stöcker (SDI) <stoecker@amigaworld.com>
//
// C-like rewrite by: Ilkka Prusi <ilkka.prusi@gmail.com>
//

#include "Nuke.h"

/*
// alias names for registers?
tmp	EQUR	D0
two	EQUR	D1
len	EQUR	D2
off	EQUR	D3
i	EQUR	D5
look	EQUR	D6
arg	EQUR	D7

lst	EQUR	A3
buf	EQUR	A5
nxt	EQUR	A6
*/

/* I dislike preprocessor-stuff but simplest way now.. */
/* check: only packing used in original?
#define tmp D0
#define two D1
#define len D2
#define off D3

#define i D5
#define look D6
#define arg D7

#define lst A3
#define buf A5
#define nxt A6
*/
 

//starts:	dc.w	0,0,4,10
uint16_t starts[] = {0,0,4,10}; 

uint16_t modes[] = {4,6,8,9,4,7,9,11,13,14,5,7,9,11,13,14};


bool CNuke::decrunch(CReadBuffer *pIn, CReadBuffer *pOut, 
				const uint32_t chunkIn, const uint32_t chunkOut)
{
	// this was set to A3 as constant which was not changed later,
	// used so just because all data registers were already in use..?
	// -> use as constant instead..
	const int32_t A3_val = 16;

//_AsmUnpack
	//movem.l	d2-d7/a3-a6,-(a7) // keep stack
	//MOVEA.L	A1,A5			; only because of Maxon A5 problem
	A5.src = A1.src;
	
	//moveq	#16,d0
	D0.l = 16;
	
	//move.l	d0,a3
	// ?? why is this in address-register?
	// running out of data registers..?
	// it's never changed anyway 
	// -> use constant instead
	//A3.src = 16; 
	
	//lea	modes,a6
	A6.src = &modes;
	
	//moveq	#0,d0
	D0.l = 0;
	//moveq	#0,d1
	D1.l = 0;
	
	//move.w	#$8000,d2
	D2.w = 0x8000;
	
	//move.w	#$8000,d3
	D3.w = 0x8000;
	
	//moveq	#0,d4
	D4.l = 0;
	//moveq	#0,d5
	D5.l = 0;
	//moveq	#0,d6
	D6.l = 0;
	//moveq	#0,d7
	D7.l = 0;

	// note: bsr allows rts back to caller
	// -> can continue here after branch
	//
	//bsr	TestCompressed
	TestCompressedF(); // uncond.
	
	//move.l	a0,d0
	D0.l = A0.src; // ?? position for length? -> must change for 64-bit..
	
	//movem.l	(a7)+,d2-d7/a3-a6 // restore stack
	return true;
	//rts
}

void CNuke::TestCompressedF()
{
	// start at label..
	goto TestCompressed; // uncond.

//; d0 mode		a0 writepos
//; d1 offslen/offset	a1 copysrc
//; d2 1bit_data		a2 writeend
//; d3 2bits_data		a3 16

//; d4 4bits_data		a4 byteread
//; d5 4bits_in		a5 wordread
//; d6 xbits_data		a6 table
//; d7 -xbits_in		a7

Compressed:
	//dbra	d5,NoRead4		; 70	Read 4 bits (mode)
	while ((D5.l--) > -1) // decr. test&branch
	{
		goto NoRead4;
	}
	
	//moveq	#7,d5			; 8
	D5.l = 7;
	
	//move.l	(a5)+,d4		; |
	D4.l = A5.l(); // refence, copy & increment offset
	
	//add.l	d4,d4			; |
	D4.l += D4.l;
	
NoRead4:				
	//moveq	#30,d0			; 70
	D0.l = 30;
	//and.w	d4,d0			; |
	D0.w &= D4.w;
	
	//roxr.l	#4,d4			; |
	XfdSlave::roxl(4, D4); // 
	
	//move.w	0(a6,d0.w),d1		; |
	D1.w = A6.w(d0.w);
	
	//clr.w	d6			; |
	D6.w = 0;
	
	//rol.l	d1,d6			; |	Read n bits (offset)
	XfdSlave::rol(D1.l, D6);
	
	//add.w	d1,d7			; |     ble jumps on N|(Z^V)
	D7.w += D1.w;
	
	if ((D7.w - D1.w) <= 0) // branch (less or equal)
	{
		//ble.s	NoReadN			; |	the add does never set V
		goto NoReadN;
	}
	
	//moveq	#0,d1			; 35
	D1.l = 0;
	
	//move.w	(a5)+,d1		; |
	D1.w = A5.w(); // ref. move & increment address
	
	// arith. shift left:
	// uses ccrc register bits as carry/overflow -> implement in base	
	//asl.l	d7,d1			; |	
	XfdSlave::asl(D7.l, D1); // TODO: finish this
	//swap	d1			; |
	XfdSlave::swap(D1); // swap halves of register
	
	//or.l	d1,d6			; |
	D6.l |= D1.l; //
	
	//sub.w	a3,d7			; |
	D7.w -= A3_val; // replaced with constant (see before)
	
NoReadN:				
	//move.l	a0,a1			; 70
	A1.src = A0.src; // address as-is
	
	//add.w	32(a6,d0.w),d6		; |
	D6.w += A6.w(D0.w+32); // offset&add to register
	
	//sub.w	d6,a1			; |
	A1.src -= D6.w;
	
	//move.w	64(a6,d0.w),d0		; |
	D0.w = A6.w(D0.w+64);
	
	// jump instead of branch when absolute address 
	// or offset longer than relative branch allows
	// 
	//jmp	TwoBitLen(pc,d0.w) //
	goto TwoBitLen;

TwoBitLen:
	//move.b	(a1)+,(a0)+		; 21
	A0.setb(A1); // copy&increment
	//move.b	(a1)+,(a0)+		; |
	A0.setb(A1); // copy&increment
	//move.b	(a1)+,(a0)+		; |
	A0.setb(A1); // copy&increment
	
	//moveq	#0,d0			; |	Read 2 bits (clen1)
	D0.l = 0;
	//add.w	d3,d3			; |
	D3.w += D3.w;
	
	if (D3.w != 0)
	{
		//bne.s	NoRead2x		; |
		goto NoRead2x;
	}
	
	//move.w	(a5)+,d3		; 1
	D3.w = A5.w(); // copy&increment
	
	// add extended: use ccr extend-bit..
	addx.w	d3,d3			; |
	
NoRead2x:				;
	addx.w	d0,d0			; 9
	//add.w	d3,d3			; |
	D3.w += D3.w;
	addx.w	d0,d0			; |

	//add.w	d0,d0
	D0.w += D0.w;
	//jmp	CJTable(pc,d0.w)
	goto CJTable;

CJTable:
	// "BRA" is unconditional branch 
	// so labels below are never used..
	//bra.s	Tab15
	goto Tab15; // unconditional
	
	// where are these used..?	
	// -> nowhere ?
c3:	
	//move.b	(a1)+,(a0)+		; 21
	A0.setb(A1); // copy&incr both addr pos
c2:	
	//move.b	(a1)+,(a0)+		; 21
	A0.setb(A1); // copy&incr both addr pos
	//move.b	(a1)+,(a0)+		; |
	A0.setb(A1); // copy&incr both addr pos

//;----------------------------------------
TestCompressed:				
	//add.w	d2,d2			; 70	Read 1 (cbit)
	
	D2.w += D2.w;
	
	// carry clear -> not larger -> goto Uncompressed
	bcc.s	Uncompressed		; | 

	// non-zero result -> goto Compressed
	bne.s	Compressed		; | 
	
	// zero result -> continue below and goto Compressed..
	
	//move.w	(a5)+,d2		; 4
	D2.w = A5.w();
	
	addx.w	d2,d2			; |
	bcs.s	Compressed		; 70
//;----------------------------------------

Uncompressed:				;
	add.w	d2,d2			; 30	Read 1 (ulen1)
	bne.s	1$			; |
	move.w	(a5)+,d2		; 2
	addx.w	d2,d2			; |
1$					;
	bcc.s	Copy3Entry		; 30
	move.b	-(a4),(a0)+		; 15

TestExit:
	// writepos - writeend <0 -> more compressed
	//cmp.l	a2,a0			; |
	if ((A0.src - A2.src) < 0)
	{
		//blt	Compressed		; |
		goto Compressed;
	}
	//rts
	return; // -> back to caller from subroutine

CopyThree:
	// copy in reverse, decr&incr addr pos
	move.b	-(a4),(a0)+		; 9
	move.b	-(a4),(a0)+		; |
	move.b	-(a4),(a0)+		; |

Copy3Entry:
	//moveq	#0,d0			; 30	Read 2 (ulen2)
	D0.l = 0;
	//add.w	d3,d3			; |
	D3.w += D3.w;
	
	bne.s	NoRead2a		; |
	move.w	(a5)+,d3		; 4
	addx.w	d3,d3			; |
NoRead2a:				;
	addx.w	d0,d0			; 30
	add.w	d3,d3			; |
	addx.w	d0,d0			; |
	add.w	d0,d0			; |
	
	//jmp	UJTable(pc,d0.w)	; |
	goto UJTable;

UJTable:
	bra.s	CopyThree		; 3
	move.b	-(a4),(a0)+		; 7
	move.b	-(a4),(a0)+		; 10
	move.b	-(a4),(a0)+		; 15
	move.b	-(a4),(a0)+		; 15
	cmp.l	a2,a0			; |
	blt	Compressed		; |
	rts				;
*/
//;---------------------------------------
FJTable:
	//moveq	#0,d0
	D0.l = 0;
	
	//move.b	(a1)+,(a0)+		; |
	//move.b	(a1)+,(a0)+		; |
	//move.b	(a1)+,(a0)+		; |
	//move.b	(a1)+,(a0)+		; |
	
	//move.b	(a1)+,(a0)+		; |
	//move.b	(a1)+,(a0)+		; |
	//move.b	(a1)+,(a0)+		; |
	//move.b	(a1)+,(a0)+		; |
	
	//move.b	(a1)+,(a0)+		; |
	//move.b	(a1)+,(a0)+		; |
	//move.b	(a1)+,(a0)+		; |
	//move.b	(a1)+,(a0)+		; |
	
	// copy&increment loop
	for (int i = 0; i < 12; i++)
	{
		A0.setb(A1);
	}
	goto Tab15; // fallthrough (below)

Tab15:
	//move.b	(a1)+,(a0)+		; |
	//move.b	(a1)+,(a0)+		; |
	//move.b	(a1)+,(a0)+		; |
	
	for (int i = 0; i < 3; i++)
	{
		A0.setb(A1);
	}
	
	//tst.b	d0
	if (D0.b != 0)
	{
		//bne.s	TestCompressed
		goto TestCompressed;
	}


	//dbra	d5,1$
	while ((D5.l--) > -1)
	{
		goto 1$;
	
	}
	
	//moveq	#7,d5
	D5.l = 7;
	//move.l	(a5)+,d4
	D4.l = A5.l();
	//add.l	d4,d4
	D4.l += D4.l;
	
1$:
	//moveq	#30,d0
	D0.l = 30;
	//and.w	d4,d0
	D0.w &= D4.w;
	//roxr.l	#4,d4
	XfdSlave::roxr(4,D4);

	// just long distance/absolute address needs
	// -> counted pc-relative offset so just long distance?
	//
	//jmp	FJTable(pc,d0.w)
	goto FJTable;

}
