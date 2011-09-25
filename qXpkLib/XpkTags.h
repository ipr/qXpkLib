#ifndef XPKTAGS_H
#define XPKTAGS_H

#include <stdint.h>
#include <limits.h>

// for buffer-handler..
#include "AnsiFile.h"


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
//#define TAG_USER   ((unsigned int)(1L<<31))

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

class XpkTag
{
public:
	TagItem m_Item;
	//void *m_pData;
	
public:
	XpkTag(void)
		: m_Item()
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
#define XPK_TagBase	((1L<<31) + ('X'<<8) + 'P')
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


/* some bitmasks in flags-field of file header */
#define XPKSTREAMF_LONGHEADERS  0x01	/* Use XpkLongLocHeaders	*/
#define XPKSTREAMF_PASSWORD     0x02	/* This file encoded		*/
#define XPKSTREAMF_EXTHEADER    0x04	/* Extended globhdr		*/

/* chunk-header type values */
enum XpkChunkHeaderType
{
	XPKCHUNK_RAW	= 0x00,	// raw copy of source 
	XPKCHUNK_PACKED	= 0x01, // packed data 
	XPKCHUNK_END	= 0x0F // empty end Chunk 
};
//#define XPKCHUNK_RAW		0x00 /* raw copy of source */
//#define XPKCHUNK_PACKED		0x01 /* packed data */
//#define XPKCHUNK_END		0x0F /* empty end Chunk */

#pragma pack(push, 1)

/* These structures define the file format for compressed streams */
struct XpkStreamHeader 
{
  uint32_t xsh_Pack;
  uint32_t xsh_CLen;
  uint32_t xsh_Type;
  uint32_t xsh_ULen;
  
  // initial 16-bytes of original file?
  uint8_t xsh_Initial[16];
  
  // if flags & XPKSTREAMF_LONGHEADERS -> use "long" chunk header, otherwise use "word" length header
  uint8_t xsh_Flags; 
  uint8_t xsh_HChk;
  uint8_t xsh_SubVrs;
  uint8_t xsh_MasVrs;
};

struct XpkChunkHdrWord 
{
  uint8_t  xchw_Type;
  uint8_t  xchw_HChk; // header checksum?
  uint16_t xchw_CChk; // chunk checksum (16-bit CRC?)
  uint16_t xchw_CLen; // chunk length
  uint16_t xchw_ULen; // uncompressed length of chunk?
};

struct XpkChunkHdrLong 
{
  uint8_t  xchl_Type;
  uint8_t  xchl_HChk; // header checksum?
  uint16_t xchl_CChk; // chunk checksum (16-bit CRC?)
  uint32_t xchl_CLen; // chunk length
  uint32_t xchl_ULen; // uncompressed length of chunk?
};

#pragma pack(pop)



// new.. hopefully more clear..
//
// file header only
//
// note: don't cast directly from buffer, might not align
//
/*
struct XpkFileInfo
{
	// first 16 bytes similar to common IFF-style
	uint32_t m_ID; // IFF-style ID (off: 0)
	uint32_t m_FileLen; // IFF-style length (off: 4)
	uint32_t m_ChunkID; // IFF-style ID of cruncher (off: 8)
	uint32_t m_ChunkLen; // first chunk length (off: 12), or uncompressed length?
	
	// First 16 bytes of orig. file (or original file name?)
    char  m_Orig_Head[16];
    
	//int32_t m_CompressionRatio;
	//uint32_t m_Reserved[8];
};
*/

class XpkChunk;
class XpkChunk
{
public:
	XpkChunk(XpkChunk *pPrev = nullptr)
	    : m_nDataOffset(0)
	    , m_pPrevious(pPrev)
	    , m_pNext(nullptr)
	    , m_Type(0)
	    , m_HChecksum(0)
	    , m_ChunkChecksum(0)
	    , m_ChunkLength(0)
	    , m_ULen(0)
	{}
	~XpkChunk()
	{}
	
	size_t m_nDataOffset; // offset to data (after chunk header)
	XpkChunk *m_pPrevious; // preceding chunk
	XpkChunk *m_pNext; // next chunk
	
	//uint32_t m_ChunkID; // IFF-style chunk-ID tag
	//uint32_t m_ChunkLen; // chunk length 
	//XpkTag *m_pTag;

	// chunk header fields
	size_t m_Type; // see XpkChunkHeaderType
	size_t m_HChecksum; // header checksum?
	size_t m_ChunkChecksum; // chunk checksum (16-bit CRC?)
	size_t m_ChunkLength; // chunk length
	size_t m_ULen; // uncompressed length of chunk?
	
};


class XpkTags
{
private:
	size_t m_nTotalSize;
	
	XpkStreamHeader m_streamHeader;
	//XpkFileInfo m_FileHeader;
	
	XpkChunk *m_pFirst;

	// linked list of tags in XPK-file
	// (replace by chunks later?)
	//XpkTag *m_pFirstTag;
	
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

	uint16_t Swap2(const uint16_t val) const
    {
        return (((val >> 8)) | (val << 8));
    }
	uint32_t Swap4(const uint32_t val) const
	{
		return (
				((val & 0x000000FF) << 24) + ((val & 0x0000FF00) <<8) |
				((val & 0x00FF0000) >> 8) + ((val & 0xFF000000) >>24)
				);
	}
	
	/* Get ID number from string */
	/*
	static char xpkupper(char c)
	{
	  if(c>='a' && c<='z')
	    c-='a'-'A';
	
	  return c;
	}
	*/

	/*	
	unsigned int idfromname(char *name)
	{
	  unsigned int i, j=0;
	
	  for(i=4; i; i--) {
	    j<<=8;
	    j+=xpkupper(*(name++));
	  }
	
	  return Swap4(j);
	}
	*/
	

	
	XpkTag *NextTag(CReadBuffer &Buffer, XpkTag *pPrevious);
	//bool ReadTagData(CReadBuffer &Buffer, XpkTag *pTag);
	//void ParseTags(CReadBuffer &Buffer);

	void ReadChunks(CReadBuffer &Buffer);

	void ReadFileInfo(CReadBuffer &Buffer);
	
public:
    XpkTags();
    ~XpkTags();
	
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
