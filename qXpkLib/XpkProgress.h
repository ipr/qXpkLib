#ifndef XPKPROGRESS_H
#define XPKPROGRESS_H

// rewrite these later..



enum Xpk_Progress_Status
{
	XPKPROG_START = 1,	/* crunching started */
	XPKPROG_MID = 2,	/* somewhere in the mid */
	XPKPROG_END = 3	/* crunching is completed */
};

class CReadBuffer;

/* Passed to ChunkHook as the ParamPacket */
struct XpkProgress 
{
	// constructor
	XpkProgress()
	{
		xp_Type = XPKPROG_START;
		
		pInputBuffer = nullptr;
		pOutputBuffer = nullptr;
		
		xp_PackedProcessed = 0;
		xp_UnpackedProcessed = 0;
		xp_UnpackedSize = 0;
		
		xp_CompressionFactor = 0;
		xp_PercentageDone = 0;
		xp_Speed = 0;
	}
	
	/* Type of report: XPKPROG_#? numbers	 */
	//unsigned int  xp_Type;
	Xpk_Progress_Status xp_Type;
	
	/* Brief name of packer being used 		 */
	//char *xp_PackerName;
	
	/* Descriptive name of packer being used 	 */
	//char *xp_PackerLongName;
	
	/* Packing/unpacking message		 */
	//char *xp_Activity;
	
	/* Name of file being processed, if available */
	//char *xp_FileName;

	CReadBuffer *pInputBuffer;
	CReadBuffer *pOutputBuffer;
	
	
	/* Amount of packed data already processed	 */
	size_t	 xp_PackedProcessed;
	
	/* Amount of unpacked data already processed */
	size_t	 xp_UnpackedProcessed;
	
	/* Amount of unpacked data in file		 */
	size_t	 xp_UnpackedSize;
	
	/* Compression factor so far		 */
	int	 xp_CompressionFactor;
	
	/* Percentage done already			 */
	unsigned int	 xp_PercentageDone;
	
	/* Bytes per second, from beginning of stream */
	unsigned int	 xp_Speed;
	
	/* For future use				 */
	unsigned int	 xp_Reserved[8];
};

#endif // XPKPROGRESS_H
