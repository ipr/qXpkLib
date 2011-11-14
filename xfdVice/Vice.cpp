///////////////////////////////////
//
// XFD-decruncer for Vice
// Multiple authors:
/*
*******************************************************
**     XFD external decruncher for Vice Cruncher     **
**        written by Mr. Larmer / Wanted Team        **
**	   shortened and fixed by SDI in 1999  	     **
**	   and fixed by Don Adan/WT in 2000  	     **
*******************************************************
*/
//
// M68k-assembler to C++ library by Ilkka Prusi <ilkka.prusi@gmail.com>
//


#include "Vice.h"

bool Vice::decrunch(CReadBuffer *pOut)
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
	
	//BSR.B	D_Vice // <- may return here
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
