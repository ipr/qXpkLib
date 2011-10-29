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

/*
// contains all needed info?
	STRUCTURE NukeData,0
	APTR	inbuf
	APTR	outbuf
	APTR	last
	APTR	next
	APTR	cwri
	APTR	uwri
	APTR	uend
	APTR	delpos
	UWORD	ulen
	UWORD	clen
	UWORD	ustop
	UWORD	ustart
	UWORD	ucount
	UWORD	scanrange
	UWORD	room1
	UWORD	room2
	UWORD	room4
	UWORD	roomN
	UWORD	data1
	UWORD	data2
	ULONG	data4
	ULONG	dataN
	APTR	dest1
	APTR	dest2
	APTR	dest4
	APTR	destN
	ULONG	dummy
	APTR	contbuf
	ULONG	flags
	LABEL	nd_sizeof
 */
 
struct NukeData 
{
	char	*inbuf;
	char	*outbuf;
	char	*last;
	char	*next;
	char		*cwri;
	char		*uwri;
	char		*uend;
	char		*delpos;
	uint16_t	ulen;
	uint16_t	clen;
	uint16_t	ustop;
	uint16_t	ustart;
	uint16_t	ucount;
	uint16_t	scanrange;
	uint16_t	room1;
	uint16_t	room2;
	uint16_t	room4;
	uint16_t	roomN;
	uint16_t	data1;
	uint16_t	data2;
	uint32_t	data4;
	uint32_t	dataN;
	char		*dest1;
	char		*dest2;
	char		*dest4;
	char		*destN;
	uint32_t	dummy;
	char		*contbuf;
	uint32_t	flags;
};

//starts:	dc.w	0,0,4,10
uint16_t starts[] = {0,0,4,10}; 

uint16_t modes[] = {4,6,8,9,4,7,9,11,13,14,5,7,9,11,13,14};


/*
dists:	dc.w	$0010,$0040,$0100,$0200
	dc.w	$0010,$0080,$0200,$0800,$2000,$4000
	dc.w	$0020,$0080,$0200,$0800,$2000,$4000

	dc.w	4,6,8,9
	dc.w	4,7,9,11,13,14
	dc.w	5,7,9,11,13,14

	dc.w	0,1,2,3
	dc.w	4,5,6,7,8,9
	dc.w	10,11,12,13,14,15


adds	dc.w	$0000,$0010,$0050,$0150
	dc.w	$0000,$0010,$0090,$0290,$0a90,$2a90
	dc.w	$0000,$0020,$00a0,$02a0,$0aa0,$2aa0

	dc.w	c2-TwoBitLen,c2-TwoBitLen,c2-TwoBitLen,c2-TwoBitLen
	dc.w	c3-TwoBitLen,c3-TwoBitLen,c3-TwoBitLen,c3-TwoBitLen
	dc.w	c3-TwoBitLen,c3-TwoBitLen
	dc.w	0,0,0,0,0,0
*/

