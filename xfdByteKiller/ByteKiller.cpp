/////////
//
// This code is conversion/rewrite from M68k-assembler to C/C++ code for portability.
//
// Multiple authors:
/*
**        written by Mr. Larmer / Wanted Team        **
**            fixed by SDI in 2000/2001		     **
**    small changes by Don Adan/WT in 2000/2001	     **
**       quick fix by Ronald van Dijk in 2002        **
*/
//
// M68k-assembler to C++ library by Ilkka Prusi <ilkka.prusi@gmail.com>
//

#include "ByteKiller.h"

///////// ByteKiller (base)

// called from derived instances
void ByteKiller::Eoruj()
{
	//move.l	-(a0),d0
	D0.l = A0.lM();

	//eor.l	d0,d5
	D5.l ^= D0.l;

/*
	// set lower-part (ccr) of status-register?
	move.w	#$0010,ccr
	
	// rotate-with-extend, shift right and use status-register
	// as "buffer" for previous bits..
	roxr.l	#1,d0
*/
	
	// simulate rotate with something like this..
	// (in base-class)
	ccr.init(0x0010);
	roxr(1, D0);
}

// called from derived instances
void ByteKiller::D_CRUN()
{
D_CRUN:
		//movem.l	d0-d7/a0-a6,-(sp) // stack
		//eor.l	d0,d5
		D5.l ^= D0.l;
		
lbC000046:	
		//lsr.l	#1,d0
		D0.l >>= 1;
		
		bne.s	lbC000054
		//bsr.b	Eoruj
		Eoruj();
		
lbC000054:	
		bcs.s	lbC0000B0
		
		//moveq	#8,d1
		moveq(8, D1);
		
		//moveq	#1,d3
		moveq(1, D3);
		
		//lsr.l	#1,d0
		D0.l >>= 1;
		
		bne.s	lbC000068

		//bsr.b	Eoruj
		Eoruj();
		
lbC000068:	
		bcs.w	lbC0000FE
		
		//moveq	#3,d1
		moveq(3, D1);
		
		clr.w	d4

lbC000070:	
		//subq.w	#1,d1
		D1.w -= 1;
		
		clr.w	d2

lbC000074:	
		//lsr.l	#1,d0
		D0.l >>= 1;
		
		bne.s	lbC000082

		// reduce repeating code: use routine in method
		//move.l	-(a0),d0
		//eor.l	d0,d5
		//move.w	#$0010,ccr
		//roxr.l	#1,d0
		Eoruj(); // exactly same segment of code
		
lbC000082:	
		//roxl.l	#1,d2
		roxl(1, D2);
		
		//dbra	d1,lbC000074
		while ((D1.l--) > -1) // decrement-test-branch
		{
			goto lbC000074;
		}
		
		//move.w	d2,d3
		D3.w = D2.w;
		//add.w	d4,d3
		D3.w += D4.w;
		
lbC00008C:	
		//moveq	#7,d1
		moveq(7,D1);
lbC00008E:	
		//lsr.l	#1,d0
		D0.l >>= 1;
		bne.s	lbC00009C
		
		// reduce repeating code: use routine in method
		//move.l	-(a0),d0
		//eor.l	d0,d5
		//move.w	#$0010,ccr
		//roxr.l	#1,d0
		Eoruj(); // exactly same segment of code
		
lbC00009C:	
		//roxl.l	#1,d2
		roxl(1, D2);
		
		//dbra	d1,lbC00008E
		while ((D1.l--) > -1) // decrement-test-branch
		{
			goto lbC00008E;
		}
		
		//cmpa.l	a1,a2
		if ((A2.src - A1.src) <= 0)
		{
			//ble.w	lbC00013A
			goto lbC00013A;
		}
		
		//move.b	d2,-(a2)
		D2.b = A2.bM();
		
		//dbra	d3,lbC00008C
		while ((D3.l--) > -1) 
		{
			goto lbC00008C;
		}
		
		//bra.s	lbC000120
		goto lbC000120;
		
lbC0000AA:	
		//moveq	#8,d1
		moveq(8, D1);
		//moveq	#8,d4
		moveq(8, D4);
		//bra.s	lbC000070
		goto lbC000070;
		
lbC0000B0:	
		//moveq	#2,d1
		moveq(2, D1);
		//subq.w	#1,d1
		D1.w -= 1;
		
		clr.w	d2

lbC0000B6:	
		//lsr.l	#1,d0
		D0.l >>= 1;
		bne.s	lbC0000C4
		
		// reduce repeating code: use routine in method
		//move.l	-(a0),d0
		//eor.l	d0,d5
		//move.w	#$0010,ccr
		//roxr.l	#1,d0
		Eoruj(); // exactly same segment of code
		
lbC0000C4:	
		//roxl.l	#1,d2
		roxl(1, D2);
		
		//dbra	d1,lbC0000B6
		while ((D1.l--) > -1) 
		{
			goto lbC0000B6;
		}
		
		cmpi.b	#2,d2
		blt.s	lbC0000F6
		cmpi.b	#3,d2
		beq.s	lbC0000AA
		moveq	#8,d1
		subq.w	#1,d1
		clr.w	d2

lbC0000DC:	
		//lsr.l	#1,d0
		D0.l >>= 1;
		bne.s	lbC0000EA
		
		// reduce repeating code: use routine in method
		//move.l	-(a0),d0
		//eor.l	d0,d5
		//move.w	#$0010,ccr
		//roxr.l	#1,d0
		Eoruj(); // exactly same segment of code

lbC0000EA:	
		//roxl.l	#1,d2
		roxl(1, D2);
		
		//dbra	d1,lbC0000DC
		while ((D1.l--) > -1) 
		{
			goto lbC0000DC;
		}
		
		//move.w	d2,d3
		D3.w = D2.w;
		
		//moveq	#12,d1
		moveq(12, D1);
		
		//bra.s	lbC0000FE
		goto lbC0000FE;
		
lbC0000F6:	
		//moveq	#9,d1
		moveq(9, D1);
		//add.w	d2,d1
		D1.w += D2.w;
		//addq.w	#2,d2
		D2.w += 2;
		//move.w	d2,d3
		D3.w = D2.w;
		goto lbC0000FE; // flow below
		
lbC0000FE:	
		//subq.w	#1,d1
		D1.w -= 1;
		clr.w	d2
		goto lbC000102; // flow below

lbC000102:	
		//lsr.l	#1,d0
		D0.l >>= 1;
		bne.s	lbC000110
		
		// reduce repeating code: use routine in method
		//move.l	-(a0),d0
		//eor.l	d0,d5
		//move.w	#$0010,ccr
		//roxr.l	#1,d0
		Eoruj(); // exactly same segment of code
		
		goto lbC000110; // flow below
		
lbC000110:	
		//roxl.l	#1,d2
		roxl(1, D2);
		
		//dbra	d1,lbC000102
		while ((D1.l--) > -1) 
		{
			goto lbC000102;
		}
		goto lbC000116; // flow below
		
lbC000116:	
		do
		{
			//subq.w	#1,a2
			A2.src -= 1;
			cmpa.l	a1,a2
			blt.s	lbC00013A
			move.b	0(a2,d2.w),(a2)		;move.b -1(A2,D2.W),-(A2)
			
			//dbra	d3,lbC000116
		} while ((D3.l--) > -1);
		
		goto lbC000120; // flow below (for now)
		
lbC000120:	
		cmpa.l	a2,a1
		blt.w	lbC000046
		tst.l	d5
		bne.s	lbC00013A
		//movem.l	(sp)+,d0-d7/a0-a6 // restore stack
		//moveq	#0,d0
		D0.l = 0;
		return;
		//rts
lbC00013A:	
		//movem.l	(sp)+,d0-d7/a0-a6 // restore stack
		//moveq	#-1,d0
		D0.l = -1;
		return; 
		//rts
*/
}


