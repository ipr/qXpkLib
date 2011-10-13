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

bool XfdByteKiller::crun(CReadBuffer *pOut, uint8_t *src, uint32_t D0)
{
	datareg D1, D2, D3, D4, D5, D6, D7;
	addrreg A0, A1, A2, A3, A4, A5, A6, A7;
	A0.src = src;
	
	// in caller
	//cmp.l	#'CRUN',(A0)+
	//bne.s	.Exit
	//A0 = A0+4;
	
	//tst.b	(A0)
	//bne.b	.Exit
	if (*A0.src != 0)
	{
		return false;
	}
	
	//move.l	(A0)+,D1
	D1.l = A0.l;
		
	//beq.b	.Exit
	if (D1.b == 0)
	{
		return false;
	}
	
	//btst	#0,D1
	//bne.b	.Exit
	if ((D1 & 1) != 0)
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
	if (*A0.src != 0)
	{
		return false;
	}
		
	//move.l	(A0),D1
	D1.l = *A0;
	
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

bool XfdByteKiller::crnd(CReadBuffer *pOut, uint8_t *src, uint32_t D0)
{
	datareg D1, D2, D3, D4, D5, D6, D7;
	addrreg A0, A1, A2, A3, A4, A5, A6, A7;
	A0.src = src;

	return false;
}

bool XfdByteKiller::marc(CReadBuffer *pOut, uint8_t *src, uint32_t D0)
{
	datareg D1, D2, D3, D4, D5, D6, D7;
	addrreg A0, A1, A2, A3, A4, A5, A6, A7;
	A0.src = src;

	return false;
}

bool XfdByteKiller::decrunch(CReadBuffer *pOut)
{
	m_pIn->SetCurrentPos(0);
	
	uint8_t *pA0 = m_pIn->GetBegin(); // <- buffer address
	uint32_t D0 = m_pIn->GetSize(); // <- buffer length

	uint32_t tag = GetULong(pA0);
	if (tag == MakeTag("CRUN"))
	{
		//cmp.l	#'CRUN',(A0)+
		//bne.s	.Exit
		//A0 = A0+4;
	
		return crun(pOut, pA0+4, D0);
	}
	if (tag == MakeTag("CRND"))
	{
		return crnd(pOut, pA0+4, D0);
	}
	if (tag == MakeTag("MARC")
		|| tag == MakeTag("TMB!")
		|| tag == MakeTag("TXIC")
		|| tag == MakeTag("SCC!")
		|| tag == MakeTag("ons ")
		|| tag == MakeTag(" Sym")
		|| tag == MakeTag("Gary"))
	{
		return marc(pOut, pA0+4, D0);
	}
	// .. and others..

	return false;
}

///////// Vice

bool XfdVice::decrunch(CReadBuffer *pOut)
{
	m_pIn->SetCurrentPos(0);
	
	uint8_t *pA0 = m_pIn->GetBegin(); // <- buffer address
	uint32_t D0 = m_pIn->GetSize(); // <- buffer length
	
	if (GetULong(pA0) != MakeTag("Vice"))
	{
		return false;
	}
	
	uint8_t b = (*(pA0 + 4));
	if (b != 0x80)
	{
		return false;
	}
	
	uint32_t D1 = 0x80000;
	//cmp.l	#100000,D0 -> compare to given size
	if ((D0 - 100000) < 0)
	{
		// file smaller than ~100kbytes?
		// blt.b	.MinMem
		//lsr.l	#1,D1
		D1 >>= 1;
		//add.l	D0,D1
		D1 += D0;
	}
	else if ((D0 - D1) > 0)
	{
		//cmp.l	D1,D0
		//bgt.b	.MaxMem
		//.MaxMem
		//MOVE.L	D1,xfdrr_MinTargetLen(A1)
		pOut->Reserve(D1);
		//MOVE.L	#-1,xfdrr_FinalTargetLen(A1)
		//MOVEQ	#1,D0
		//D0 = 1; // status before exit? (1, true)
		//rts
	}
	
	// checks done, decrunch..

//label DB_Vice:
	//MOVEM.L	D2-D7/A2-A6,-(SP) // (move listed registers)
	//MOVEA.L	A0,A2
	//MOVE.L	xfdbi_SourceBuffer(A2),A6
	//MOVE.L	xfdbi_UserTargetBuf(A2),A3	; dest start
	uint8_t *A3 = pOut->GetBegin();
	
	//MOVEA.L	A6,A0
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
	
	// TODO: .. finish implement..
	return false;
}

////////////////////
