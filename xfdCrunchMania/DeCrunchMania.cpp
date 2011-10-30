/**
***------------------------------------------------------------------------
*** This is the ultimate Data-Decrunch-Routine
*** for Crunch-Mania V1.4
*** (c) 1991 by FRESH THRASH of CERBERUS, all rights reserved
*** You may use this piece of code as long as you don't claim that
*** you have written it. In any case the author (me) has to be
*** mentioned someplace in your proggy.
*** Note: Source- and Destinationaddresses have to be always even Addresses
***------------------------------------------------------------------------
*** Here is the Format of the Header:
*** Type  Offset  Contents                   Function
*** LONG  0       "CrM!"                     to recongnize crunched files
*** WORD  4       Minimum Security Distance  to savely decrunch data when
***					     Source and Dest is in the same
***					     Memoryblock
*** LONG  6       Original Len               Datalen before packing
*** LONG  10 ($a) Crunched Len               Datalen after packing without
***					     Header
**/
//
// M68k-assembler to C++ library by Ilkka Prusi <ilkka.prusi@gmail.com>
//

#include "DeCrunchMania.h"

bool DeCrunchMania::decrunch(CReadBuffer *pOut)
{
	// split into methods to keep track of those labels..

	OverlapDecrunch(pOut);
	return true;
}

/*
***------------------------------------------------------------------------
** Jump here to decrunch some data with overlap check
** You need some Memory directly in front of the Destination Area
** which has to be as large as the MinSecDist
** Load the Regs with:
** a0: Adr of Source (with Header)	** a1: Adr of Dest
**---------------------------------------------------------------
*/
void DeCrunchMania::OverlapDecrunch(CReadBuffer *pOut)
{
//OverlapDecrunch:
	//movem.l	d0-d7/a0-a6,-(sp) // stack -> skip
	
	//cmp.l	#"CrM!",(a0)+ // already in constructor/before calling this
	//bne.s	.NotCrunched
	
	//moveq	#0,d0
	D0.l = 0;
	
	//move.w	(a0)+,d0	;MinSecDist
	D0.w = A0.w();
	
	//move.l	(a0)+,d1	;DestLen
	D1.l = A0.l();
	m_destLen = D1.l; // keep this
	pOut->Reserve(m_destLen);
	
	//move.l	(a0)+,d2	;SrcLen
	D2.l = A0.l();
	m_sourceLen = D2.l; // keep this
	
	//lea	0(a0,d0.l),a2
	A2.src = (A0.src + D0.l); // MinSecDist ?
	
	//cmp.l	a1,a2
	if ((A2.src - A1.src) <= 0)
	{
		//ble.s	.NoCopy
		//goto NoCopy1;
		
		//move.l	a0,a2
		A2.src = A0.src;
		//bsr.s	FastDecruncher
		FastDecruncher();
		return;
	}
	
	//move.l	a0,a2
	A2.src = A0.src;
	
	//move.l	a1,a0
	A0.src = A1.src;
	
	//sub.l	d0,a0		;MinSecDist abziehen
	A0.src -= D0.l;
	
	//move.l	a0,a3
	A3.src = A0.src;

	//move.l	d2,d7
	D7.l = D2.l;
	
	//lsr.l	#2,d7		;Longs
	D7.l >>= 2;

//CopyLoop1:
	do
	{
		//move.l	(a2)+,(a3)+
		A3.setl(A2);
		
		//subq.l	#1,d7
		D7.l -= 1;
	} while (D7.l != 0);
	//bne.s	CopyLoop1
	
	//move.l	(a2)+,(a3)+	;in case of ...
	A3.setl(A2);


//NoCopy1:
	//move.l	a0,a2
	A2.src = A0.src;
	//bsr.s	FastDecruncher
	FastDecruncher();
	//return; // not needed here..

//NotCrunched:
	//movem.l	(sp)+,d0-d7/a0-a6 // restore stack
	//rts
}

