///////////////////////////////////
//
// XFD-decrunching support:
// slave-decrunchers for use by XFD-master.
//
// (May be used in XpkMaster when XFD-decruncher is needed.)
//
// Ilkka Prusi
// ilkka.prusi@gmail.com
//
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

#include "XfdSlave.h"

///////// base is pure virtual for now

///////// ByteKiller

bool XfdByteKiller::crun(CReadBuffer *pOut, uint8_t *src, uint32_t size)
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

bool XfdByteKiller::crnd(CReadBuffer *pOut, uint8_t *src, uint32_t size)
{

	return false;
}

bool XfdByteKiller::marc(CReadBuffer *pOut, uint8_t *src, uint32_t size)
{

	return false;
}

bool XfdByteKiller::xvdg(CReadBuffer *pOut, uint8_t *src, uint32_t size)
{

	return false;
}

/*
compiled AMOS-programs, starting with common hunk-header (since executable) ?
; Amos Pro CLI
; Amos Pro Wb
; Amos Wb
; Amos CLI
bool XfdByteKiller::amos(CReadBuffer *pOut, uint8_t *src, uint32_t size)
{

	return false;
}
*/

bool XfdByteKiller::arpf(CReadBuffer *pOut, uint8_t *src, uint32_t size)
{

	return false;
}

bool XfdByteKiller::arp3(CReadBuffer *pOut, uint8_t *src, uint32_t size)
{

	return false;
}

// Pepsi ACE
bool XfdByteKiller::ace(CReadBuffer *pOut, uint8_t *src, uint32_t size)
{

	return false;
}

bool XfdByteKiller::pack(CReadBuffer *pOut, uint8_t *src, uint32_t size)
{

	return false;
}

bool XfdByteKiller::decrunch(CReadBuffer *pOut)
{
	m_pIn->SetCurrentPos(0);
	
	uint8_t *pA0 = m_pIn->GetBegin(); // <- buffer address
	uint32_t size = m_pIn->GetSize(); // <- buffer length

	// init here
	D0.l = size;
	A0.src = pA0+4;

	uint32_t tag = MakeTag((char*)pA0);
	if (tag == MakeTag("CRUN"))
	{
		//cmp.l	#'CRUN',(A0)+
		//bne.s	.Exit
		return crun(pOut, pA0+4, size);
	}
	
	if (tag == MakeTag("CRND"))
	{
		return crnd(pOut, pA0+4, size);
	}
	
	if (tag == MakeTag("MARC")
		|| tag == MakeTag("TMB!")
		|| tag == MakeTag("TXIC")
		|| tag == MakeTag("SCC!"))
	{
		return marc(pOut, pA0+4, size);
	}
	
	if (tag == MakeTag("ons ")
		|| tag == MakeTag(" Sym")
		|| tag == MakeTag("Gary"))
	{
		// non-supported variant of marc() ?
		return false;
	}

	if (tag == MakeTag("xVdg"))
	{
		//cmp.l	#'xVdg',(A0)+
		//bne.s	.Exit
		return xvdg(pOut, pA0+4, size);
	}
	// if (tag == MakeTag("xVgd")) // this also?
	
	/* compiled AMOS program .. starts with common hunk header (since executable) ? */
	//return amos(pOut, pA0+4, size);

	if (tag == MakeTag("ARPF"))
	{
		return arpf(pOut, pA0+4, size);
	}
	
	if (tag == MakeTag("ARP3"))
	{
		return arp3(pOut, pA0+4, size);
	}

	if (tag == MakeTag("ACE!"))
	{
		/* Pepsi ACE */
		return ace(pOut, pA0+4, size);
	}

	if (tag == MakeTag("PACK"))
	{
		return pack(pOut, pA0+4, size);
	}

	// missed any ?? (hope not..)
	return false;
}

///////// Vice

