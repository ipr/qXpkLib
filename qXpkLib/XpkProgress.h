///////////////////////////////////
//
// This XpkProgress is redesigned now
// as interface for sub-libraries
// to call methods in parent master-library.
//
// Ilkka Prusi
// ilkka.prusi@gmail.com
//

#ifndef XPKPROGRESS_H
#define XPKPROGRESS_H

#include <stdint.h>

// fwd. decl.
class CIOBuffer;
class CIoContext;
class QString;

class CLibMaster;

// TODO: continue changes for cleaner use in different cases

class XpkProgress 
{
private:
	CLibMaster *m_pMaster;
	
protected:
	// protected constructor:
	// only friends allowed to create (master)
	//
	XpkProgress(CLibMaster *pMaster)
		: m_pMaster(pMaster)
		, pInputIo(nullptr)
		, pOutputIo(nullptr)
		, pInputBuffer(nullptr)
		, pOutputBuffer(nullptr)
		, xp_chunkIn(0)
		, xp_chunkOut(0)
		, xp_WholePackedFileSize(0)
		, xp_PackedProcessed(0)
		, xp_UnpackedProcessed(0)
		, xp_UnpackedSize(0)
	{}

public:
	// TODO: replace buffers by context:
	// this way can wrap in/out to/from file/buffer..
	//
	CIoContext *pInputIo;
	CIoContext *pOutputIo;

	/* input and output buffers for sub-library (decruncher) */
	CIOBuffer *pInputBuffer;
	CIOBuffer *pOutputBuffer;
	
	// get output-context for given file with path,
	// parent-path will be added to extract to correct location.
	// use for multi-file archive output.
	//
	CIoContext *getOutput(QString &filePath)
	{
		return m_pMaster->getOutput(filePath);
	}

	// get output-context for when no name known
	// or single file/chunk output.
	//
	/*
	CIoContext *getOutput()
	{
		return m_pMaster->getOutput();
	}
	*/


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
	
	friend class CLibMaster;
};

#endif // XPKPROGRESS_H
