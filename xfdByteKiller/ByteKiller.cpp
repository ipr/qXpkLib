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

///////// ByteKiller

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
	if ((D1.b & 1) != 0)
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
		D5.l = A0.l(0);
		A0.src -= 4;
		
		//move.l	-(a0),d0
		D0.l = A0.l(0);
		A0.src -= 4;
		
		//bsr.s	D_CRUN
		goto D_CRUN;
		// replace goto, should return here..

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

/*
Eoruj:
		move.l	-(a0),d0
		eor.l	d0,d5
		move.w	#$0010,ccr
		roxr.l	#1,d0
		rts

D_CRUN:
		//movem.l	d0-d7/a0-a6,-(sp) // stack
		eor.l	d0,d5
		
lbC000046:	
		lsr.l	#1,d0
		bne.s	lbC000054
		bsr.b	Eoruj
lbC000054:	
		bcs.s	lbC0000B0
		moveq	#8,d1
		moveq	#1,d3
		lsr.l	#1,d0
		bne.s	lbC000068
		bsr.b	Eoruj
lbC000068:	
		bcs.w	lbC0000FE
		moveq	#3,d1
		clr.w	d4
lbC000070:	
		subq.w	#1,d1
		clr.w	d2
lbC000074:	
		lsr.l	#1,d0
		bne.s	lbC000082
		move.l	-(a0),d0
		eor.l	d0,d5
		move.w	#$0010,ccr
		roxr.l	#1,d0
lbC000082:	
		roxl.l	#1,d2
		dbra	d1,lbC000074
		move.w	d2,d3
		add.w	d4,d3
lbC00008C:	
		moveq	#7,d1
lbC00008E:	
		lsr.l	#1,d0
		bne.s	lbC00009C
		move.l	-(a0),d0
		eor.l	d0,d5
		move.w	#$0010,ccr
		roxr.l	#1,d0
lbC00009C:	
		roxl.l	#1,d2
		dbra	d1,lbC00008E
		cmpa.l	a1,a2
		ble.w	lbC00013A
		move.b	d2,-(a2)
		dbra	d3,lbC00008C
		bra.s	lbC000120
lbC0000AA:	
		moveq	#8,d1
		moveq	#8,d4
		bra.s	lbC000070
lbC0000B0:	
		moveq	#2,d1
		subq.w	#1,d1
		clr.w	d2
lbC0000B6:	
		lsr.l	#1,d0
		bne.s	lbC0000C4
		move.l	-(a0),d0
		eor.l	d0,d5
		move.w	#$0010,ccr
		roxr.l	#1,d0
lbC0000C4:	
		roxl.l	#1,d2
		dbra	d1,lbC0000B6
		cmpi.b	#2,d2
		blt.s	lbC0000F6
		cmpi.b	#3,d2
		beq.s	lbC0000AA
		moveq	#8,d1
		subq.w	#1,d1
		clr.w	d2
lbC0000DC:	
		lsr.l	#1,d0
		bne.s	lbC0000EA
		move.l	-(a0),d0
		eor.l	d0,d5
		move.w	#$0010,ccr
		roxr.l	#1,d0
lbC0000EA:	
		roxl.l	#1,d2
		dbra	d1,lbC0000DC
		move.w	d2,d3
		moveq	#12,d1
		bra.s	lbC0000FE
lbC0000F6:	
		moveq	#9,d1
		add.w	d2,d1
		addq.w	#2,d2
		move.w	d2,d3
lbC0000FE:	
		subq.w	#1,d1
		clr.w	d2
lbC000102:	
		lsr.l	#1,d0
		bne.s	lbC000110
		move.l	-(a0),d0
		eor.l	d0,d5
		move.w	#$0010,ccr
		roxr.l	#1,d0
lbC000110:	
		roxl.l	#1,d2
		dbra	d1,lbC000102
lbC000116:	
		subq.w	#1,a2
		cmpa.l	a1,a2
		blt.s	lbC00013A
		move.b	0(a2,d2.w),(a2)		;move.b -1(A2,D2.W),-(A2)
		dbra	d3,lbC000116
lbC000120:	
		cmpa.l	a2,a1
		blt.w	lbC000046
		tst.l	d5
		bne.s	lbC00013A
		movem.l	(sp)+,d0-d7/a0-a6
		moveq	#0,d0
		rts
lbC00013A:	
		//movem.l	(sp)+,d0-d7/a0-a6 // stack
		//moveq	#-1,d0
		D0.l = -1;
		rts

*/


 // just flag for exit -> return false		
		//moveq	#0,d0
		//rts
	return false;

}

bool crnd::decrunch(CReadBuffer *pOut)
{

	return false;
}

bool marc::decrunch(CReadBuffer *pOut)
{

	return false;
}

bool xvdg::decrunch(CReadBuffer *pOut)
{

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

	return false;
}

bool arp3::decrunch(CReadBuffer *pOut)
{

	return false;
}

// Pepsi ACE
bool ace::decrunch(CReadBuffer *pOut)
{

	return false;
}

bool pack::decrunch(CReadBuffer *pOut)
{

	return false;
}