/*
**-----------------------------------------------------------
** Jump here to decrunch some data without any overlap checks
** The Regs have to loaded with:
** a0: Adr of Source (with Header)
** a1: Adr of Dest
**-----------------------------------------------------------
*/
void DeCrunchMania::NormalDecrunch(CReadBuffer *pOut)
{
//NormalDecrunch:
	//movem.l	d0-d7/a0-a6,-(sp) // keep stack
	//cmp.l	#"CrM!",(a0)+
	/*
	if (::memcmp("CrM!", A0.src, 4) != 0) // already in constructor, removed..
	{
		//bne.s	NotCrunched
		return;
	}
	A0.src += 4;
	*/
	
	// no action on test result, just for incrementing position??
	// -> just do that then..
	//tst.w	(a0)+		;skip MinSecDist
	A0.src += 2;
	
	//move.l	(a0)+,d1	;OrgLen
	D1.l = A0.l(); // original length?
	m_destLen = D1.l;
	pOut->Reserve(m_destLen);
	
	//move.l	(a0)+,d2	;CrLen
	D2.l = A0.l(); // crunched length?
	m_sourceLen = D2.l; // 
	
	//move.l	a0,a2
	A2.src = A0.src;
	
	//bsr.s	FastDecruncher
	FastDecruncher();
	//return; // not needed here..
	
//NotCrunched:
	//movem.l	(sp)+,d0-d7/a0-a6 // restore stack
	//rts
}
	
/*
**-------------------------------------------------------------------
** This is the pure Decrunch-Routine
** The Registers have to be loaded with the following values:
** a1: Adr of Destination (normal)	** a2: Adr of Source (packed)
** d1: Len of Destination		** d2: Len of Source
**-------------------------------------------------------------------
*/
void DeCrunchMania::FastDecruncher()
{
//FastDecruncher:
	//move.l	a1,a5			;Decrunched Anfang (hier Ende des Decrunchens)
	A5.src = A1.src;
	
	//add.l	d1,a1
	A1.src += D1.l;
	
	//add.l	d2,a2
	A2.src += D2.l;
	
	//move.w	-(a2),d0		;Anz Bits in letztem Wort
	D0.w = A2.wM();
	
	//move.l	-(a2),d6		;1.LW
	D6.l = A2.lM();
	
	//moveq	#16,d7			;Anz Bits
	D7.l = 16;
	
	//sub.w	d0,d7			;Anz Bits, die rotiert werden müssen
	D7.w -= D0.w;
	
	//lsr.l	d7,d6			;1.Bits an Anfang bringen
	D6.l >>= D7.l;
	
	//move.w	d0,d7			;Anz Bits, die noch im Wort sind
	D7.w = D0.w;
	
	//moveq	#16,d3
	D3.l = 16;
	
	//moveq	#0,d4
	D4.l = 0;
	
	/*
DecrLoop:
	//cmp.l	a5,a1
	if ((A1.src - A5.src) <= 0)
	{
		//ble.L	DecrEnd			;a1=a5: fertig (a1<a5: eigentlich Fehler)
		goto DecrEnd;
	}

	//bsr.s	BitTest
	BitTest();
	
	bcc.s	InsertSeq		;1.Bit 0: Sequenz
	
	//moveq	#0,d4
	D4.l = 0;

// ** einzelne Bytes einfügen ***
InsertBytes:
	moveq	#8,d1
	bsr.w	GetBits
	move.b	d0,-(a1)
	dbf	d4,InsertBytes
	//bra.s	DecrLoop
	goto DecrLoop;

// *------------
SpecialInsert:
	//moveq	#14,d4
	moveq(14, D4);
	//moveq	#5,d1
	moveq(5, D1);
	
	bsr.s	BitTest
	bcs.s	IB1
	//moveq	#14,d1
	moveq(14, D1);
IB1:	bsr.s	GetBits
	add.w	d0,d4
	bra.s	InsertBytes
// *------------
InsertSeq:
// ** Anzahl der gleichen Bits holen **
	bsr.s	BitTest
	bcs.s	AB1
	moveq	#1,d1			;Maske: 0 (1 AB)
	moveq	#1,d4			;normal: Summe 1
	bra.s	ABGet
AB1:
	bsr.s	BitTest
	bcs.s	AB2
	moveq	#2,d1			;Maske: 01 (2 ABs)
	moveq	#3,d4			;ab hier: Summe mindestens 3
	bra.s	ABGet
AB2:
	bsr.s	BitTest
	bcs.s	AB3
	moveq	#4,d1			;Maske: 011 (4 ABs)
	moveq	#7,d4			;hier: Summe 11
	bra.s	ABGet
AB3:
	moveq	#8,d1			;Maske: 111 (8 ABs)
	moveq	#$17,d4			;hier: Summe 11
ABGet:
	bsr.s	GetBits
	add.w	d0,d4			;d0: Länge der Sequenz - 1
	cmp.w	#22,d4
	beq.s	SpecialInsert
	blt.s	Cont
	subq.w	#1,d4
Cont:
// ** SequenzAnbstand holen **
	bsr.s	BitTest
	bcs.s	DB1
	moveq	#9,d1			;Maske: 0 (9 DBs)
	moveq	#$20,d2
	bra.s	DBGet
DB1:
	bsr.s	BitTest
	bcs.s	DB2
	moveq	#5,d1			;Maske: 01 (5 DBs)
	moveq	#0,d2
	bra.s	DBGet
DB2:
	moveq	#14,d1			;Maske: 11 (12 DBs)
	move.w	#$220,d2
DBGet:
	bsr.s	GetBits
	add.w	d2,d0
	lea	0(a1,d0.w),a3		;a3 auf Anf zu kopierender Seq setzten
InsSeqLoop:
	move.b	-(a3),-(a1)		;Byte kopieren
	dbf	d4,InsSeqLoop

	bra.w	DecrLoop
*/

	

/*
// *-----------
DecrEnd:
	rts		;a5: Start of decrunched Data
*/
}