///////// crun

bool crun::decrunch(CReadBuffer *pOut)
{
	// in caller
	//cmp.l	#'CRUN',(A0)+
	//bne.s	.Exit
	//A0 = A0+4;
	//D0.l = size;
	//A0.src = src;
	
	//tst.b	(A0)
	//bne.b	.Exit
	if (A0.b(0) != 0)
	{
		return false;
	}
	
	//move.l	(A0)+,D1
	D1.l = A0.l();
	if (D1.b == 0)
	{
		//beq.b	.Exit
		return false;
	}
	
	//btst	#0,D1
	btst(0, D1);
	if (bne())
	{
		//bne.b	.Exit
		return false;
	}
	
	//moveq	#12,D0
	D0.l = 12;
	//add.l	D0,D1
	D1.l += D0.l;

	//move.l	D1,xfdrr_MinSourceLen(A1)
	//if (m_pIn->GetSize() != D1.l)
	
	//tst.b	(A0)
	//bne.b	.Exit
	if (*(A0.src) != 0)
	{
		return false;
	}
		
	//move.l	(A0),D1
	D1.l = A0; // "auto" lvalue -> operator? 
	
	//beq.b	.Exit
	//bmi.b	.Exit
	if (D1.l <= 0)
	{
		return false;
	}
	
	//move.l	D1,xfdrr_MinTargetLen(A1)
	//move.l	D1,xfdrr_FinalTargetLen(A1)
	pOut->Reserve(D1.l);


DB_CRUN:
	//movem.l	d2-d7/a2-a6,-(a7) // stack store
	//move.l	a0,a5
	A5.setl(A0);
	
	//move.l	xfdbi_UserTargetBuf(A5),A1
	//move.l	xfdbi_SourceBuffer(A5),A0
	A0.src = m_pIn->GetBegin();
	//move.l	4(a0),d0
	D0.l = A0.l(4);
	
	//move.l	8(a0),d1
	D1.l = A0.l(8);
	
	//lea	12(a0),a0
	A0.src += 12;
	
	//movea.l	a1,a2
	A2.src = A1.src;
	
	//adda.l	d1,a2
	A2.src += D1.l;
	
	//adda.l	d0,a0
	A0.src += D0.l;
	
	//move.l	-(a0),d5
	D5.l = A0.lM();
	
	//move.l	-(a0),d0
	D0.l = A0.lM();
	
	//bsr.s	D_CRUN
	D_CRUN();

	//tst.l	d0
	//bne.b	Error
	if (D0.l != 0)
	{
		throw IOException("XFD:corrupted data");
	}
	
	// just flag for exit -> return true
	//moveq	#1,d0
	//D0.l = 1;
	return true;



 // just flag for exit -> return false		
		//moveq	#0,d0
		//rts
	return false;

}

