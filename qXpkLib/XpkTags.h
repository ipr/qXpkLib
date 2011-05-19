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


/* These structures define the file format for compressed streams */
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


class XpkTags
{
private:
	size_t m_nTotalSize;

	XpkStreamHeader m_Header;
	
protected:
	inline uint16_t GetUWord(const unsigned char *pBuf)
	{
		// combine&byteswap
		return ((pBuf[1] << 8) + pBuf[0]);
	}
	
	inline uint32_t GetULong(const unsigned char *pBuf)
	{
		// combine&byteswap
		return ((pBuf[3] << 24) + (pBuf[2] << 16) + (pBuf[1] << 8) + pBuf[0]);
	}
	
	void ReadFileHeader(CReadBuffer &Buffer);
	
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