bool XfdVice::decrunch(CReadBuffer *pOut)
{
	m_pIn->SetCurrentPos(0);
	
	A0.src = m_pIn->GetBegin(); // <- buffer address
	D0.l = m_pIn->GetSize(); // <- buffer length
	
	if (MakeTag((char*)A0.src) != MakeTag("Vice"))
	{
		return false;
	}
	
	//CMP.B	#$80,4(A0)
	if ((A0.b(4) - 0x80) != 0)
	{
		//BNE.B	.Exit
		return false;
	}
	
	D1.l = 0x80000;
	//cmp.l	#100000,D0 -> compare to given size
	if ((D0.l - 100000) < 0)
	{
		// file smaller than ~100kbytes?
		// blt.b	.MinMem
		//lsr.l	#1,D1
		D1.l >>= 1;
		//add.l	D0,D1
		D1.l += D0.l;
	}
	else if ((D0.l - D1.l) > 0)
	{
		//cmp.l	D1,D0
		//bgt.b	.MaxMem
		//.MaxMem
		//MOVE.L	D1,xfdrr_MinTargetLen(A1)
		pOut->Reserve(D1.l);
		//MOVE.L	#-1,xfdrr_FinalTargetLen(A1)
		//MOVEQ	#1,D0
		//D0 = 1; // status before exit? (1, true)
		//rts
	}
	
	// checks done, decrunch..

//label DB_Vice:
	//MOVEM.L	D2-D7/A2-A6,-(SP) // (move listed registers, decrease stack)
	//MOVEA.L	A0,A2
	A2.src = A0.src;
	//MOVE.L	xfdbi_SourceBuffer(A2),A6
	A6.src = A0.src; //m_pIn->GetAtCurrent();
	
	//MOVE.L	xfdbi_UserTargetBuf(A2),A3	; dest start
	A3.src = pOut->GetBegin();
	
	//MOVEA.L	A6,A0
	A0.src = A6.src;
	
	//ADD.L		xfdbi_SourceBufLen(A2),A0	; source end
	A0.src += m_pIn->GetSize();
	
	//ADDQ.L	#4,A6				; source start
	A6.src += 4;
	
	//MOVE.L	A3,A1
	A1.src = A3.src;
	
	//ADD.L		xfdbi_UserTargetBufLen(A2),A1	; dest end
	A1.src += pOut->GetSize();

	// keep address? -> needs update..
	//MOVE.L	A3,D2
	//D2.l = (int32_t)(A3.src);
	
	//BSR.B	D_Vice
	//goto D_Vice;
	//MOVE.W	#XFDERR_CORRUPTEDDATA,xfdbi_Error(A2)

	//SUBA.L	D2,A3
	//A3.src += D2.l;
	
	//MOVE.L	A3,xfdbi_TargetBufSaveLen(A2)

	//; D0 already set
	//MOVEM.L	(SP)+,D2-D7/A2-A6 // restore stack
	//RTS 
	
//; A6 -> Crunched data start, past header (source)
//; A0 -> Crunched data end (source)
//; A3 -> Decrunch buffer start (destination)
//; A1 -> Decrunch buffer end (destination)

//; Uses: A0,A1,A3,A4,A5,A6, D0,D1,D3,D4,D5,D6,D7

D_Vice:
		//LEA	$200(A6),A5
		A5.src = A6.src + 0x200;
		
		//LEA	$400(A6),A4
		A4.src = A6.src + 0x400;
		
		//MOVEQ	#1,D6
		D6.l = 1;
		
		//MOVE.W	$1FE(A6),D4
		D4.w = A6.w(0x1FE);
		
		//MOVEQ	#$1F,D7
		D7.l = 0x1F;
		
		//MOVEQ	#7,D3
		D3.l = 7;

/*
Vice01:	
		//MOVEQ	#0,D1
		D1.l = 0;
		
		//BSR.S	.ViceSub
		goto ViceSub;
		
		//TST.B	D5
		if (D5.b == 0)
		{
			//BEQ.S	.Error
			goto Error;
		}
		
		//MOVE.B	D5,D1
		D1.b = D5.b;
		
		BPL.S	.Vice03
		BCLR	D3,D1
		
		//SUBQ.B	#1,D1
		D1.b -= 1;
		
		// this should be own method called..
		BSR.S	.ViceSub
		

Vice02:	
		//CMPA.L	A1,A3
		if ((A3.src - A1.src) > 0)
		{
			//BGE.B	.DestEnd
			goto DestEnd;
		}
		
		//MOVE.B	D5,(A3)+
		A3.setb(D5);
		
		//DBRA	D1,.Vice02
		while ((D1.l--) > -1)
		{
			goto Vice02;
		}
		
		//BRA.S	.Vice01
		goto Vice01; // unconditional

Vice03:	
		//SUBQ.B	#1,D1
		D1.b -= 1;

Vice04:	
		//MOVE.W	D4,D5
		D5.w = D4.w;

Vice05:	
		//LSR.L	#1,D6
		D6.l >>= 1;
		
		BCC.S	.Vice08
		BEQ.S	.Vice07
		
Vice06:	
		//MOVE.W	(A5,D5.W),D5
		D5.w = A5.w(D5.w);
		
		BMI.S	.Vice09
		
		//LSR.L	#1,D6
		D6.l >>= 1;
		
		BCC.S	.Vice08
		BEQ.S	.Vice07
		
		//MOVE.W	(A5,D5.W),D5
		D5.w = A5.w(D5.w);
		
		BPL.S	.Vice05
		
		//CMPA.L	A1,A3
		if ((A3.src - A1.src) > 0)
		{
			//BGE.B	.DestEnd
			goto DestEnd;
		}
		
		//MOVE.B	D5,(A3)+
		D5.b = A3.b();
		
		//DBRA	D1,.Vice04
		while ((D1.l--) > -1)
		{
			goto Vice04;
		}
		
		//BRA.S	.Vice01
		goto Vice01; // unconditional
		
Vice07:	
		//MOVE.L	(A4)+,D6
		D6.l = A4.l();
		
		//CMPA.L	A0,A4
		if ((A4.src - A0.src) > 0)
		{
			//BGT.B	.SourceEnd
			goto SourceEnd;
		}
		
		//LSR.L	#1,D6
		D6.l >>= 1;
		
		BSET	D7,D6
		BCS.S	.Vice06
		
Vice08:	
		//MOVE.W	(A6,D5.W),D5
		D5.w = A6.w(D5.w);
		
		BMI.S	.Vice09
		
		//LSR.L	#1,D6
		D6.l >>= 1;
		
		BCC.S	.Vice08
		BEQ.S	.Vice07
		
		//MOVE.W	(A5,D5.W),D5
		D5.w = A5.w(D5.w);
		
		BPL.S	.Vice05
		
Vice09:	
		//CMPA.L	A1,A3
		if ((A3.src - A1.src) > 0)
		{
			//BGE.B	.DestEnd
			goto DestEnd;
		}
		
		//MOVE.B	D5,(A3)+
		A3.setb(D5);
		
		//DBRA	D1,.Vice04
		while ((D1.l--) > -1)
		{
			goto Vice04;
		}
		
		//BRA.S	.Vice01
		goto Vice01; // unconditional

DestEnd:
		//MOVEQ	#0,D0
		D0.l = 0; // set return value: false
		//RTS // return from subroutine
		//return false;

SourceEndSub:
		// increment stack-pointer (A7) by 4..
		ADDQ.L	#4,SP
		
Error:
SourceEnd:
		// set return value to true end exit
		//MOVEQ	#1,D0
		D0.l = 1; // set return value: true
		RTS // return from subroutine
		//return true;

ViceSub:	
		//MOVE.W	D4,D5
		D5.w = D4.w;

ViceS1:	
		//LSR.L	#1,D6
		D6.l >>= 1;
		
		BCC.S	.ViceS4
		BEQ.S	.ViceS3

ViceS2:	
		//MOVE.W	(A5,D5.W),D5
		D5.w = A5.w(D5.w);
		
		BMI.S	.End
		
		//LSR.L	#1,D6
		D6.l >>= 1;
		
		BCC.S	.ViceS4
		BEQ.S	.ViceS3
		
		//MOVE.W	(A5,D5.W),D5
		D5.w = A5.w(D5.w);
		
		BPL.S	.ViceS1
		RTS // return to caller..

ViceS3:	
		//MOVE.L	(A4)+,D6
		D6.l = A4.l();
		
		//CMP.L	A0,A4
		if ((A4.src - A0.src) > 0)
		{
			//BGT.B	.SourceEndSub
			goto SourceEndSub;
		}
		
		//LSR.L	#1,D6
		D6.l >>= 1;
		
		BSET	D7,D6
		BCS.S	.ViceS2

ViceS4:	
		//MOVE.W	(A6,D5.W),D5
		D5.w = A6.w(D5.w);

		BMI.S	.End
		
		//LSR.L	#1,D6
		D6.l >>= 1;
		
		BCC.S	.ViceS4
		BEQ.S	.ViceS3
		
		//MOVE.W	(A5,D5.W),D5
		D5.w = A5.w(D5.w);
		
		BPL.S	.ViceS1

End:	
		// finally returns from decrunching here?
		RTS
		END
*/
	// TODO: .. finish implement..
	return false;
}