void DeCrunchMania::BitTest()
{
//BitTest:
	//subq.w	#1,d7
	D7.w -= 1;
	if (D7.w != 0)
	{
		//bne.s	BTNoLoop <- inline instead of jumping to one statement..
		//lsr.l	#1,d6			;Bit rausschieben und Flags setzen
		D6.l >>= 1;
		return; // rts in BTNoLoop too
	}
	
	//moveq	#16,d7			;hier kein add notwendig: d7 vorher 0
	moveq(16, D7);
	
	//move.w	d6,d0
	D0.w = D6.w;
	
	//lsr.l	#1,d6			;Bit rausschieben und Flags setzen
	D6.l >>= 1;
	
	//swap	d6			;ror.l	#16,d6
	swap(D6);
	
	//move.w	-(a2),d6		;nächstes Wort holen
	D6.w = A2.wM();
	
	//swap	d6			;rol.l	#16,d6
	swap(D6);
	
	//lsr.w	#1,d0			;Bit rausschieben und Flags setzen
	D0.w >>= 1;
	
	// TODO: cc/cs into ccr
	//rts
}

void DeCrunchMania::GetBits()
{
//GetBits:				;d1:AnzBits->d0:Bits
	//move.w	d6,d0			;d6:Akt Wort
	D0.w = D6.w;
	//lsr.l	d1,d6			;nächste Bits nach vorne bringen
	D6.l >>= D1.w;
	//sub.w	d1,d7			;d7:Anz Bits, die noch im Wort sind
	D7.w -= D1.w;
	
	if (D7.w > 0)
	{
		//bgt.s	GBNoLoop
		GBNoLoop();
		return;
	}
	
	// first was commented-out in original too..
//;	add.w	#16,d7			;BitCounter korrigieren
	//add.w	d3,d7			;BitCounter korrigieren
	D7.w += D3.w;
	
	//ror.l	d7,d6			;restliche Bits re rausschieben
	ror(D7.w, D6);
	
	//move.w	-(a2),d6		;nächstes Wort holen
	D6.w = A2.wM();
	
	//rol.l	d7,d6			;und zurückrotieren
	rol(D7.w, D6);

	GBNoLoop(); // continued to "below" in asm?
}

void DeCrunchMania::GBNoLoop()
{
/*
	//add.w	d1,d1			;*2 (in Tab sind Ws)
	D1.w += D1.w;
	
	and.w	AndData-2(pc,d1.w),d0	;unerwünschte Bits rausschmeißen
	
// *----------
AndData:
	dc.w	%1,%11,%111,%1111,%11111,%111111,%1111111
	dc.w	%11111111,%111111111,%1111111111
	dc.w	%11111111111,%111111111111
	dc.w	%1111111111111,%11111111111111
*/
}

