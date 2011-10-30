///////////////////////////////////
//
// XFD-decruncer for VDCO (Virtual Dreams)
// Multiple authors:
/*
*******************************************************
**     XFD external decruncher for VDCO Cruncher     **
**        written by Mr. Larmer / Wanted Team        **
**	   shortened and fixed by SDI in 1999  	     **
*******************************************************
*/
//
// M68k-assembler to C++ library by Ilkka Prusi <ilkka.prusi@gmail.com>
//

#include "VDCO.h"

bool VDCO::decrunch(CReadBuffer *pOut)
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

