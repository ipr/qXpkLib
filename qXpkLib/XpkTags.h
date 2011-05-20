#ifndef XPKTAGS_H
#define XPKTAGS_H

#include <stdint.h>
#include <limits.h>

#include "AnsiFile.h"

#ifndef UBYTE
typedef uint8_t UBYTE;
#endif

#ifndef UWORD
typedef uint16_t UWORD;
#endif

#ifndef ULONG
//typedef uint32_t ULONG; <- better but windef.h has with different type..
typedef unsigned long ULONG;
#endif


/* constants for Tag.ti_Tag, control tag values */
/* terminates array of TagItems. ti_Data unused */
enum TagType
{
	TAG_DONE    = 0,
	TAG_END	    = 0, /* synonym for TAG_DONE			  */
	TAG_IGNORE  = 1, /* ignore this item, not end of array		  */
	TAG_MORE    = 2, /* ti_Data is pointer to another array of TagItems. note that this tag terminates the current array */

	TAG_SKIP    = 3, /* skip this and the next ti_Data items	 */

	/* differentiates user tags from control tags */
	TAG_USER    = 1L<<31

};

struct TagItem;
struct TagItem
{
  /* identifies the type of data */
  //Tag ti_Tag;
  TagType ti_Tag;
  
  /* type-specific data	*/
  void *ti_Data;
  
  // next tag
  TagItem *next;
};


// TODO: are these really needed..

/*
 * The packing/unpacking tags
 *
 * (TRUE) or (FALSE) mean the default value given by xpkmaster.library
 */
#define XPK_TagBase	(TAG_USER + ('X'<<8) + 'P')
#define XTAG(a)		(XPK_TagBase+a)

/* Caller must supply ONE of these to tell Xpk#?ack where to get data from */
/* Process an entire named file */
#define XPK_InName	  XTAG(0x01)

/* Caller must supply 1 of these to tell Xpk#?ackFile where to send data to */
/* Master allocs OutBuf - ti_Data points to buf ptr */
#define XPK_GetOutBuf	  XTAG(0x13)
 /* Callback Hook to get output buffers */
#define XPK_OutHook	  XTAG(0x14)

/* Other tags for Pack/Unpack */
/* Length of data in input buffer */
#define XPK_InLen	  XTAG(0x20)
/* tags added for xfdmaster support (version 4 revision 25) */
/* 16 bit key (unpack only)	*/
#define XPK_Key16	  XTAG(0x37)
/* 32 bit key (unpack only)	*/
#define XPK_Key32	  XTAG(0x38)

/* tag added to support seek (version 5) */
#define XPK_NeedSeek	  XTAG(0x39) /* turn on Seek function usage	*/

/* preference depending tags added for version 4 - their default value
 * may depend on preferences, see <xpk/xpkprefs.h> for more info */
/* Use xfdmaster.library (FALSE) */
#define XPK_UseXfdMaster  XTAG(0x40)
/* Use packers in extern dir (TRUE)*/
#define XPK_UseExternals  XTAG(0x41)

/* tags XTAG(0x50) to XTAG(0x6F) are for XpkPassRequest -- see below */
#define XPK_MARGIN	256	/* Safety margin for output buffer	*/


// just for memory.. see how to replace..
/* These structures define the file format for compressed streams */
/*
struct XpkStreamHeader 
{
  ULONG xsh_Pack;
  ULONG xsh_CLen;
  ULONG xsh_Type;
  ULONG xsh_ULen;
  UBYTE xsh_Initial[16];
  UBYTE xsh_Flags;
  UBYTE xsh_HChk;
  UBYTE xsh_SubVrs;
  UBYTE xsh_MasVrs;
};

struct XpkChunkHdrWord 
{
  UBYTE xchw_Type;
  UBYTE xchw_HChk;
  UWORD xchw_CChk;
  UWORD xchw_CLen;
  UWORD xchw_ULen;
};

struct XpkChunkHdrLong 
{
  UBYTE xchl_Type;
  UBYTE xchl_HChk;
  UWORD xchl_CChk;
  ULONG xchl_CLen;
  ULONG xchl_ULen;
};

// TODO: does this align correctly in x86?
//
typedef union 
{
  struct XpkChunkHdrLong xch_Long;
  struct XpkChunkHdrWord xch_Word;
} XpkChunkHeader;


struct xpkFileHeader
{
	struct XpkStreamHeader h_Glob;
    XpkChunkHeader	 h_Loc;
    ULONG			 h_LocSize;
};
*/