bool crnd::decrunch(CReadBuffer *pOut)
{
RB_CRND:
		// already in caller/constructor
		//cmp.l	#"CRND",(A0)+
		//bne.s	.Exit
		//tst.b	(A0)
		if (A0.b(0) != 0)
		{
			//bne.b	.Exit
			return false;
		}
		
		//move.l	(A0),D1
		D1.l = A0.l(0); // no address increment
		
		if (D1.l == 0)
		{
			//beq.b	.Exit
			return false;
		}

		//btst	#0,D1
		btst(0, D1);
		if (bne())
		{
			//bne.b	.Exit
			return false;
		}
		
		//add.l	D1,A0
		A0.src += D1.l;
		
		//moveq	#20,D0
		moveq(20, D0);
		
		//add.l	D0,D1
		D1.l += D0.l;
/*
		// just load limits (that we have in buffer-class now)		
		move.l	D1,xfdrr_MinSourceLen(A1)
		move.l	(A0),D1
		beq.b	.Exit
		bmi.b	.Exit
		move.l	D1,xfdrr_MinTargetLen(A1)
		move.l	D1,xfdrr_FinalTargetLen(A1)
		moveq	#1,d0
		rts
.Exit		
		//moveq	#0,d0 
		//rts
		return false;
		*/
/*
DB_CRND		
		movem.l	d2-d7/a2-a6,-(a7) // keep stack
		move.l	a0,a5

		move.l	xfdbi_UserTargetBuf(A5),A1
		move.l	xfdbi_SourceBuffer(A5),A0
		move.l	xfdbi_MinSourceLen(A5),D0

		move.l	a0,a4
		adda.l	D0,A0
		movem.l	-8(a0),a2/a3
		movem.l	a2/a3,(a4)
		LEA	-16(A0),A0
		movea.l	(A0),A2
		adda.l	A1,A2

		move.l	4(a0),d5
		move.l	-(a0),d0

		//bsr.w	D_CRUN
		D_CRUN();

		tst.l	d0
		bne.w	Error

		moveq	#1,d0
.Exit		
		movem.l	(a7)+,d2-d7/a2-a6 // restore stack
		rts
*/
	return false;
}

bool marc::decrunch(CReadBuffer *pOut)
{
/*
RB_MARC		move.l	D2,-(A7)
		btst	#0,D0
		bne.b	.Exit
		move.l	A0,D2
		add.l	D0,A0
.FindCos	cmp.l	D2,A0
		beq.b	.Exit
		tst.w	-(A0)
		beq.b	.FindCos
		subq.l	#2,A0
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
.OK		move.l	-4(A0),D1
		beq.b	.Exit
		bmi.b	.Exit
		move.l	D1,xfdrr_MinTargetLen(A1)
		move.l	D1,xfdrr_FinalTargetLen(A1)
		sub.l	D2,A0
		addq.l	#4,A0
		move.l	A0,xfdrr_MinSourceLen(A1)
		move.l	(A7)+,D2
		moveq	#1,d0
		rts
.Exit		move.l	(A7)+,D2
		moveq	#0,d0
		rts
*/

/*
DB_MARC		movem.l	d2-d7/a2-a6,-(a7)
		move.l	a0,a5

		move.l	xfdbi_UserTargetBuf(A5),A1
		move.l	xfdbi_SourceBuffer(A5),A0
		move.l	xfdbi_MinSourceLen(A5),D0

		adda.l	D0,A0
		subq.l	#4,A0

		movea.l	-(A0),A2
		adda.l	A1,A2
		move.l	-(a0),d5
		move.l	-(a0),d0

		//bsr.w	D_CRUN
		D_CRUN();

		tst.l	d0
		bne.w	Error

		moveq	#1,d0
.Exit		movem.l	(a7)+,d2-d7/a2-a6
		rts

*/

	return false;
}

