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

#include "BzDecompress.h"


///////////// protected methods

void OnError(const int32_t errorcode)
{
	// error happened -> no sense to continue
	// -> just throw it to caller and end
	// TODO: get error message by code?
	throw ArcException("Error in decompress", errorcode);
}

int32_t CBzDecompress::BZ2_decompress(DState *s)
{
	if (s->state == BZ_X_MAGIC_1) 
	{
		/*initialise the save area*/
		s->init();
	}
	
	/* stuff that needs to be saved/restored */
	// why not just keep it in the struct directly ??
	// (or as members..)
	
	int32_t i           = s->save_i;
	int32_t j           = s->save_j;
	int32_t t           = s->save_t;
	int32_t alphaSize   = s->save_alphaSize;
	int32_t nGroups     = s->save_nGroups;
	int32_t nSelectors  = s->save_nSelectors;
	int32_t EOB         = s->save_EOB;
	int32_t groupNo     = s->save_groupNo;
	int32_t groupPos    = s->save_groupPos;
	int32_t nextSym     = s->save_nextSym;
	int32_t nblockMAX   = s->save_nblockMAX;
	int32_t nblock      = s->save_nblock;
	int32_t es          = s->save_es;
	int32_t N           = s->save_N;
	int32_t curr        = s->save_curr;
	int32_t zt          = s->save_zt;
	int32_t zn          = s->save_zn; 
	int32_t zvec        = s->save_zvec;
	int32_t zj          = s->save_zj;
	int32_t gSel        = s->save_gSel;
	int32_t gMinlen     = s->save_gMinlen;
	int32_t *gLimit      = s->save_gLimit;
	int32_t *gBase       = s->save_gBase;
	int32_t *gPerm       = s->save_gPerm;
	
}

///////////// public methods

CBzDecompress::CBzDecompress()
{
}

//bool CBzDecompress::Decompress(CReadBuffer &pIn, const size_t nRead, CReadBuffer *pOut);

