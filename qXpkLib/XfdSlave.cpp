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


#include "XfdSlave.h"

///////// base is pure virtual for now

///////// ByteKiller

bool XfdByteKiller::crun(CReadBuffer *pOut, uint8_t *src, uint32_t size)
{
	D0.l = size;
	A0.src = src;
	
	// in caller
	//cmp.l	#'CRUN',(A0)+
	//bne.s	.Exit
	//A0 = A0+4;
	
	//tst.b	(A0)
	//bne.b	.Exit
	if (*(A0.src) != 0)
	{
		return false;
	}
	
	//move.l	(A0)+,D1
	D1.l = A0.l();
		
	//beq.b	.Exit
	if (D1.b == 0)
	{
		return false;
	}
	
	//btst	#0,D1
	//bne.b	.Exit
	if ((D1.b & 1) != 0)
	{
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
	D1.l = A0.l();
	
	//beq.b	.Exit
	//bmi.b	.Exit
	if (D1.l <= 0)
	{
		return false;
	}
	
	//move.l	D1,xfdrr_MinTargetLen(A1)
	//move.l	D1,xfdrr_FinalTargetLen(A1)
	pOut->Reserve(D1.l);
	
	// just flag for exit -> return true
	//moveq	#1,d0
	//D0.l = 1;
	return true;

/* // just flag for exit -> return false		
.Exit		moveq	#0,d0
		rts
	return false;
*/

}

bool XfdByteKiller::crnd(CReadBuffer *pOut, uint8_t *src, uint32_t size)
{
	D0.l = size;
	A0.src = src;

	return false;
}

bool XfdByteKiller::marc(CReadBuffer *pOut, uint8_t *src, uint32_t size)
{
	D0.l = size;
	A0.src = src;

	return false;
}

bool XfdByteKiller::xvdg(CReadBuffer *pOut, uint8_t *src, uint32_t size)
{
	D0.l = size;
	A0.src = src;

	return false;
}

/*
; Amos Pro CLI
; Amos Pro Wb
; Amos Wb
; Amos CLI
bool XfdByteKiller::amos(CReadBuffer *pOut, uint8_t *src, uint32_t size)
{
	D0.l = size;
	A0.src = src;

	return false;
}
*/

bool XfdByteKiller::arpf(CReadBuffer *pOut, uint8_t *src, uint32_t size)
{
	D0.l = size;
	A0.src = src;

	return false;
}

bool XfdByteKiller::arp3(CReadBuffer *pOut, uint8_t *src, uint32_t size)
{
	D0.l = size;
	A0.src = src;

	return false;
}

// Pepsi ACE
bool XfdByteKiller::ace(CReadBuffer *pOut, uint8_t *src, uint32_t size)
{
	D0.l = size;
	A0.src = src;

	return false;
}

bool XfdByteKiller::pack(CReadBuffer *pOut, uint8_t *src, uint32_t size)
{
	D0.l = size;
	A0.src = src;

	return false;
}

bool XfdByteKiller::decrunch(CReadBuffer *pOut)
{
	m_pIn->SetCurrentPos(0);
	
	uint8_t *pA0 = m_pIn->GetBegin(); // <- buffer address
	uint32_t size = m_pIn->GetSize(); // <- buffer length

	//D0.l = size;
	//A0.src = src;


	uint32_t tag = GetULong(pA0);
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
	
	/* AMOS .. common hunk header (executable) ? */

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
	
	//datareg D0, D1;
	//addrreg A0;
	A0.src = m_pIn->GetBegin(); // <- buffer address
	D0.l = m_pIn->GetSize(); // <- buffer length
	
	if (GetULong(A0.src) != MakeTag("Vice"))
	{
		return false;
	}
	
	//CMP.B	#$80,4(A0)
	if (A0.b(4) != 0x80)
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
	//MOVE.L	xfdbi_SourceBuffer(A2),A6
	A6.src = A0.src;
	
	//MOVE.L	xfdbi_UserTargetBuf(A2),A3	; dest start
	A3.src = pOut->GetBegin();
	
	//MOVEA.L	A6,A0
	A0.src = A6.src;
	
	//ADD.L		xfdbi_SourceBufLen(A2),A0	; source end
	//ADDQ.L	#4,A6				; source start
	//MOVE.L	A3,A1
	//ADD.L		xfdbi_UserTargetBufLen(A2),A1	; dest end

	//MOVE.L	A3,D2
	//BSR.B	D_Vice
	//MOVE.W	#XFDERR_CORRUPTEDDATA,xfdbi_Error(A2)

	//SUBA.L	D2,A3
	//MOVE.L	A3,xfdbi_TargetBufSaveLen(A2)

	//; D0 already set
	//MOVEM.L	(SP)+,D2-D7/A2-A6
	//RTS
	
/*
; A6 -> Crunched data start, past header (source)
; A0 -> Crunched data end (source)
; A3 -> Decrunch buffer start (destination)
; A1 -> Decrunch buffer end (destination)

; Uses: A0,A1,A3,A4,A5,A6, D0,D1,D3,D4,D5,D6,D7

D_Vice		LEA	$200(A6),A5
		LEA	$400(A6),A4
		MOVEQ	#1,D6
		MOVE.W	$1FE(A6),D4
		MOVEQ	#$1F,D7
		MOVEQ	#7,D3

.Vice01:	MOVEQ	#0,D1
		BSR.S	.ViceSub
		TST.B	D5
		BEQ.S	.Error
		MOVE.B	D5,D1
		BPL.S	.Vice03
		BCLR	D3,D1
		SUBQ.B	#1,D1
		BSR.S	.ViceSub
.Vice02:	CMPA.L	A1,A3
		BGE.B	.DestEnd
		MOVE.B	D5,(A3)+
		DBRA	D1,.Vice02
		BRA.S	.Vice01
.Vice03:	SUBQ.B	#1,D1
.Vice04:	MOVE.W	D4,D5
.Vice05:	LSR.L	#1,D6
		BCC.S	.Vice08
		BEQ.S	.Vice07
.Vice06:	MOVE.W	(A5,D5.W),D5
		BMI.S	.Vice09
		LSR.L	#1,D6
		BCC.S	.Vice08
		BEQ.S	.Vice07
		MOVE.W	(A5,D5.W),D5
		BPL.S	.Vice05
		CMPA.L	A1,A3
		BGE.B	.DestEnd
		MOVE.B	D5,(A3)+
		DBRA	D1,.Vice04
		BRA.S	.Vice01
.Vice07:	MOVE.L	(A4)+,D6
		CMPA.L	A0,A4
		BGT.B	.SourceEnd
		LSR.L	#1,D6
		BSET	D7,D6
		BCS.S	.Vice06
.Vice08:	MOVE.W	(A6,D5.W),D5
		BMI.S	.Vice09
		LSR.L	#1,D6
		BCC.S	.Vice08
		BEQ.S	.Vice07
		MOVE.W	(A5,D5.W),D5
		BPL.S	.Vice05
.Vice09:	CMPA.L	A1,A3
		BGE.B	.DestEnd
		MOVE.B	D5,(A3)+
		DBRA	D1,.Vice04
		BRA.S	.Vice01

.DestEnd	MOVEQ	#0,D0
		RTS

.SourceEndSub	ADDQ.L	#4,SP
.Error
.SourceEnd	MOVEQ	#1,D0
		RTS

.ViceSub:	MOVE.W	D4,D5
.ViceS1:	LSR.L	#1,D6
		BCC.S	.ViceS4
		BEQ.S	.ViceS3
.ViceS2:	MOVE.W	(A5,D5.W),D5
		BMI.S	.End
		LSR.L	#1,D6
		BCC.S	.ViceS4
		BEQ.S	.ViceS3
		MOVE.W	(A5,D5.W),D5
		BPL.S	.ViceS1
		RTS
.ViceS3:	MOVE.L	(A4)+,D6
		CMP.L	A0,A4
		BGT.B	.SourceEndSub
		LSR.L	#1,D6
		BSET	D7,D6
		BCS.S	.ViceS2
.ViceS4:	MOVE.W	(A6,D5.W),D5
		BMI.S	.End
		LSR.L	#1,D6
		BCC.S	.ViceS4
		BEQ.S	.ViceS3
		MOVE.W	(A5,D5.W),D5
		BPL.S	.ViceS1
.End:		RTS
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

	if (GetULong(A0.src) != MakeTag("VDCO"))
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
	//MOVEQ	#12,D0
	D0.l = 12;
	
	//ADD.L	8(A0),D0		* most files are some bytes longer
	D0.l += A0.l(8);
	
	//MOVE.L	D0,xfdrr_MinSourceLen(A1)
	//MOVEQ	#1,D0
	//return true; // checking done (RB_VDCO)

/*
DB_VDCO		MOVEM.L	D2-D7/A2-A6,-(A7)
		MOVE.L	A0,A5
		MOVE.L	xfdbi_UserTargetBuf(A5),A1
		MOVE.L	A1,A3
		ADDA.L	xfdbi_TargetBufSaveLen(A5),A3
		MOVE.L	xfdbi_SourceBuffer(A5),A0

; A0 -> Crunched Data (source)
; A1 -> Decrunch buffer (destination)
; A3 -> End of decrunch buffer

		LEA	12(A0),A0	; Skip header
		MOVEQ	#15,D7
		MOVEQ	#4,D6
		TST.B	(A0)+
		BEQ.B	.vdco3
		MOVEQ	#$1F,D7
		MOVEQ	#3,D6
		BRA.B	.vdco3

.vdco2		LEA	8(A1),A4
		CMP.L	A3,A4
		BHI.B	.err
		MOVE.B	(A0)+,(A1)+
		MOVE.B	(A0)+,(A1)+
		MOVE.B	(A0)+,(A1)+
		MOVE.B	(A0)+,(A1)+
		MOVE.B	(A0)+,(A1)+
		MOVE.B	(A0)+,(A1)+
		MOVE.B	(A0)+,(A1)+
		MOVE.B	(A0)+,(A1)+
.vdco3		MOVE.B	(A0)+,D0
		BEQ.B	.vdco2
		MOVEQ	#7,D1
.vdco4		ADD.B	D0,D0
		BCS.B	.vdco5
		CMP.L	A3,A1
		BEQ.B	.err
		MOVE.B	(A0)+,(A1)+
		DBRA	D1,.vdco4
		BRA.B	.vdco3

.vdco5		MOVEQ	#0,D2
		MOVE.B	(A0)+,D2	; Terminator
		BEQ.B	.end
		MOVE.L	D7,D3
		AND.W	D2,D3
		LSL.W	D6,D2
		MOVE.B	(A0)+,D2
		MOVEA.L	A1,A2
		SUBA.W	D2,A2
		ADDQ.W	#1,D3
		LEA	1(A1,D3.W),A4
		CMP.L	A3,A4
		BHI.B	.err
.loop		MOVE.B	(A2)+,(A1)+
		DBRA	D3,.loop
		DBRA	D1,.vdco4
		BRA.B	.vdco3

.end		MOVEQ	#1,D0
		CMP.L	A3,A1
		BEQ.B	.ok
.err		MOVEQ	#0,D0
		MOVE.W	#XFDERR_CORRUPTEDDATA,xfdbi_Error(A5)
.ok		MOVEM.L	(A7)+,D2-D7/A2-A6
		RTS

		END
*/

}

////////////////////

