///////////////////////////////////
//
// This XpkProgress is mostly directly from XPKMaster sources
// that existed already, this will be dropped in the future..
//

#ifndef XPKPROGRESS_H
#define XPKPROGRESS_H

#include <stdint.h>

// fwd. decl.
class CReadBuffer;

struct XpkProgress 
{
	// constructor
	XpkProgress()
	{
		// TODO: reduce these, may be too many..
		
		pInputBuffer = nullptr;
		pOutputBuffer = nullptr;

		xp_chunkIn = 0;
		xp_chunkOut = 0;
		
		xp_WholePackedFileSize = 0;
		xp_PackedProcessed = 0;
		xp_UnpackedProcessed = 0;
		xp_UnpackedSize = 0;
	}

	/* input and output buffers for sub-library (decruncher) */
	CReadBuffer *pInputBuffer;
	CReadBuffer *pOutputBuffer;

	// TODO:? simplify by this?
	// (current chunk and related data..)
	//XpkChunk *pChunk;


	/* size of compressed chunk to decruncher */
	size_t	 xp_chunkIn;
	
	/* size of uncompressed chunk from decruncher */
	size_t	 xp_chunkOut;
	
	/* size of packed file in total, metadata and all */
	size_t	 xp_WholePackedFileSize;
	
	/* Amount of packed data already processed	 */
	size_t	 xp_PackedProcessed;
	
	/* Amount of unpacked data already processed */
	size_t	 xp_UnpackedProcessed;
	
	/* size of unpacked data in total */
	size_t	 xp_UnpackedSize;
	
};

#endif // XPKPROGRESS_H