bool xvdg::decrunch(CReadBuffer *pOut)
{
/*
RB_xVdg		cmp.l	#'xVdg',(A0)+
		bne.s	.Exit
		tst.b	(A0)
		bne.b	.Exit
		move.l	(A0)+,D1
		beq.b	.Exit
		bmi.b	.Exit
		moveq	#12,D0
		move.l	D1,xfdrr_MinTargetLen(A1)
		sub.l	D0,D1
		move.l	D1,xfdrr_FinalTargetLen(A1)
		tst.b	(A0)
		bne.b	.Exit
		move.l	(A0),D1
		beq.b	.Exit
		btst	#0,D1
		bne.b	.Exit
		add.l	D0,D1
		move.l	D1,xfdrr_MinSourceLen(A1)
		moveq	#1,d0
		rts
.Exit		moveq	#0,d0
		rts
*/
/*
DB_xVdg		movem.l	d2-d7/a2-a6,-(a7)
		move.l	a0,a5
		move.l	xfdbi_UserTargetBuf(A5),A1
		move.l	xfdbi_SourceBuffer(A5),A0
		move.l	xfdbi_MinSourceLen(A5),D0
		adda.l	D0,A0
		movea.l	-(A0),A2
		adda.l	A1,A2
		move.l	-(a0),d5
		move.l	-(a0),d0
		
		//bsr.w	D_CRUN
		D_CRUN();
		
		tst.l	d0
		bne.w	Error
		moveq	#1,d0
.Exit		movem.l	(a7)+,d2-d7/a2-a6
		rts
*/
	return false;
}

/*
compiled AMOS-programs, starting with common hunk-header (since executable) ?
; Amos Pro CLI
; Amos Pro Wb
; Amos Wb
; Amos CLI
bool amos::decrunch(CReadBuffer *pOut)
{

	return false;
}
*/

bool arpf::decrunch(CReadBuffer *pOut)
{
/*
RB_ARPF		cmp.l	#"ARPF",(A0)+
		bne.s	.Exit
		tst.b	(A0)
		bne.b	.Exit
		move.l	(A0)+,D1
		beq.b	.Exit
		btst	#0,D1
		bne.b	.Exit
		add.l	D1,A0
		moveq	#12,D0
		add.l	D0,D1
		move.l	D1,xfdrr_MinSourceLen(A1)
		tst.b	(A0)
		bne.b	.Exit
		move.l	(A0),D1
		beq.b	.Exit
		bmi.b	.Exit
		move.l	D1,xfdrr_MinTargetLen(A1)
		move.l	D1,xfdrr_FinalTargetLen(A1)
		moveq	#1,d0
		rts
.Exit		moveq	#0,d0
		rts

DB_ARPF		movem.l	d2-d7/a2-a6,-(a7)
		move.l	a0,a5
		move.l	xfdbi_UserTargetBuf(A5),A1
		move.l	xfdbi_SourceBuffer(A5),A0
		move.l	xfdbi_MinSourceLen(A5),D0
		adda.l	D0,A0
		movea.l	-(A0),A2
		adda.l	A1,A2
		move.l	-(a0),d5
		move.l	-(a0),d0
		
		//bsr.w	D_CRUN
		D_CRUN();
		
		tst.l	d0
		bne.w	Error
		moveq	#1,d0
.Exit		movem.l	(a7)+,d2-d7/a2-a6
		rts

*/
	return false;
}

