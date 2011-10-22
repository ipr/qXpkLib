/* xpkSQSH.c -- an LZ based cruncher; special algorithms for 8 bit sound data;
 * fast decrunching
 * Copyright (C) 1994 John Hendrikx
 * This file is part of the xpk package.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
 * USA.
 */

/* Written by John Hendrikx <?>
 * Converted by Bert Jahn <wepl@kagi.com>
 * XPK library version by Vesa Halttunen <vesuri@jormas.com>
 * C++ library version by Ilkka Prusi <ilkka.prusi@gmail.com>
 */


#include "xpkSQSH.h"

// from master-project for buffer-class
#include "AnsiFile.h"

// from master-project (fulfill fwd. decl.)
#include "XpkProgress.h"


// (see header)
xpkSQSH g_Instance;
xpkLibraryBase *GetXpkInstance(void)
{
  // TODO: switch to: new xpkRLEN() when there are members..
	return &g_Instance;
}

// see other versions below later..
//
int32_t bfextu(unsigned char *p, int32_t bo, int32_t bc) 
{
	int32_t r;
	
	p += bo / 8;
	r = *(p++);
	r <<= 8;
	r |= *(p++);
	r <<= 8;
	r |= *p;
	r <<= bo % 8;
	r &= 0xffffff;
	r >>= 24 - bc;
	
	return r;
}

//#define bfextu1 ((*(src + d0 / 8) >> (7 - (d0 % 8))) & 1)
//
// should return bool ? (that's how it's used anyway..)
inline int32_t bfextu1(const unsigned char *src, const int32_t d0)
{
	// just test one bit some place in *src..
	//
	return ((*(src + d0 / 8) >> (7 - (d0 % 8))) & 1);
}

// same as bfextu1() but for 3-bit field?
// see also bfexts()..
int32_t bfextu3(unsigned char *p, int32_t bo) 
{
	int32_t r;
	
	p += bo / 8;
	r = *(p++);
	r <<= 8;
	r |= *p;
	r >>= 13 - (bo % 8);
	r &= 7;
	
	return r;
}

// simulate M68k BFEXTS instruction?
// must be simpler way.. 
// just generate bitmask of suitable size and get it done..
int32_t bfexts(unsigned char *p, int32_t bo, int32_t bc) 
{
	int32_t r;
	
	p += bo / 8;
	r = *(p++);
	r <<= 8;
	r |= *(p++);
	r <<= 8;
	r |= *p;
	r <<= (bo % 8) + 8;
	r >>= 32 - bc;
	
	return r;
}

