//////////////////////////
//
// BzDecompress
//
// Modified from BZip2 source by Ilkka Prusi <ilkka.prusi@gmail.com>
// for use in C++ library
//
// Original notes below:
/* ------------------------------------------------------------------
   This file is part of bzip2/libbzip2, a program and library for
   lossless, block-sorting data compression.

   bzip2/libbzip2 version 1.0.6 of 6 September 2010
   Copyright (C) 1996-2010 Julian Seward <jseward@bzip.org>

   Please read the WARNING, DISCLAIMER and PATENTS sections in the 
   README file.

   This program is released under the terms of the license contained
   in the file LICENSE.
   ------------------------------------------------------------------ */

#ifndef BZDECOMPRESS_H
#define BZDECOMPRESS_H

// prefer ISO-standard typedefs
// for future compatibility
#include <stdint.h>

/*-- states for decompression. --*/

enum BzDecompressStates // in C++0x : uint32_t
{
	BZ_X_IDLE        = 1,
	BZ_X_OUTPUT      = 2,

	BZ_X_MAGIC_1     = 10,
	BZ_X_MAGIC_2     = 11,
	BZ_X_MAGIC_3     = 12,
	BZ_X_MAGIC_4     = 13,
	BZ_X_BLKHDR_1    = 14,
	BZ_X_BLKHDR_2    = 15,
	BZ_X_BLKHDR_3    = 16,
	BZ_X_BLKHDR_4    = 17,
	BZ_X_BLKHDR_5    = 18,
	BZ_X_BLKHDR_6    = 19,
	BZ_X_BCRC_1      = 20,
	BZ_X_BCRC_2      = 21,
	BZ_X_BCRC_3      = 22,
	BZ_X_BCRC_4      = 23,
	BZ_X_RANDBIT     = 24,
	BZ_X_ORIGPTR_1   = 25,
	BZ_X_ORIGPTR_2   = 26,
	BZ_X_ORIGPTR_3   = 27,
	BZ_X_MAPPING_1   = 28,
	BZ_X_MAPPING_2   = 29,
	BZ_X_SELECTOR_1  = 30,
	BZ_X_SELECTOR_2  = 31,
	BZ_X_SELECTOR_3  = 32,
	BZ_X_CODING_1    = 33,
	BZ_X_CODING_2    = 34,
	BZ_X_CODING_3    = 35,
	BZ_X_MTF_1       = 36,
	BZ_X_MTF_2       = 37,
	BZ_X_MTF_3       = 38,
	BZ_X_MTF_4       = 39,
	BZ_X_MTF_5       = 40,
	BZ_X_MTF_6       = 41,
	BZ_X_ENDHDR_2    = 42,
	BZ_X_ENDHDR_3    = 43,
	BZ_X_ENDHDR_4    = 44,
	BZ_X_ENDHDR_5    = 45,
	BZ_X_ENDHDR_6    = 46,
	BZ_X_CCRC_1      = 47,
	BZ_X_CCRC_2      = 48,
	BZ_X_CCRC_3      = 49,
	BZ_X_CCRC_4      = 50
};


class CBzDecompress
{
protected:

	/*-- Constants for the fast MTF decoder. --*/
	
	const int32_t MTFA_SIZE = 4096;
	const int32_t MTFL_SIZE = 16;
	
	/*-- Structure holding all the decompression-side stuff. --*/
	
	// is this all really needed here??
	// seems much extra stuff..
	//
	struct {
      /* pointer back to the struct bz_stream */
      bz_stream* strm;

      /* state indicator for this stream */
      int32_t    state;

      /* for doing the final run-length decoding */
      uint8_t    state_out_ch;
      int32_t    state_out_len;
      Bool     blockRandomised;
      
      /* BZ_RAND_DECLS */
      int32_t rNToGo;
      int32_t rTPos;

      /* the buffer for bit stream reading */
      uint32_t   bsBuff;
      int32_t    bsLive;

      /* misc administratium */
      int32_t    blockSize100k;
      Bool     smallDecompress;
      int32_t    currBlockNo;
      //int32_t    verbosity;

      /* for undoing the Burrows-Wheeler transform */
      int32_t    origPtr;
      uint32_t   tPos;
      int32_t    k0;
      int32_t    unzftab[256];
      int32_t    nblock_used;
      int32_t    cftab[257];
      int32_t    cftabCopy[257];

      /* for undoing the Burrows-Wheeler transform (FAST) */
      uint32_t   *tt;

      /* for undoing the Burrows-Wheeler transform (SMALL) */
      uint16_t   *ll16;
      uint8_t    *ll4;

      /* stored and calculated CRCs */
      uint32_t   storedBlockCRC;
      uint32_t   storedCombinedCRC;
      uint32_t   calculatedBlockCRC;
      uint32_t   calculatedCombinedCRC;

      /* map of bytes used in block */
      int32_t    nInUse;
      Bool     inUse[256];
      Bool     inUse16[16];
      uint8_t    seqToUnseq[256];