bool arp3::decrunch(CReadBuffer *pOut)
{
/*
RB_ARP3		movem.l	d2-d5,-(a7)
		cmp.l	#'ARP3',(A0)+
		bne.b	.Exit
		tst.b	(A0)
		bne.b	.Exit
		move.l	(A0)+,D1
		beq.b	.Exit
		btst	#0,D1
		bne.b	.Exit
		move.l	(A0)+,D2
		beq.b	.Skip1
		btst	#0,D2
		bne.b	.Exit
		sub.l	#$C00000,D2
.Skip1		move.l	(A0)+,D3
		beq.b	.Skip2
		btst	#0,D3
		bne.b	.Exit
		sub.l	#$200000,D3
.Skip2		moveq	#16,D5
		add.l	D5,D1
		add.l	D1,A0
		move.l	-(A0),D4
		beq.b	.Exit
		bmi.b	.Exit
		tst.l	D2
		beq.b	.NoSlow
		add.l	D2,A0
		add.l	(A0),D4
		add.l	D2,D1
.NoSlow		tst.l	D3
		beq.b	.NoFast
		add.l	D3,A0
		add.l	(A0),D4
		add.l	D3,D1
.NoFast		add.l	D5,D1
		move.l	D1,xfdrr_MinSourceLen(A1)
		move.l	D4,xfdrr_FinalTargetLen(A1)
		move.l	D4,xfdrr_MinTargetLen(A1)
		moveq	#1,d0
		bra.b	.End
		rts
.Exit		moveq	#0,d0
.End		movem.l	(a7)+,d2-d5
		rts

DB_ARP3		movem.l	d2-d7/a2-a6,-(a7)
		move.l	a0,a5
		move.l	xfdbi_UserTargetBuf(A5),A1
		move.l	xfdbi_SourceBuffer(A5),A0
		move.l	4(a0),d0
		adda.l	D0,A0
		lea	$20(A0),A0
		movea.l	-(A0),A2
		adda.l	A1,A2
		move.l	-(a0),d5
		move.l	-(a0),d0
		
		//bsr.w	D_CRUN
		D_CRUN();
		
		tst.l	D0
		bne.w	Error
		move.l	xfdbi_SourceBuffer(a5),a2
		tst.l	8(A2)
		beq.b	.skip3
		addq.l	#8,A0
		add.l	(A0)+,A1
		add.l	8(A2),A0
		sub.l	#$C00000,A0
		movea.l	-(A0),A2
		adda.l	A1,A2
		move.l	-(a0),d5
		move.l	-(a0),d0
		
		//bsr.w	D_CRUN
		D_CRUN();
		
		tst.l	D0
		bne.w	Error
.skip3		move.l	xfdbi_SourceBuffer(a5),a2
		tst.l	12(A2)
		beq.b	.skip4
		addq.l	#8,A0
		add.l	(A0)+,A1
		add.l	12(A2),A0
		sub.l	#$200000,A0
		movea.l	-(A0),A2
		adda.l	A1,A2
		move.l	-(a0),d5
		move.l	-(a0),d0
		
		//bsr.w	D_CRUN
		D_CRUN();
		
.skip4		tst.l	d0
		bne.w	Error
		moveq	#1,d0
.Exit		movem.l	(a7)+,d2-d7/a2-a6
		rts
*/
	return false;
}

// Pepsi ACE
bool ace::decrunch(CReadBuffer *pOut)
{
/*
RB_ACE		cmp.l	#"ACE!",(A0)+
		bne.s	.Exit
		tst.b	(A0)
		bne.b	.Exit
		move.l	(A0)+,D1
		beq.b	.Exit
		btst	#0,D1
		bne.b	.Exit
		moveq	#12,D0
		add.l	D0,D1
		move.l	D1,xfdrr_MinSourceLen(A1)
		tst.b	(A0)
		bne.b	.Exit
		move.l	(A0),D1
		beq.b	.Exit
		bmi.b	.Exit
		move.l	D1,xfdrr_MinTargetLen(A1)
		move.l	D1,xfdrr_FinalTargetLen(A1)
		moveq	#1,d0
		rts
.Exit		moveq	#0,d0
		rts

DB_ACE		movem.l	d2-d7/a2-a6,-(a7)
		move.l	a0,a5
		move.l	xfdbi_UserTargetBuf(A5),A1
		move.l	xfdbi_SourceBuffer(A5),A0
		move.l	xfdbi_MinSourceLen(A5),D0
		move.l	8(a0),a2
		move.l	12(a0),d5
		adda.l	D0,A0
		adda.l	A1,A2
		move.l	-(a0),d0
		
		//bsr.w	D_CRUN
		D_CRUN();
		
		tst.l	d0
		bne.w	Error
		moveq	#1,d0
.Exit		movem.l	(a7)+,d2-d7/a2-a6
		rts
*/
	return false;
}

bool pack::decrunch(CReadBuffer *pOut)
{

	return false;
}