/*
WRITE0:	MACRO
30$	SUBQ.W	#1,room1(A4)
	BCC.B	90$

	move.l	dest1(a4),a0
	move.w	data1(a4),(a0)
	move.l	cwri(a4),dest1(a4)
	addq.l	#2,cwri(a4)
	move.w	#15,room1(a4)

90$	asl.w	data1(a4)
	ENDM

WRITE1:	MACRO
31$	subq.w	#1,room1(a4)
	bcc.s	91$

	move.l	dest1(a4),a0
	move.w	data1(a4),(a0)
	move.l	cwri(a4),dest1(a4)
	addq.l	#2,cwri(a4)
	move.w	#15,room1(a4)

91$	asl.w	data1(a4)
	addq.w	#1,data1(a4)
	ENDM



WRITE2:	MACRO
32$	subq.w	#2,room2(a4)
	bcc.s	92$

	move.l	dest2(a4),a0
	move.w	data2(a4),(a0)
	move.l	cwri(a4),dest2(a4)
	addq.l	#2,cwri(a4)
	move.w	#14,room2(a4)

92$	asl.w	data2(a4)
	asl.w	data2(a4)
	or.w	arg,data2(a4)
	ENDM



WRITE4:	MACRO
34$	subq.w	#4,room4(a4)
	bcc.s	94$

	move.l	dest4(a4),a0
	move.l	data4(a4),(a0)
	move.l	cwri(a4),dest4(a4)
	addq.l	#4,cwri(a4)
	move.w	#28,room4(a4)
	clr.l	data4(a4)

94$	move.l	data4(a4),tmp
	or.w	arg,tmp
	ror.l	#4,tmp
	move.l	tmp,data4(a4)
	ENDM



WRITEN:	MACRO			; d0,arg,a0
38$	moveq	#0,d0
	move.l	dataN(a4),d0
	asl.l	arg,d0
	or.l	off,d0

	sub.w	arg,roomN(a4)
	bcc.s	99$

	move.w	roomN(a4),arg
	neg.w	arg
	ror.l	arg,d0
	move.l	destN(a4),a0
	move.w	d0,(a0)
	move.l	cwri(a4),destN(a4)
	addq.l	#2,cwri(a4)
	rol.l	arg,d0
	neg.w	arg
	add.w	#16,arg
	move.w	arg,roomN(a4)

99$	move.l	d0,dataN(a4)
	ENDM



UNCOMP:	MACRO
	addq.w	#1,ucount(a4)
	bne.s	9$
	move.w	i,ustart(a4)	; first uncompressed
9$
	ENDM

WRUNCO:			; d2=number, Cc,  -d0, -a0, -a1
;	movem.l	a0-a6/d0-d7,-(sp)	; DEBUG
;	bsr	_stat2
;	movem.l	(sp)+,a0-a6/d0-d7

	WRITE0
	move.w	#-1,ucount(a4)	; reset ucount

	move.w	ustart(a4),d0	; write bytes
	lea	-1(buf,d0.w),a0
	move.l	uwri(a4),a1
	move.w	d7,d0
;;;	lsr.w	#1,d0
;;;	bcc.s	86$
85$	move.b	(a0)+,-(a1)
;;;86$	move.b	(a0)+,-(a1)
	dbra	d0,85$
	move.l	a1,uwri(a4)

	tst.w	d7
	bne.s	long
	WRITE1
	bra	uexit	; len OK

long	move.w	d7,d0
	WRITE0
loop	cmp.w	#3,d0
	ble.s	last3
	moveq	#0,d7
	WRITE2
	subq.w	#3,d0
	bra.s	loop

last3	neg.w	d0
	moveq	#3,d7
	and.w	d0,d7
	WRITE2

uexit	rts

*/

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
	// it's never changed anyway so use constant instead
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

	//bsr	TestCompressed
	goto TestCompressed;
	
	//move.l	a0,d0
	D0.l = A0.src; // ?? position for length? -> must change for 64-bit..
	
	//movem.l	(a7)+,d2-d7/a3-a6 // restore stack
	return true;
	//rts

//; d0 mode		a0 writepos
//; d1 offslen/offset	a1 copysrc
//; d2 1bit_data		a2 writeend
//; d3 2bits_data		a3 16

//; d4 4bits_data		a4 byteread
//; d5 4bits_in		a5 wordread
//; d6 xbits_data		a6 table
//; d7 -xbits_in		a7

