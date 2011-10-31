///////////////////////////////////
//
// XpkTags:
// fileformat handling, chunk handler,
// should call given library to decrunch each chunk.
//
// Ilkka Prusi
// ilkka.prusi@gmail.com
//

#include "XpkTags.h"

// checksum functions
#include "xpkmaster_checksum.h"


// verify chunk header according to size of it
bool XpkTags::verifyHeaderLong(XpkChunkHdrLong *pChunkHeader)
{
	// is this correct?
    // should verify against: pChunkHeader->xchl_HChk ?
    if (hchecksum((uint8_t*)pChunkHeader, sizeof(XpkChunkHdrLong)) == 0)
    {
		return true;
    }
	return false;
}

// verify chunk header according to size of it
bool XpkTags::verifyHeaderWord(XpkChunkHdrWord *pChunkHeader)
{
	// is this correct?
    // should verify against: pChunkHeader->xchw_HChk ?
    if (hchecksum((uint8_t*)pChunkHeader, sizeof(XpkChunkHdrWord)) == 0)
    {
		return true;
    }
	return false;
}

//////////////////
//
// XPK fileformat:
// chunk-based, crunched data in chunks
//
// Chunk format:
// * chunk header
// - 1 byte for chunk type
// - 1 byte for chunk header checksum ?
// - 2 bytes for chunk checksum
// - 2/4 bytes for chunk (compressed) length
// - 2/4 bytes for uncompressed length?
// * chunk data
// - amount of packed data given in chunk header..
//
// Note: chunk header may have different size in different file
// according to flags (if "long" sizes are used for chunks).
//
//
void XpkTags::ReadChunks(CReadBuffer &Buffer)
{
	m_pFirst = new XpkChunk();
	m_pFirst->m_nDataOffset = Buffer.GetCurrentPos();

	XpkChunk *pCurrent = m_pFirst;
	while (Buffer.IsEnd() == false)
	{
		bool isValidHeader = false;
		if (m_streamHeader.xsh_Flags & XPKSTREAMF_LONGHEADERS)
		{
			XpkChunkHdrLong *pHdr = (XpkChunkHdrLong*)Buffer.GetAtCurrent();
			
			pCurrent->m_Type = pHdr->xchl_Type;
			pCurrent->m_HChecksum = pHdr->xchl_HChk;
			pCurrent->m_ChunkChecksum = Swap2(pHdr->xchl_CChk);
			pCurrent->m_ChunkLength = Swap4(pHdr->xchl_CLen);
			pCurrent->m_UnLen = Swap4(pHdr->xchl_ULen);
			
			pCurrent->m_nDataOffset += sizeof(XpkChunkHdrLong);
			isValidHeader = verifyHeaderLong(pHdr);
		}
		else
		{
			XpkChunkHdrWord *pHdr = (XpkChunkHdrWord*)Buffer.GetAtCurrent();
			
			pCurrent->m_Type = pHdr->xchw_Type;
			pCurrent->m_HChecksum = pHdr->xchw_HChk;
			pCurrent->m_ChunkChecksum = Swap2(pHdr->xchw_CChk);
			pCurrent->m_ChunkLength = Swap2(pHdr->xchw_CLen);
			pCurrent->m_UnLen = Swap2(pHdr->xchw_ULen);

			pCurrent->m_nDataOffset += sizeof(XpkChunkHdrWord);
			isValidHeader = verifyHeaderWord(pHdr);
		}
		
		// TODO: need header checksum verification somewhere around here..
		if (isValidHeader == false)
		{
			// TODO: exception or skip ?
		}
		
		if (pCurrent->m_Type != XPKCHUNK_RAW
			&& pCurrent->m_Type != XPKCHUNK_PACKED
			&& pCurrent->m_Type != XPKCHUNK_END)
		{
			// TODO: exception or skip ?
		}
		
		// move to actual data of chunk (according to chunk header size)
		Buffer.SetCurrentPos(pCurrent->m_nDataOffset);

		// "end-of-file" chunk? (empty)
		if (pCurrent->m_Type == XPKCHUNK_END)
		{
			return;
		}
		
		// TODO:
		// .. process chunk
		// -> walkthrough in XPK-master now..
		
		// offset to start of next chunk:
		// start of data in current + size of data in current
		size_t nNextChunkOffset = pCurrent->m_nDataOffset + pCurrent->m_ChunkLength;
		Buffer.SetCurrentPos(nNextChunkOffset);
		
		pCurrent->m_pNext = new XpkChunk(pCurrent);
		pCurrent->m_pNext->m_nDataOffset = nNextChunkOffset;
		pCurrent = pCurrent->m_pNext;
	}
}