// this should get size of chunk somehow,
// or is it expecting marker-byte somewhere?
void unsqsh(unsigned char *src, unsigned char *dst) 
{
	// registers: force 32-bit
	int32_t d0,d1,d2,d3,d4,d5,d6,a2,a5;
	unsigned char *a4,*a6;
	
	// 
	unsigned char a3[] = { 2,3,4,5,6,7,8,0,3,2,4,5,6,7,8,0,4,3,5,2,6,7,8,0,5,4,
						6,2,3,7,8,0,6,5,7,2,3,4,8,0,7,6,8,2,3,4,5,0,8,7,6,2,3,4,5,0 };

	a6 = dst;
	a6 += *src++ << 8;
	a6 += *src++;
	d0 = d1 = d2 = d3 = a2 = 0;
 
	d3 = *(src++);
	*(dst++) = d3;
	goto l6c6;

l6c6:	
	if (d1 < 8) 
	{
		if (bfextu1(src, d0)) 
		{
			goto l75a;
		}
		else
		{
			d0 ++;
			d5 = 0;
			d6 = 8;
		}
		goto l734;
	}

	if (bfextu1(src, d0)) 
	{
		d0 += 1;
		d6 = 8;
		if (d6 == a2) 
		{
			if (d2 < 20) 
			{
				d5 = 0;
			}
			else
			{
				d5 = 1;
				d2 += 8;
			}
		}
		else
		{
			d6 = a2;
			d5 = 4;
			d2 += 8;
		}
		goto l734; 
	}
	d0 ++;
	if (! bfextu1(src, d0)) 
	{
		goto l75a;
	}
	d0 ++;
	if (bfextu1(src, d0)) 
	{
		d0 ++;
		if (! bfextu1(src, d0)) 
		{
			d6 = 3;
			d0 ++;
		}
		else
		{
			d6 = bfextu3(src,d0);
			d0 += 3;
		}
	}
	else
	{
		d6 = 2;
		d0 ++;
	}

	// only place using table in a3 ?
	d6 = *(a3 + (8*a2) + d6 - 17);
	if (d6 != 8) 
	{
		d5 = 4;
		d2 += 8;
	}
	else if (d2 < 20) 
	{
		d5 = 0;
	}
	else
	{
		d5 = 1;
		d2 += 8;
	}
	
	goto l734; // <- now below..

l734:
	do
	{
		d4 = bfexts(src,d0,d6);
		d0 += d6;
		d3 -= d4;
		*dst++ = d3;
		d5--;
	} while (d5 != -1);
	
	if (d1 != 31) 
	{
		d1 += 1;
	}
	a2 = d6;
	goto l74c;

l74c:	
	d6 = d2;
	d6 >>= 3;
	d2 -= d6;
	
	if (dst < a6) 
	{
		goto l6c6;
	}
	// this is end of chunk?
	return;

l75a:	

	d0++;
	if (bfextu1(src, d0)) 
	{
		d0++;
		if (bfextu1(src, d0)) 
		{
			d0++;
			if (bfextu1(src, d0)) 
			{
				d0++;
				if (bfextu1(src, d0)) 
				{
					d0++;
					
					d6 = bfextu(src,d0,5);
					d0 += 5;
					d4 = 16;
					d6 += d4; 
				}
				else
				{
					d0++;
					
					d6 = bfextu3(src,d0);
					d0 += 3;
					d6 += 8;
				}
				goto l7a8; // this skips anyway
			}
			else
			{
				d4 = 6;
			}
		}
		else
		{
			d4 = 4;
		}
	}
	else
	{
		d4 = 2;
	}
	
	d0++;
	d6 = bfextu1(src, d0);
	d0 ++;
	
	goto l7a8; // 

l7a8:	
	if (bfextu1(src, d0)) 
	{
		d5 = 12;
		a5 = -0x100;
	}
	else
	{
		d0 ++;
		if (bfextu1(src, d0)) 
		{
			d5 = 14;
			a5 = -0x1100;
		}
		else
		{
			d5 = 8;
			a5 = 0;
		}
	}

	d0++;
	d4 = bfextu(src,d0,d5);
	d0 += d5;
	d6 -= 3;
	
	if (d6 < 0) 
	{
		d6 += 2;
		a4 = -1 + dst + a5 - d4;
	}
	else
	{
		if (d6 != 0) 
		{
			d1 -= 1;
		}
		
		d1 -= 1;
		if (d1 < 0) 
		{
			d1 = 0;
		}
		d6 += 2;
		a4 = -1 + dst + a5 - d4;
	}
	
	do
	{
		*dst++ = *a4++;
		d6--;
	} while (d6 != -1);
	
	d3 = *(--a4);
	goto l74c;
}


xpkSQSH::xpkSQSH()
    : xpkLibraryBase()
{
	// only to/from buffer supported here
	m_XpkCaps.input.buffer = true;
	m_XpkCaps.output.buffer = true;
	m_XpkCaps.m_LibIdentifier = "SQSH";
	m_XpkCaps.m_LibDescription = "XPK (SQSH) implementation";
}

xpkSQSH::~xpkSQSH()
{
}

bool xpkSQSH::Crunch(XpkProgress *pProgress)
{
	// not supported (yet)
	return false;
}

bool xpkSQSH::Decrunch(XpkProgress *pProgress)
{
	// TODO: we should pass size of chunk to unpacking,
	// then update amount of data actually processed..
	
	//size_t nOut = pProgress->pOutputBuffer->GetCurrentPos();
	
	// TODO: use sizes for input & output..
	//pProgress->xp_chunkIn;
	//pProgress->xp_chunkOut;

	unsqsh(pProgress->pInputBuffer->GetAtCurrent(), pProgress->pOutputBuffer->GetAtCurrent());
	
	return true;
}