////////// VDCO (Virtual Dreams)

bool XfdVDCO::decrunch(CReadBuffer *pOut)
{
	m_pIn->SetCurrentPos(0);
	
	A0.src = m_pIn->GetBegin(); // <- buffer address
	D0.l = m_pIn->GetSize(); // <- buffer length

	if (MakeTag((char*)A0.src) != MakeTag("VDCO"))
	{
		return false;
	}
	//TST.B	4(A0)
	if (A0.b(4) != 0)
	{
		//BNE.B	.Exit
		return false;
	}
	
	//TST.B	8(A0)
	if (A0.b(8) != 0)
	{
		//BNE.B	.Exit
		return false;
	}

	//TST.L	8(A0)
	if (A0.l(8) == 0)
	{
		//BEQ.B	.Exit
		return false;
	}

	//MOVE.L	4(A0),D0
	D0.l = A0.l(4);
	if (D0.l == 0)
	{
		//BEQ.B	.Exit
		return false;
	}

	//MOVE.L	D0,xfdrr_MinTargetLen(A1)
	//MOVE.L	D0,xfdrr_FinalTargetLen(A1)
	pOut->Reserve(D0.l);
	
	D0.l = 12;		//MOVEQ	#12,D0
	
	//ADD.L	8(A0),D0		* most files are some bytes longer
	D0.l += A0.l(8);
	
	// note: header is 12 bytes?
	// -> test current pos
	
	//MOVE.L	D0,xfdrr_MinSourceLen(A1)
	//MOVEQ	#1,D0
	//return true; // checking done (goto: RB_VDCO)

	// actual decrunching starts.. (separate method?)
//DB_VDCO		MOVEM.L	D2-D7/A2-A6,-(A7) // keep registers in stack
DB_VDCO:
	// these we can skip and we can get pointers directly
	//MOVE.L	A0,A5
	//MOVE.L	xfdbi_UserTargetBuf(A5),A1
	//MOVE.L	A1,A3
	//ADDA.L	xfdbi_TargetBufSaveLen(A5),A3
	//MOVE.L	xfdbi_SourceBuffer(A5),A0

//; A0 -> Crunched Data (source)
//; A1 -> Decrunch buffer (destination)
//; A3 -> End of decrunch buffer
	A0.src = m_pIn->GetBegin();
	A1.src = pOut->GetBegin();
	A3.src = pOut->GetEnd();

	A0.src += 12;	//LEA	12(A0),A0	; Skip header
	D7.l = 15;		//MOVEQ	#15,D7
	D6.l = 4;		//MOVEQ	#4,D6
	
	//TST.B	(A0)+
	if (A0.b() == 0)
	{
		//BEQ.B	.vdco3
		goto vdco3;
	}
	
	// used in vdco5
	D7.l = 0x1F;	//MOVEQ	#$1F,D7
	D6.l = 3;		//MOVEQ	#3,D6
	
	//BRA.B	.vdco3
	goto vdco3; // unconditional

vdco2:
	// load address with offset (skip 8 in dest)
	A4.src = A1.src + 8;	//LEA	8(A1),A4
	
	//CMP.L	A3,A4 // is dest-pos at/over end of buffer?
	if ((A4.src - A3.src) > 0)
	{
		//BHI.B	.err
		// -> no more jumping, just exit and give error
		throw IOException("address outside expected");
	}
	
	// copy 8 bytes as-is
	//MOVE.B	(A0)+,(A1)+
	//MOVE.B	(A0)+,(A1)+
	//MOVE.B	(A0)+,(A1)+
	//MOVE.B	(A0)+,(A1)+
	//MOVE.B	(A0)+,(A1)+
	//MOVE.B	(A0)+,(A1)+
	//MOVE.B	(A0)+,(A1)+
	//MOVE.B	(A0)+,(A1)+
	for (int i = 0; i < 8; i++)
	{
		A1.setb(A0);
	}
		
vdco3:		
	D0.b = A0.b();	//MOVE.B	(A0)+,D0
	if (D0.b == 0)
	{
		//BEQ.B	.vdco2
		goto vdco2;
	}
	
	D1.l = 7;		//MOVEQ	#7,D1

vdco4:	
	D0.b += D0.b;	//ADD.B	D0,D0
	
	// if carry set?
	if (D0.w & (1 << 8))
	{
		//BCS.B	.vdco5
		goto vdco5;
	}
	
	//CMP.L	A3,A1 // does not modify register values (just status-bits)
	if ((A1.src - A3.src) == 0)
	{
		//BEQ.B	.err
		// end before expected?
		throw IOException("unexpected equal address");
	}
		
	A1.setb(A0);	//MOVE.B	(A0)+,(A1)+
	
	// decrement&branch until -1
	//DBRA	D1,vdco4
	while ((D1.l--) > -1)
	{
		goto vdco4;
	}

	//BRA.B	vdco3
	goto vdco3; // unconditional branch

vdco5:
	
	D2.l = 0;			//MOVEQ	#0,D2
	D2.b = A0.b();		//MOVE.B	(A0)+,D2	; Terminator
	
	if (D2.b == 0) // test for 0 as file-end marker byte?
	{
		//BEQ.B	.end
		goto end;
	}
	
	// here D7 should be constant 0x1F or (dec) 15,
	// depending of branch to vdco3;
	// D6 should be constant 3 or 4 (dec)
	// depending on same branch.. (not changed after start)
	
	D3.l = D7.l;		//MOVE.L	D7,D3
	D3.w &= D2.w;		//AND.W	D2,D3
	D2.w <<= D6.w;		//LSL.W	D6,D2
	D2.b = A0.b();		//MOVE.B	(A0)+,D2
	// !! ^^ should have D2.w |= A0.b() ??
	// like this:
	//D2.w |= (A0.b());
	
	//A2.src = A1.src;	//MOVEA.L	A1,A2
	//A2.src -= D2.w;		//SUBA.W	D2,A2
	A2.src = (A1.src - D2.w);

	D3.w += 1;			//ADDQ.W	#1,D3
		
	A4.src = (A1.src + D3.W)+1; //LEA	1(A1,D3.W),A4
	
	//CMP.L	A3,A4
	if ((A4.src - A3.src) > 0)
	{
		//BHI.B	.err
		throw IOException("address outside expected");
	}

loop:	
	A1.setb(A2); //MOVE.B	(A2)+,(A1)+
	
	//DBRA	D3,loop // decrement&branch -> loop
	while ((D3.l--) > -1)
	{
		goto loop;
	}
	
	//DBRA	D1,vdco4 // decrement&branch -> loop
	while ((D1.l--) > -1)
	{
		goto vdco4;
	}
	
	//BRA.B	vdco3
	goto vdco3; // unconditional branch

end:
	//MOVEQ	#1,D0
	// -> just set exit status (true)
	
	//CMP.L	A3,A1
	if (A3.src == A1.src)
	{
		//BEQ.B	.ok
		goto ok;
	}
	else
	{
		//.err:		MOVEQ	#0,D0
		// -> just set exit status (false)
		
		//MOVE.W	#XFDERR_CORRUPTEDDATA,xfdbi_Error(A5)
		throw IOException("address mismatch");
	}
		
ok:
	//MOVEM.L	(A7)+,D2-D7/A2-A6 // return stack, exit ok
	//RTS
	return true;

}

////////////////////

