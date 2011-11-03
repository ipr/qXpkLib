///////////////////////////////////
//
// XpkTags:
// fileformat handling, chunk handler,
// should call given library to decrunch each chunk.
//
// Ilkka Prusi
// ilkka.prusi@gmail.com
//


#ifndef XPKTAGS_H
#define XPKTAGS_H

#include <stdint.h>
#include <limits.h>

// for buffer-handler..
#include "AnsiFile.h"



/* some bitmasks in flags-field of file header */
enum XpkStreamFlags
{
	XPKSTREAMF_LONGHEADERS  = 0x01, // Use XpkLongLocHeaders
	XPKSTREAMF_PASSWORD     = 0x02, // This file encoded
	XPKSTREAMF_EXTHEADER    = 0x04  // Extended globhdr
};

/* chunk-header type values */
enum XpkChunkHeaderType
{
	XPKCHUNK_RAW	= 0x00,	// raw copy of source 
	XPKCHUNK_PACKED	= 0x01, // packed data 
	XPKCHUNK_END	= 0x0F // empty end Chunk 
};


/* These structures define the file format for compressed streams 

   note: don't cast directly to struct without packing-pragma
*/
struct XpkStreamHeader 
{
  uint32_t xsh_PackID; // common 4-byte IFF-style ID (magic number)
  uint32_t xsh_CompressedLen; // size of file minus two first fields (8 bytes)
  uint32_t xsh_PackerType; // type of sub-library (4-byte IFF-style ID)
  uint32_t xsh_UnpackedLen; // uncompressed length of all data in file?
  
  // initial 16-bytes of original file?
  uint8_t xsh_Initial[16];
  
  // if flags & XPKSTREAMF_LONGHEADERS -> use "long" chunk header, otherwise use "word" length header
  uint8_t xsh_Flags; 
  
  // this header checksum?
  uint8_t xsh_HeaderChk;
  
  // required version of sub-library?
  uint8_t xsh_SubVrs;

  // master-library version?  
  uint8_t xsh_MasVrs;
};

// following need forced to 1-byte align, no padding,
// converted to XpkChunk for better usability in code
#pragma pack(push, 1)

// chunk header may have either of two formats
// (per-file flag in stream-header),
// can cast with packing-pragma:
// these should be converted to in-memory types anyway.
//
struct XpkChunkHdrWord 
{
  uint8_t  xchw_Type;
  uint8_t  xchw_HChk; // header checksum?
  uint16_t xchw_CChk; // chunk checksum (16-bit CRC?)
  uint16_t xchw_CLen; // chunk compressed length
  uint16_t xchw_ULen; // uncompressed length of chunk data
};

struct XpkChunkHdrLong 
{
  uint8_t  xchl_Type;
  uint8_t  xchl_HChk; // header checksum?
  uint16_t xchl_CChk; // chunk checksum (16-bit CRC?)
  uint32_t xchl_CLen; // chunk compressed length
  uint32_t xchl_ULen; // uncompressed length of chunk data
};

#pragma pack(pop)


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
	    , m_UnLen(0)
	{}
	~XpkChunk()
	{}
	
	size_t m_nDataOffset; // offset to data (after chunk header)
	XpkChunk *m_pPrevious; // preceding chunk
	XpkChunk *m_pNext; // next chunk
	
	// chunk header fields
	size_t m_Type; // see XpkChunkHeaderType
	size_t m_HChecksum; // header checksum?
	size_t m_ChunkChecksum; // chunk checksum (16-bit CRC?)
	size_t m_ChunkLength; // chunk compressed length
	size_t m_UnLen; // uncompressed length of chunk data
	
};

// fileformat types
enum XpkFormat
{
	XPKMODE_UNKNOWN = 0,
	XPKMODE_UPUP = 1,
	XPKMODE_UPSTD = 2,
	XPKMODE_UPPP = 3,
	XPKMODE_UPXFD = 4,
	XPKMODE_PKSTD = 20
};


class XpkTags
{
private:
	XpkStreamHeader m_streamHeader;
	XpkFormat m_formatType;
	
	// size of extended header (if exists in file)	
	size_t m_extHeaderLen;
	
	// type name/ID of cruncher used in file
	std::string m_typeName;
	
	XpkChunk *m_pFirst;

protected:
	inline uint16_t GetUWord(const unsigned char *pBuf)
	{
		return ((pBuf[0] << 8) + pBuf[1]);
	}
	
	inline uint32_t GetULong(const unsigned char *pBuf)
	{
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

	// need better way of sharing code..
	uint32_t MakeTag(const char *buf) const
    {
        uint32_t tmp = 0;
        tmp |= (((uint32_t)(buf[3])) << 24);
        tmp |= (((uint32_t)(buf[2])) << 16);
        tmp |= (((uint32_t)(buf[1])) << 8);
        tmp |= ((uint32_t)(buf[0]));
        return tmp;
    }
	
	bool verifyHeaderLong(XpkChunkHdrLong *pChunkHeader);
	bool verifyHeaderWord(XpkChunkHdrWord *pChunkHeader);

	void ReadChunks(CIOBuffer &Buffer);

	bool ReadStreamHeader(CIOBuffer &Buffer);
	
	void Clear();
	
public:
    XpkTags();
    ~XpkTags();
	
	// verify that file is XPK:
	// expect certain structure
	// regardless of sub-type (packer)
	bool isXpkFile(const uint8_t *buffer) const;
	
	bool ParseHeader(CIOBuffer *pBuffer);
	bool ParseChunks(CIOBuffer *pBuffer);

	// TODO: add this so single-pass
	// can be done without loading entire file to memory
	//XpkChunk *nextChunk(CReadBuffer &Buffer, XpkChunk *pCurrent = nullptr);
	
	bool verifyChecksum(XpkChunk *pChunk, CIOBuffer *pOutBuffer);
	
	XpkFormat getFormat() const
	{
		return m_formatType;
	}
	
	const XpkStreamHeader *getHeader() const
	{
		return &m_streamHeader;
	}
	
	const std::string& getTypename() const
	{
		return m_typeName;
	}
	
	XpkChunk *getFirst() const
	{
		return m_pFirst;
	}
	
};

#endif // XPKTAGS_H