/*
 * The file info block
 */
/*
struct XpkFib 
{
  // Unpacked, packed, archive? 
  unsigned int xf_Type;
  // Uncompressed length      
  unsigned int xf_ULen;
  // Compressed length        
  unsigned int xf_CLen;
  // Next chunk len           
  unsigned int xf_NLen;
  // Uncompressed bytes so far 
  unsigned int xf_UCur;
  // Compressed bytes so far  
  unsigned int xf_CCur;
  // 4 letter ID of packer    
  unsigned int xf_ID;
  // 4 letter name of packer  
  char  xf_Packer[6];
  // Required sublib version     
  unsigned short xf_SubVersion;
  // Required masterlib version 
  unsigned short xf_MasVersion;
  // Password?                
  unsigned int xf_Flags;
  // First 16 bytes of orig. file 
  char  xf_Head[16];
  // Compression ratio        
  int   xf_Ratio;
  // For future use           
  unsigned int xf_Reserved[8];
};
*/

// new.. hopefully more clear..
//
// file header only
//
struct XpkFileInfo
{
	// first 16 bytes similar to common IFF-style
	uint32_t m_ID; // IFF-style ID (off: 0)
	uint32_t m_FileLen; // IFF-style length (off: 4)
	uint32_t m_ChunkID; // IFF-style ID of cruncher (off: 8)
	uint32_t m_ChunkLen; // first chunk length (off: 12)
	
	// First 16 bytes of orig. file 
    char  m_Orig_Head[16];
};

class XpkChunk;
class XpkChunk
{
public:
	XpkChunk()
	    : m_ChunkID(0)
	    , m_ChunkLen(0)
	    , m_pNext(nullptr)
	    , m_pPrevious(nullptr)
	    , m_nDataOffset(0)
	{}
	~XpkChunk()
	{}
	
	uint32_t m_ChunkID; // IFF-style chunk-ID tag
	uint32_t m_ChunkLen; // chunk length 
	
	XpkChunk *m_pNext;
	XpkChunk *m_pPrevious;
	
	size_t m_nDataOffset; // offset to data (after chunk header)
	
};


class XpkTags
{
private:
	size_t m_nTotalSize;
	
	XpkFileInfo m_FileHeader;
	XpkChunk *m_pFirst;

	//XpkFib m_FileInfo;
	//XpkStreamHeader m_Header;
	
protected:
	inline uint16_t GetUWord(const unsigned char *pBuf)
	{
		// combine&byteswap
		//return ((pBuf[1] << 8) + pBuf[0]);
		// debug
		return ((pBuf[0] << 8) + pBuf[1]);
	}
	
	inline uint32_t GetULong(const unsigned char *pBuf)
	{
		// combine&byteswap
		//return ((pBuf[3] << 24) + (pBuf[2] << 16) + (pBuf[1] << 8) + pBuf[0]);
		// debug
		return ((pBuf[0] << 24) + (pBuf[1] << 16) + (pBuf[2] << 8) + pBuf[3]);
	}
	
	void ReadFileInfo(CReadBuffer &Buffer);
	void ReadStreamHeader(CReadBuffer &Buffer);
	
	TagItem *NextTagItem(CReadBuffer &Buffer, TagItem *pCurrent);
	void ParseTags(CReadBuffer &Buffer);
	
public:
    XpkTags();
	
	// verify that file is XPK:
	// expect certain structure
	// regardless of sub-type (packer)
	bool IsXpkFile(CReadBuffer &Buffer)
	{
		unsigned char *pBuf = Buffer.GetBegin();
		if (::memcmp(pBuf, "XPKF", 4) == 0)
		{
			return true;
		}
		return false;
	}
	
	void ParseToNodeList(CReadBuffer &Buffer);
	
};

#endif // XPKTAGS_H
