/*
 * This C++ library version by: Ilkka Prusi <ilkka.prusi@gmail.com>
 * reuses multiple parts of code from qLhaLib (now xadLHA) based on Unix LHa
 * since ARJ uses same static Huffman as LHa (methods 4..7)
 * instead of rewriting old unarj-decoding.
 *
 * Based on Unix LhA source code: lha-1.14i-ac20040929
 * Copyrights:
 * LHarc    for UNIX  V 1.02  Copyright(C) 1989  Y.Tagawa
 * LHx      for MSDOS V C2.01 Copyright(C) 1990  H.Yoshizaki
 * LHx(arc) for OSK   V 2.01  Modified     1990  Momozou
 * LHa      for UNIX  V 1.00  Copyright(C) 1992  Masaru Oki
 * LHa      for UNIX  V 1.14  Modified     1995  Nobutaka Watazaki
 * LHa      for UNIX  V 1.14i Modified     2000  Tsugio Okamoto
 *                    Autoconfiscated 2001-2004  Koji Arai
 * 
 * C++ library version (qLhaLib and xadLHA), modifications by Ilkka Prusi (2011).
 *
 */

#include "Decode.h"

/* ------------------------------------------------------------------------ */
/* LHa for UNIX                                                             */
/*              maketbl.c -- makes decoding table                           */
/*                                                                          */
/*      Modified                Nobutaka Watazaki                           */
/*                                                                          */
/*  Ver. 1.14   Source All chagned              1995.01.14  N.Watazaki      */
/* ------------------------------------------------------------------------ */

void CHuffmanTree::make_table(
    const int16_t	nchar,
    uint8_t			bitlen[],
    int16_t			tablebits,
    uint16_t		table[])
{
	// note: loops below are 1..<=16 -> need 17 cells
    uint16_t  count[17];  /* count of bitlen */
    uint16_t  weight[17]; /* 0x10000ul >> bitlen */
    uint16_t  start[17];  /* first code of bitlen */
    uint32_t  iCount = 0;

    /* initialize */
	::memset(count, 0, sizeof(uint16_t)*17);
    for (iCount = 1; iCount <= 16; iCount++) 
	{
        weight[iCount] = 1 << (16 - iCount);
    }

    /* count */
    for (iCount = 0; iCount < nchar; iCount++)
	{
        count[bitlen[iCount]]++;
	}

    /* calculate first code */
    uint16_t total = 0;
    for (iCount = 1; iCount <= 16; iCount++) 
	{
        start[iCount] = total;
        total += weight[iCount] * count[iCount];
    }
    if ((total & 0xffff) != 0)
	{
        //error("make_table(): Bad table (5)");
		throw ArcException("make_table(): Bad table (5)", total);
	}

    /* shift data for make table. */
    int m = 16 - tablebits;
    for (iCount = 1; iCount <= tablebits; iCount++) 
	{
        start[iCount] >>= m;
        weight[iCount] >>= m;
    }

    /* initialize */
    int start_m = start[tablebits + 1] >> m;
    if (start_m != 0)
	{
		int kEnd = 1 << tablebits;
        for (int i = start_m; i < kEnd; i++)
		{
            table[i] = 0;
		}
	}

    int avail = nchar;

    /* create table and tree */
    for (int j = 0; j < nchar; j++) 
	{
		// unconditional set -> make scoped
        int k = bitlen[j];
        if (k == 0)
		{
            continue;
		}
		
        unsigned int l = start[k] + weight[k];
        if (k <= tablebits) 
		{
            /* code in table */
            for (unsigned int i = start[k]; i < l; i++)
			{
                table[i] = j;
			}
        }
        else 
		{
            /* code not in table */
			unsigned int i = start[k];
			
            uint16_t *pTbl = &table[i >> m];
            i <<= tablebits;
			
            int n = k - tablebits;
			
            /* make tree (n length) */
			make_table_tree(n, j, i, pTbl, avail);
        }
        start[k] = l;
    }
}

// split from above: this is called in loop from make_table(),
// only part in make_table() that uses left[] and right[] arrays
//
void CHuffmanTree::make_table_tree(const int treeLen, const int j, unsigned int &i, uint16_t *pTbl, int &avail)
{
	/* make tree (n length) */
	int nLen = treeLen;
    while (--nLen >= 0) 
	{
        if ((*pTbl) == 0) 
		{
            right[avail] = left[avail] = 0;
            (*pTbl) = avail++;
        }
        if (i & 0x8000)
		{
            pTbl = &right[(*pTbl)];
		}
        else
		{
            pTbl = &left[(*pTbl)];
		}
        i <<= 1;
    }
    (*pTbl) = j;
}