//////////////////
//
// XPK fileformat:
// chunk-based, crunched data in chunks
// 
// File header:
// starts as common IFF-style header:
// - 4-byte ID, 'XPKF'
// - 4-byte int for filesize minus header (8)
// - 4-byte sub-type (cruncher-ID) e.g. 'SQSH'
// - 4-byte int for total uncompressed length ?
// - first 16-bytes of original file
// - 1 byte for flags
// - 1 byte for header checksum ?
// - 1 byte for minor version of cruncher/library ?
//  - actually version required of sub-library..
// - 1 byte for major version of cruncher/library ?
//
bool XpkTags::ReadFileInfo(CReadBuffer &Buffer)
{
	// should have enough data to actually parse file header
	if (Buffer.GetSize() < sizeof(XpkStreamHeader))
	{
		return false;
	}
	
	if (isXpkFile(Buffer.GetBegin()) == false)
	{
		return false;
	}
	Buffer.SetCurrentPos(0); // start at beginning if not already..
	
	// set default
	m_formatType = XPKMODE_UPSTD;
	
	// "XPKF", 0x58504b46 (XPK_COOKIE, magic number)
	// note: support also XFD-packed files? ("XFDD")
	m_streamHeader.xsh_PackID = GetULong(Buffer.GetNext(4));
	
	if (m_streamHeader.xsh_PackID != MakeTag("XPKF"))
	// this too ? && m_streamHeader.xsh_PackID != MakeTag("XFDD"))
	{
		return false;
	}
	
	// file length without IFF header (type+length = 8) ?
	m_streamHeader.xsh_CompressedLen = GetULong(Buffer.GetNext(4));
	
	// keep packer type as type name/ID,
	// just access as-is
	m_typeName.assign((char*)Buffer.GetAtCurrent(), 4);
	
	// packer type, e.g. "SQSH", "NUKE", "RLEN"..
	m_streamHeader.xsh_PackerType = GetULong(Buffer.GetNext(4));
	
	// TODO: check supported types..? if != MakeTag()..
	// -> caller/parent should do (knows libraries..)
	
	// uncompressed length?
	m_streamHeader.xsh_UnpackedLen = GetULong(Buffer.GetNext(4));

	// first 16 bytes of original file
	::memcpy(m_streamHeader.xsh_Initial, Buffer.GetNext(16), 16);
	
	// flags
	m_streamHeader.xsh_Flags = Buffer.GetNextByte();
	
	/*
	// also check "XFDD", 0x58464444 in file ID?
	if (m_streamHeader.xsh_Flags & XMF_XFD && m_streamHeader.xsh_PackID == MakeTag("XFDD"))
	{
		m_formatType = XPKMODE_UPXFD;
	}
	
	if (m_streamHeader.xsh_Flags & XMF_PASSTHRU)
	{
		// unpacked?
		m_formatType = XPKMODE_UPUP;
	}
	
	if (m_streamHeader.xsh_Flags & XPKSTREAMF_PASSWORD)
	{
		// password-protected file?
	}
	*/
	
	// ..no idea.. header checksum?
	m_streamHeader.xsh_HeaderChk = Buffer.GetNextByte();
	
	// minor&major version of XPK master/cruncher?
	m_streamHeader.xsh_SubVrs = Buffer.GetNextByte(); // sub-library version required?
	m_streamHeader.xsh_MasVrs = Buffer.GetNextByte();
	
	// TODO: remove later, debug-test..
	if (Buffer.GetCurrentPos() != sizeof(XpkStreamHeader))
	{
		throw IOException("Read size does not match stream-header size");
	}
	
	// non-zero header checksum? (note where doing checksumming..)
    if (hchecksum(Buffer.GetBegin(), sizeof(XpkStreamHeader)) != 0)
    {
		throw ArcException("Header checksum error", m_streamHeader.xsh_HeaderChk);
    }
    
    // has extended header?
    if (m_streamHeader.xsh_Flags & XPKSTREAMF_EXTHEADER)
    {
		// size of extended header if present?
		m_extHeaderLen = GetUWord(Buffer.GetNext(2));
		
		/*
		// this done above..		
	    if(!hookread(xbuf, XIO_READ, &exthlen, sizeof(UWORD)))
			goto Abort; -> exception
			
		// no read, just skip it??
        if(!hookread(xbuf, XIO_READ, NULL, exthlen))
			goto Abort; -> exception
			
		// later it is read anyway, why not just directly when detected?
			
		*/
		
		// increment by length-field size anyway..
        m_extHeaderLen += sizeof(uint16_t);	/* for unwinding while XpkExamine */
        
        // note: increase buffer position by size of extended header?
	}

	// header parsed, should be valid file?	
	return true;
}


