#ifndef XPKTAGS_H
#define XPKTAGS_H

#include <stdint.h>
#include <limits.h>

// for buffer-handler..
#include "AnsiFile.h"

/*
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
*/

/* constants for Tag.ti_Tag, control tag values */
/* terminates array of TagItems. ti_Data unused */
/*
enum TagType
{
	TAG_DONE    = 0,
	TAG_END	    = 0, // synonym for TAG_DONE			  
	TAG_IGNORE  = 1, // ignore this item, not end of array		  
	TAG_MORE    = 2, // ti_Data is pointer to another array of TagItems. note that this tag terminates the current array 

	TAG_SKIP    = 3, // skip this and the next ti_Data items	 

	// differentiates user tags from control tags 
	TAG_USER    = 1L<<31
};
*/

/* constants for Tag.ti_Tag, control tag values */
/* terminates array of TagItems. ti_Data unused */
#define TAG_DONE   (0L)
/* synonym for TAG_DONE			  */
#define TAG_END	   (0L)
/* ignore this item, not end of array		  */
#define	TAG_IGNORE (1L)
/* ti_Data is pointer to another array of TagItems
 * note that this tag terminates the current array */
#define	TAG_MORE   (2L)
/* skip this and the next ti_Data items	 */
#define	TAG_SKIP   (3L)

/* differentiates user tags from control tags */
#define TAG_USER   ((unsigned int)(1L<<31))

// note: can't cast directly to this
// since ptr is 8 bytes on 64-bit build..
// -> offset would be wrong
struct TagItem
{
  /* identifies the type of data */
  uint32_t ti_Tag;
  
  /* type-specific data	*/
  void *ti_Data;
};

class XpkTag;
class XpkTag
{
public:
	TagItem m_Item;
	//void *m_pData;
	