/*
Compressed:
	dbra	d5,NoRead4		; 70	Read 4 bits (mode)
	moveq	#7,d5			; 8
	move.l	(a5)+,d4		; |
	add.l	d4,d4			; |
NoRead4:				;
	moveq	#30,d0			; 70
	and.w	d4,d0			; |
	roxr.l	#4,d4			; |
					; |
	move.w	0(a6,d0.w),d1		; |
					; |
	clr.w	d6			; |
	rol.l	d1,d6			; |	Read n bits (offset)
	add.w	d1,d7			; |     ble jumps on N|(Z^V)
	ble.s	NoReadN			; |	the add does never set V
	moveq	#0,d1			; 35
	move.w	(a5)+,d1		; |
	asl.l	d7,d1			; |	
	swap	d1			; |
	or.l	d1,d6			; |
	
	//sub.w	a3,d7			; |
	D7.w -= A3_val; // replaced with constant (see before)
	
NoReadN:				;
	move.l	a0,a1			; 70
	add.w	32(a6,d0.w),d6		; |
	sub.w	d6,a1			; |
					; |
	move.w	64(a6,d0.w),d0		; |
	jmp	TwoBitLen(pc,d0.w)

TwoBitLen:
	move.b	(a1)+,(a0)+		; 21
	move.b	(a1)+,(a0)+		; |
	move.b	(a1)+,(a0)+		; |
					; |
	moveq	#0,d0			; |	Read 2 bits (clen1)
	add.w	d3,d3			; |
	bne.s	NoRead2x		; |
	move.w	(a5)+,d3		; 1
	addx.w	d3,d3			; |
NoRead2x:				;
	addx.w	d0,d0			; 9
	add.w	d3,d3			; |
	addx.w	d0,d0			; |

	add.w	d0,d0
	jmp	CJTable(pc,d0.w)
CJTable:
	bra.s	Tab15
c3:	move.b	(a1)+,(a0)+		; 21
c2:	move.b	(a1)+,(a0)+		; 21
	move.b	(a1)+,(a0)+		; |

//;----------------------------------------
TestCompressed:				;
	add.w	d2,d2			; 70	Read 1 (cbit)
	bcc.s	Uncompressed		; |
	bne.s	Compressed		; |
	move.w	(a5)+,d2		; 4
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
TestExit:				; |
	cmp.l	a2,a0			; |
	blt	Compressed		; |
	rts

CopyThree:
	move.b	-(a4),(a0)+		; 9
	move.b	-(a4),(a0)+		; |
	move.b	-(a4),(a0)+		; |
Copy3Entry:
	moveq	#0,d0			; 30	Read 2 (ulen2)
	add.w	d3,d3			; |
	bne.s	NoRead2a		; |
	move.w	(a5)+,d3		; 4
	addx.w	d3,d3			; |
NoRead2a:				;
	addx.w	d0,d0			; 30
	add.w	d3,d3			; |
	addx.w	d0,d0			; |
	add.w	d0,d0			; |
	jmp	UJTable(pc,d0.w)	; |

UJTable:
	bra.s	CopyThree		; 3
	move.b	-(a4),(a0)+		; 7
	move.b	-(a4),(a0)+		; 10
	move.b	-(a4),(a0)+		; 15
	move.b	-(a4),(a0)+		; 15
	cmp.l	a2,a0			; |
	blt	Compressed		; |
	rts				;

//;---------------------------------------
FJTable:
	moveq	#0,d0
	move.b	(a1)+,(a0)+		; |
	move.b	(a1)+,(a0)+		; |
	move.b	(a1)+,(a0)+		; |

	move.b	(a1)+,(a0)+		; |
	move.b	(a1)+,(a0)+		; |
	move.b	(a1)+,(a0)+		; |
	move.b	(a1)+,(a0)+		; |

	move.b	(a1)+,(a0)+		; |
	move.b	(a1)+,(a0)+		; |
	move.b	(a1)+,(a0)+		; |
	move.b	(a1)+,(a0)+		; |

	move.b	(a1)+,(a0)+		; |
Tab15	move.b	(a1)+,(a0)+		; |
	move.b	(a1)+,(a0)+		; |
	move.b	(a1)+,(a0)+		; |
	tst.b	d0
	bne.s	TestCompressed

	dbra	d5,1$
	moveq	#7,d5
	move.l	(a5)+,d4
	add.l	d4,d4
1$
	moveq	#30,d0
	and.w	d4,d0
	roxr.l	#4,d4

	jmp	FJTable(pc,d0.w)
*/

}
