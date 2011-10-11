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

bool XfdByteKiller::decrunch(CReadBuffer *pOut)
{
	// implement..
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
	/*
	cmp.l	#100000,D0 -> compare to given size
	if (D0 == 100000) // side-effect on register?
	{
	}
	*/
	
	// branch-less-than-zero ?
	// blt.b	.MinMem
	
	
	//cmp.l	D1,D0
	if (D1 != D0)
	{
		// invalid size?
		return false;
	}
	
	//bgt.b	.MaxMem

	// .MinMem	
	//lsr.l	#1,D1
	D1 >>= 1;
	//add.l	D0,D1
	D1 += D0;

	//.MaxMem
	//MOVE.L	D1,xfdrr_MinTargetLen(A1)
	//MOVE.L	#-1,xfdrr_FinalTargetLen(A1)
	//MOVEQ	#1,D0
	//rts


	// finish implement..
	return false;
}

////////////////////
