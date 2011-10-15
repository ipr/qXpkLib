///////////////////////////
//
// xpk "NUKE" compression decruncher
//
// Based on M68k-assembler code
// by: Dirk Stöcker (SDI) <stoecker@amigaworld.com>
//
// Author: Ilkka Prusi
// ilkka.prusi@gmail.com
//
 

#include "Nuke.h"

/*
 * Pack a chunk
 */
struct NukeData 
{
	char	*inbuf;
	char	*outbuf;
	char	*last;
	char	*next;
	uint32_t cwri;
	uint32_t	uwri;
	uint32_t	uend;
	uint32_t	delpos;
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
	uint32_t	dest1;
	uint32_t	dest2;
	uint32_t	dest4;
	uint32_t	destN;
	uint32_t	dummy;
	uint32_t	contbuf;
	uint32_t	flags;
};


CNuke::CNuke()
{
}

/*
; ==================================================================
;                      xpkNUKE assembler routines
; ==================================================================
	INCLUDE	"exec/types.i"
	XDEF	_AsmPack
	XDEF	_AsmUnpack
	XDEF	NoReadN

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

starts:	dc.w	0,0,4,10

dists:	dc.w	$0010,$0040,$0100,$0200
	dc.w	$0010,$0080,$0200,$0800,$2000,$4000
	dc.w	$0020,$0080,$0200,$0800,$2000,$4000

	dc.w	4,6,8,9
	dc.w	4,7,9,11,13,14
	dc.w	5,7,9,11,13,14

	dc.w	0,1,2,3
	dc.w	4,5,6,7,8,9
	dc.w	10,11,12,13,14,15

modes	dc.w	4,6,8,9
	dc.w	4,7,9,11,13,14
	dc.w	5,7,9,11,13,14

adds	dc.w	$0000,$0010,$0050,$0150
	dc.w	$0000,$0010,$0090,$0290,$0a90,$2a90
	dc.w	$0000,$0020,$00a0,$02a0,$0aa0,$2aa0

	dc.w	c2-TwoBitLen,c2-TwoBitLen,c2-TwoBitLen,c2-TwoBitLen
	dc.w	c3-TwoBitLen,c3-TwoBitLen,c3-TwoBitLen,c3-TwoBitLen
	dc.w	c3-TwoBitLen,c3-TwoBitLen
	dc.w	0,0,0,0,0,0

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



_AsmPack:
	movem.l	d2-d7/a2-a6,-(sp)
;	illegal		; Debug
	bsr	init

	moveq	#0,d0
	moveq	#0,two
	moveq	#0,d7
;================= Main loop
mainloop:
;	movem.l	a0-a6/d0-d7,-(sp)	; DEBUG
;	bsr	_stat3
;	movem.l	(sp)+,a0-a6/d0-d7

	moveq	#0,two
	move.b	-1(buf,i.w),two
	asl.w	#8,two
	move.b	0(buf,i.w),two
	add.l	two,two

;================= Test compressable
	move.w	0(lst,two.l),look
	bne.s	byterunchk

;================= Not compressable
uncompressable
	UNCOMP
	move.w	i,0(lst,two.l)
	addq.w	#1,i

	cmp.w	ustop(a4),i
	blt.s	mainloop
	bra	mainexit

;================= Byterun
byterunchk
	move.b	-1(buf,i.w),d0
	cmp.b	0(buf,i.w),d0
	bne.s	getmatch

	lea	1(buf,i.w),a0
	move.l	a0,a1

	move.w	ustop(a4),d4
	sub.w	i,d4
1$	cmp.b	(a0)+,d0
	dbne	d4,1$


	move.l	a0,len
	cmp.l	uend(a4),len
	ble.s	3$
	move.l	uend(a4),len
3$	sub.l	a1,len
	cmp.w	#2,len
	ble.s	getmatch ; ble

	moveq	#1,off
	subq.w	#1,len

	UNCOMP
	addq.w	#1,i
	bra	gooddeal

;================= Getmatch
getmatch

	cmp.w	scanrange(a4),i	; do we have to start deleting old entries?
	bls.s	nodel

	moveq	#0,d7
	move.w	i,d7
	sub.w	scanrange(a4),d7
	add.w	d7,d7
	lea	0(nxt,d7.l),a1

	move.l	delpos(a4),a0
	moveq	#0,d7
1$	move.l	d7,(a0)+	; del two entries at once.
	cmp.l	a0,a1
	bcc.s	1$
	move.l	a0,delpos(a4)

nodel
	move.w	i,d4		; lowlim = i-scanrange
	sub.w	scanrange(a4),d4
	bcc.s	1$
	moveq	#0,d4
1$
	lea	1(buf,i.w),a0	; maxlenpos= buf+i+1
	move.l	a0,d2
	addq.l	#1,d2
	move.l	a0,d0		; cmpstart = buf+i+1
	move.b	(a0),d7		; testbyte = buf[i+1]

	move.l	buf,d3		; foundpos = scanbuf+look
	add.l	look,d3
	addq.l	#2,d3

	move.l	buf,a2		; testbase= buf
	move.w	i,(nxt)		; next[0]=i

	bra.s	ientry

;================= Inner loop
couldbemax:
	lea	1(buf,look.w),a0	; cmp1= scanbase+look
	move.l	d0,a1		; cmp2= scanstart
1$
	cmpm.b	(a0)+,(a1)+	; while(*cmp1++==*cmp2++)
	beq.s	1$		;   ;

	cmp.l	a1,d2		; if( len>=maxlen )
	bcc.s	NoNewMax

;	tst.w	look
;	beq.s	FoundBestMatch

	cmp.w	d4,look		;	if( look<=lowlim )
	bls.s	FoundBestMatch	;		goto QuitII

	sub.l	d2,a2		;	testbase+= maxlen-len
	add.l	a1,a2
	move.l	a1,d2		;	maxlenpos= len
	move.l	a0,d3		;	foundpos = cmp1
	move.b	-(a1),d7	;	testbyte = *(cmp2-1)
NoNewMax:

;================= Inner sanctum
InnerInner:
	add.w	look,look
	move.w	0(nxt,look.l),look	; look=Next[look]
ientry	cmp.b	1(a2,look.w),d7
	beq.s	1$

	add.w	look,look
	move.w	0(nxt,look.l),look	; look=Next[look]
	cmp.b	1(a2,look.w),d7
	bne.s	InnerInner

1$	cmp.w	i,look
	bne.s	couldbemax

;================= Compute offs

FoundBestMatch:
	sub.l	d2,off		; offset = foundpos-maxlenpos
	neg.l	off

;	cmp.w	scanrange(a4),d0
;	ble.s	ok
;bug	nop

gotoffs	cmp.l	uend(a4),len
	ble.s	nooverr
overr	move.l	uend(a4),len
nooverr	sub.l	d0,len		; maxlen= maxlenpos-cmpstart
	moveq	#0,d0


;================= Check if good deal
	cmp.w	#1,len
	bgt.s	gooddeal
	cmp.w	#$150,off	; FIXME!  (fixed CvR V1.03)
	bcs.s	gooddeal

	moveq	#0,d7
	move.w	i,d7
	add.l	d7,d7
	move.w	0(lst,two.l),0(nxt,d7.l)
	bra	uncompressable

;================= Compressable
gooddeal
	move.w	ucount(a4),d7	; write uncompressed
	blt.s	1$
	jsr	WRUNCO
	bra.s	hadunco
1$	WRITE1

hadunco
;	movem.l	a0-a6/d0-d7,-(sp)	; DEBUG
;	bsr	_stat
;	movem.l	(sp)+,a0-a6/d0-d7


	move.l	len,d0
	cmp.w	#3,d0
	ble.s	getmode
	moveq	#3,d0
;================= Getmode

getmode	lea	starts(pc),a2
	add.w	d0,d0
	move.w	0(a2,d0.w),d0

	lea	dists(pc),a2
	add.w	d0,d0
	add.w	d0,a2

2$	cmp.w	(a2),off
	blt.s	wrmode
3$	sub.w	(a2)+,off
	cmp.w	(a2),off
	bge.s	3$

wrmode	move.w	64(a2),d7
	WRITE4

wroffs	move.w	32(a2),d7
	WRITEN

	cmp.w	#2,len
	ble	genentries

;================= Write len
wrlen	move.l	len,d0
	cmp.w	#5,d0
	bgt.s	longlen

	subq.w	#3,d0
	not.w	d0
	moveq	#3,d7
	and.w	d0,d7
	WRITE2
	bra	genentries

longlen	moveq	#0,d7
	WRITE2
	subq.w	#6,d0
1$	cmp.w	#14,d0
	ble.s	last15
	moveq	#0,d7
	move.l	d0,-(sp)
	WRITE4
	move.l	(sp)+,d0
	sub.w	#15,d0
	bra.s	1$

last15	not.w	d0
	moveq	#15,d7
	and.w	d0,d7
	WRITE4

;================= Gen entries
genentries
	lea	1(buf,i.w),a0
	move.w	i,d7
	add.w	d7,d7
	lsr.l	#1,two

;	cmp.w	#1,off	;
;	bne.s	entry	;

;	add.w	len,i	;
;	addq.w	#1,i	;
;	bra.s	mainchk	;

	bra.s	entry

eloop	lsl.w	#8,two
	move.b	(a0)+,two
entry	lea	0(lst,two.l),a1
	add.l	two,a1
	move.w	(a1),0(nxt,d7.l)
	move.w	i,(a1)
	addq.w	#2,d7
	addq.w	#1,i
	dbra	len,eloop

;================= End
mainchk
;;;	add.l	two,two	; not needed, since mainloop does recalculate it
	cmp.w	ustop(a4),i
	blt	mainloop
mainexit
	bsr	flush
	bsr	compact

	movem.l	(sp)+,d2-d7/a2-a6
	rts

;======================================================================
init	moveq	#0,d0
	move.w	d0,room1(a4)
	move.w	d0,room2(a4)
	move.w	d0,room4(a4)
	move.w	d0,roomN(a4)

	move.w	d0,data1(a4)
	move.w	d0,data2(a4)
	move.l	d0,data4(a4)
	move.l	d0,dataN(a4)

	lea	dummy(a4),a0
	move.l	a0,dest1(a4)
	move.l	a0,dest2(a4)
	move.l	a0,dest4(a4)
	move.l	a0,destN(a4)

	move.l	last(a4),lst
	move.l	inbuf(a4),buf
	move.l	next(a4),nxt

	move.l	nxt,delpos(a4)
	move.w	#$6a90,scanrange(a4)	; $6aa0
	subq.w	#1,scanrange(a4)

	move.l	outbuf(a4),cwri(a4)
	move.l	outbuf(a4),a0
	add.w	clen(a4),a0
	move.l	a0,uwri(a4)

	move.l	inbuf(a4),a0
	add.w	ulen(a4),a0
	move.l	a0,uend(a4)

	move.w	ulen(a4),ustop(a4)
	subq.w	#1,ustop(a4)

	moveq	#1,i
	moveq	#0,two
	moveq	#0,look

	move.w	#-1,ucount(a4)
	move.w	#0,ustart(a4)
	rts

;======================================================================
flush
	cmp.w	ulen(a4),i
	beq.s	just1
	UNCOMP
just1	UNCOMP
	move.w	ucount(a4),d7
	bsr	WRUNCO

	move.l	dest1(a4),a0
	move.w	data1(a4),d0
	move.w	room1(a4),d1
	lsl.w	d1,d0
	move.w	d0,(a0)

	move.l	dest2(a4),a0
	move.w	data2(a4),d0
	move.w	room2(a4),d1
	lsl.w	d1,d0
	move.w	d0,(a0)

	move.l	dest4(a4),a0
	move.l	data4(a4),d0
	move.w	room4(a4),d1
	lsr.l	d1,d0
	move.l	d0,(a0)

	move.l	destN(a4),a0
	move.w	dataN+2(a4),d0
	move.w	roomN(a4),d1
	lsl.w	d1,d0
	move.w	d0,(a0)

	rts


;======================================================================
compact	move.l	uwri(a4),d0
	sub.l	cwri(a4),d0
	bge.s	3$

	moveq	#-1,d0
	bra.s	exitc

3$	move.l	uwri(a4),a2
	move.l	outbuf(a4),a0
	add.w	clen(a4),a0
	sub.l	uwri(a4),a0
	move.l	a0,d2
	move.l	cwri(a4),a1

1$	move.l	a1,d0
	add.l	d2,d0
	moveq	#3,d1
	and.l	d0,d1
	beq.s	copy
	move.b	#$FB,(a1)+
	bra.s	1$

copylp	move.b	(a2)+,(a1)+
copy	dbra	d2,copylp

	move.l	a1,d0
	sub.l	outbuf(a4),d0
exitc	rts

; d0=cmpstart     a0=cmp1
; d1=twochars+    a1=cmp2
; d2=maxlenpos    a2=testbase
; d3=foundpos     a3=lst+
; d4=lowerlimit   a4=
; d5=i+           a5=buf+
; d6=look+        a6=nxt+
; d7=testbyte     a7=

;***************************************************************************


_AsmUnpack
	movem.l	d2-d7/a3-a6,-(a7)
	MOVEA.L	A1,A5			; only because of Maxon A5 problem
	moveq	#16,d0
	move.l	d0,a3
	lea	modes,a6
	moveq	#0,d0
	moveq	#0,d1
	move.w	#$8000,d2
	move.w	#$8000,d3
	moveq	#0,d4
	moveq	#0,d5
	moveq	#0,d6
	moveq	#0,d7
	bsr	TestCompressed
	move.l	a0,d0
	movem.l	(a7)+,d2-d7/a3-a6
	rts

; d0 mode		a0 writepos
; d1 offslen/offset	a1 copysrc
; d2 1bit_data		a2 writeend
; d3 2bits_data		a3 16

; d4 4bits_data		a4 byteread
; d5 4bits_in		a5 wordread
; d6 xbits_data		a6 table
; d7 -xbits_in		a7

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
	sub.w	a3,d7			; |
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

;----------------------------------------
TestCompressed:				;
	add.w	d2,d2			; 70	Read 1 (cbit)
	bcc.s	Uncompressed		; |
	bne.s	Compressed		; |
	move.w	(a5)+,d2		; 4
	addx.w	d2,d2			; |
	bcs.s	Compressed		; 70
;----------------------------------------

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

;---------------------------------------
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