	XpkTag *m_pNext;
	
public:
	XpkTag(void)
	    : m_pNext(nullptr)
		//, m_pData(nullptr)
	{}
	~XpkTag(void)
	{
		/*
		if (m_pData != nullptr)
		{
			delete m_pData;
		}
		*/
	}
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
/* File handle - start from curposition
 * If packing partial file, must also supply InLen */
#define XPK_InFH	  XTAG(0x02)
/* Single unblocked memory buffer, must also supply InLen */
#define XPK_InBuf	  XTAG(0x03)
/* Call custom Hook to read data
 * Must also supply InLen, when hook cannot do! (not for XPK unpacking) */
#define XPK_InHook	  XTAG(0x04)

/* Caller must supply 1 of these to tell Xpk#?ackFile where to send data to */
/* Write (or overwrite) this data file */
#define XPK_OutName	  XTAG(0x10)
/* File handle - write from curpos on */
#define XPK_OutFH	  XTAG(0x11)
/* Unblocked buf - must also supply OutBufLen */
#define XPK_OutBuf	  XTAG(0x12)
/* Master allocs OutBuf - ti_Data points to buf ptr */
#define XPK_GetOutBuf	  XTAG(0x13)
 /* Callback Hook to get output buffers */
#define XPK_OutHook	  XTAG(0x14)

/* Other tags for Pack/Unpack */
/* Length of data in input buffer */
#define XPK_InLen	  XTAG(0x20)
/* Length of output buffer     */
#define XPK_OutBufLen	  XTAG(0x21)
/* ti_Data pts to long to rec OutLen */
#define XPK_GetOutLen	  XTAG(0x22)
/* ti_Data pts to long to rec OutBufLen */
#define XPK_GetOutBufLen  XTAG(0x23)
/* Password for de/encoding    */
#define XPK_Password	  XTAG(0x24)
/* ti_Data points to buffer for errmsg */
#define XPK_GetError	  XTAG(0x25)
/* Memory type for output buffer */
#define XPK_OutMemType	  XTAG(0x26)
/* Bool: Passthru unrec. fmts on unpack */
#define XPK_PassThru	  XTAG(0x27)
/* Bool: Step down packmethod if nec. */
#define XPK_StepDown	  XTAG(0x28)
/* Call this Hook between chunks */
#define XPK_ChunkHook	  XTAG(0x29)
/* Do a FindMethod before packing */
#define XPK_PackMethod	  XTAG(0x2a)
/* Chunk size to try to pack with */
#define XPK_ChunkSize	  XTAG(0x2b)
/* Packing mode for sublib to use */
#define XPK_PackMode	  XTAG(0x2c)
/* Don't overwrite existing files */
#define XPK_NoClobber	  XTAG(0x2d)
/* Skip this tag               */
#define XPK_Ignore	  XTAG(0x2e)
/* Change priority for (un)packing */
#define XPK_TaskPri	  XTAG(0x2f)
/* File name for progress report */
#define XPK_FileName	  XTAG(0x30)
/* !!! obsolete !!!            */
#define XPK_ShortError	  XTAG(0x31)
/* Query available packers     */
#define XPK_PackersQuery  XTAG(0x32)
/* Query properties of a packer */
#define XPK_PackerQuery	  XTAG(0x33)
/* Query properties of packmode */
#define XPK_ModeQuery	  XTAG(0x34)
/* Lossy packing permitted? (FALSE)*/
#define XPK_LossyOK	  XTAG(0x35)
/* Ignore checksum             */
#define XPK_NoCRC         XTAG(0x36)
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
/* automatic password req.? (FALSE)*/
#define XPK_PassRequest   XTAG(0x42)
/* use prefs semaphore ? (TRUE) */
#define XPK_Preferences   XTAG(0x43)
/* automatic chunk report ? (FALSE)*/
#define XPK_ChunkReport	  XTAG(0x44)

/* tags XTAG(0x50) to XTAG(0x6F) are for XpkPassRequest -- see below */
#define XPK_MARGIN	256	/* Safety margin for output buffer	*/

#pragma pack(push, 1)

// temp
#define MAXPACKERS 100

struct XpkPackerList {
  unsigned int xpl_NumPackers;
/* NOTE: This isn't 100% compatible with the Amiga version! */
  unsigned int xpl_Packer[MAXPACKERS];
};
struct XpkMode {
  /* Chain to next descriptor for ModeDesc list*/
  struct XpkMode *xm_Next;
  /* Maximum efficiency handled by this mode */
  unsigned int xm_Upto;
  /* Defined below                         */
  unsigned int xm_Flags;
  /* Extra memory required during packing  */
  unsigned int xm_PackMemory;
  /* Extra memory during unpacking         */
  unsigned int xm_UnpackMemory;
  /* Approx packing speed in K per second  */
  unsigned int xm_PackSpeed;
  /* Approx unpacking speed in K per second */
  unsigned int xm_UnpackSpeed;
  /* CF in 0.1%				 */
  unsigned short xm_Ratio;
  /* Desired chunk size in K (!!) for this mode*/
  unsigned short xm_ChunkSize;
  /* 7 character mode description     */
  char  xm_Description[10];
};

struct XpkPackerInfo {
  /* Brief name of the packer      */
  char  xpi_Name[24];
  /* Full name of the packer       */
  char  xpi_LongName[32];
  /* One line description of packer */
  char  xpi_Description[80];
  /* Defined below                 */
  unsigned int xpi_Flags;
  /* Max input chunk size for packing */
  unsigned int xpi_MaxChunk;
  /* Default packing chunk size    */
  unsigned int xpi_DefChunk;
  /* Default mode on 0..100 scale  */
  unsigned short xpi_DefMode;
};

#pragma pack(pop)

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

// TODO: does this align correctly in x86/x64? (32/64 bit cpu??)
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
	uint32_t m_ChunkLen; // first chunk length (off: 12), or uncompressed length?
	
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

	// linked list of tags in XPK-file
	// (replace by chunks later?)
	XpkTag *m_pFirstTag;
	
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

	uint32_t Swap4(const uint32_t val) const
	{
		return (
				((val & 0x000000FF) << 24) + ((val & 0x0000FF00) <<8) |
				((val & 0x00FF0000) >> 8) + ((val & 0xFF000000) >>24)
				);
	}
	
	/* Get ID number from string */
	static char xpkupper(char c)
	{
	  if(c>='a' && c<='z')
	    c-='a'-'A';
	
	  return c;
	}
	
	unsigned int idfromname(char *name)
	{
	  unsigned int i, j=0;
	
	  for(i=4; i; i--) {
	    j<<=8;
	    j+=xpkupper(*(name++));
	  }
	
	  return Swap4(j);
	}
	

	// get size of structure following the tag..
	// this is necessary so we can skip/use it
	size_t getChunkSize(uint32_t tagID) const;
	
	void ReadFileInfo(CReadBuffer &Buffer);
	void ReadStreamHeader(CReadBuffer &Buffer);
	
	XpkTag *NextTag(CReadBuffer &Buffer, XpkTag *pPrevious);
	bool ReadTagData(CReadBuffer &Buffer, XpkTag *pTag);
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