void XpkTags::Clear()
{

	XpkChunk *pCurrent = m_pFirst;
	while (pCurrent != nullptr)
	{
		XpkChunk *pNext = pCurrent->m_pNext;
		delete pCurrent;
		pCurrent = pNext;
	}
	m_pFirst = nullptr;
	m_typeName.clear();
}

//////////// public methods

XpkTags::XpkTags()
    : m_streamHeader()
    , m_formatType(XPKMODE_UNKNOWN)
    , m_extHeaderLen(0)
    , m_typeName()
    , m_pFirst(nullptr)
{
}

XpkTags::~XpkTags()
{
	Clear();
}

// verify that file is XPK:
// expect certain structure
// regardless of sub-type (packer)
bool XpkTags::isXpkFile(const uint8_t *buffer) const
{
	if (::memcmp(buffer, "XPKF", 4) == 0)
	{
		return true;
	}
	return false;
}

// used to check and parse file metadata
// to detect support file
// -> don't throw exception if invalid file?
bool XpkTags::ParseHeader(CReadBuffer *pBuffer)
{
	// read&parse file header 
	return ReadFileInfo(*pBuffer);
}

// parse information,
// should throw exception on error? 
// (cannot continue where expected to..)
bool XpkTags::ParseChunks(CReadBuffer *pBuffer)
{
	Clear(); // should make new instance of this instead..

	// read&parse file header 
	if (ReadFileInfo(*pBuffer) == false)
	{
		return false;
	}

	// read chunks from file
	ReadChunks(*pBuffer);
	return true;
}

// TODO: add this so single-pass
// can be done without loading entire file to memory
/*
XpkChunk *XpkTags::nextChunk(CReadBuffer &Buffer, XpkChunk *pCurrent)
{
	if (pCurrent == nullptr)
	{
		// find first chunk
	}
	else
	{
		// find next chunk
	}
}
*/

// verify checksum on chunk data
// (after unpacking)
//
bool XpkTags::verifyChecksum(XpkChunk *pChunk, CReadBuffer *pOutBuffer)
{
	uint8_t *pBuf = pOutBuffer->GetAt(pOutBuffer->GetCurrentPos() - pChunk->m_UnLen);
	uint16_t checksum = cchecksum(pBuf, pChunk->m_UnLen);
	if (checksum == pChunk->m_ChunkChecksum)
	{
		return true;
	}
	return false;
}