      /* for decoding the MTF values */
      uint8_t    mtfa   [MTFA_SIZE];
      int32_t    mtfbase[256 / MTFL_SIZE];
      uint8_t    selector   [BZ_MAX_SELECTORS];
      uint8_t    selectorMtf[BZ_MAX_SELECTORS];
      uint8_t    len  [BZ_N_GROUPS][BZ_MAX_ALPHA_SIZE];

      int32_t    limit  [BZ_N_GROUPS][BZ_MAX_ALPHA_SIZE];
      int32_t    base   [BZ_N_GROUPS][BZ_MAX_ALPHA_SIZE];
      int32_t    perm   [BZ_N_GROUPS][BZ_MAX_ALPHA_SIZE];
      int32_t    minLens[BZ_N_GROUPS];

      /* save area for scalars in the main decompress code */
      int32_t    save_i;
      int32_t    save_j;
      int32_t    save_t;
      int32_t    save_alphaSize;
      int32_t    save_nGroups;
      int32_t    save_nSelectors;
      int32_t    save_EOB;
      int32_t    save_groupNo;
      int32_t    save_groupPos;
      int32_t    save_nextSym;
      int32_t    save_nblockMAX;
      int32_t    save_nblock;
      int32_t    save_es;
      int32_t    save_N;
      int32_t    save_curr;
      int32_t    save_zt;
      int32_t    save_zn; 
      int32_t    save_zvec;
      int32_t    save_zj;
      int32_t    save_gSel;
      int32_t    save_gMinlen;
      int32_t*   save_gLimit;
      int32_t*   save_gBase;
      int32_t*   save_gPerm;
   }
   DState;

	/*-- Macros for decompression. --*/
	
	inline void SET_LL4(const int32_t i, const int32_t n, DState *s) const
	{
		if ((i & 0x1) == 0)
		{
			s->ll4[(i >> 1)] = (s->ll4[(i >> 1)] & 0xf0) | n; 
		}
		else
		{
			s->ll4[(i >> 1)] = (s->ll4[(i >> 1)] & 0x0f) | (n << 4);
		}
	}

	inline void SET_LL(const int32_t i, const int32_t n, DState *s) const
	{ 
		s->ll16[i] = (uint16_t)(n & 0x0000ffff);
		SET_LL4(i, n >> 16, s);
	}
	
	inline uint32_t GET_LL4(const int32_t i, const DState *s) const
	{
		uint32_t tmp = (s->ll4[(i >> 1)]);
		tmp >>= ((i << 2) & 0x4);
		return (tmp & 0xF);
	}

	inline uint32_t GET_LL(const int32_t i, const DState *s)
	{
		uint32_t tmp = (s->ll16[i]);
		return (tmp | (GET_LL4(i, s) << 16));
	}

	bool BZ_GET_SMALL(int32_t &cccc, DState *s) const
	{
		/* c_tPos is unsigned, hence test < 0 is pointless. */ 
		if (s->tPos >= (uint32_t)100000 * (uint32_t)s->blockSize100k) 
		{
			// this is supposed to return from where we were called from?
			// 
			return true; // or just 1 ? (supposed to be int?)
		}
		// is this really suppose to modify the value
		// or just use it as implicit temp? 
		// -> assume modify..
		cccc = BZ2_indexIntoF( s->tPos, s->cftab );    
		s->tPos = GET_LL(s->tPos);
		return false;
	}
	
	bool BZ_GET_FAST(int32_t &cccc, DState *s) const
	{
		// reduce duplication of code
		return BZ_GET_FAST_C(cccc, s->tPos, s->tt, s->blockSize100k);
	}
	
	bool BZ_GET_FAST_C(int32_t &cccc, uint32_t &c_tPos, uint32_t *c_tt, const uint32_t ro_blockSize100k /*DState *s*/) const
	{
		/* c_tPos is unsigned, hence test < 0 is pointless. */
		// should be: s->tPos ?
		if (c_tPos >= (uint32_t)100000 * ro_blockSize100k) 
		{
			// supposed return from caller?
			return true;
		}
		// should use: s->tt ?
		c_tPos = c_tt[c_tPos];
		cccc = (uint8_t)(c_tPos & 0xff);
		c_tPos >>= 8;
		return false;
	}


	// seems this really needs to be inlined..
	// -> don't make caller inline then ?
	inline int32_t BZ2_indexIntoF(const int32_t indx, const int32_t *cftab) const
	{
		int32_t nb = 0;
		int32_t na = 256;
		do 
		{
			int32_t mid = (nb + na) >> 1;
			if (indx >= cftab[mid]) 
			{
				nb = mid; 
			}
			else 
			{
				na = mid;
			}
		} while (na - nb != 1);
		return nb;
	}

/*
	// call to huffman-tables -> another object..
	void BZ2_hbCreateDecodeTables ( Int32 *limit,
									Int32 *base,
									Int32 *perm,
									UChar *length,
									Int32 minLen,
									Int32 maxLen,
									Int32 alphaSize );


	int32_t BZ2_decompress(DState *s);
*/

public:
    CBzDecompress();
    
    //bool Decompress(CAnsiFile &archive, CReadBuffer *pOut);
};

#endif // BZDECOMPRESS_H
