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


/////////////
// these may be tricky to convert..
//
// old stuff uses tag ID to decide type of struct there is
// and they won't compile correctly on 32/64 bit cpus..
// get rid of them entirely instead of hacking something together
// and replace with something more easily portable.
//
/*
XpkTag *XpkTags::NextTag(CReadBuffer &Buffer, XpkTag *pPrevious)
{
	if (pPrevious != nullptr)
	{
		// amount to change offset:
		// don't cast directly to struct
		// since ptr size may vary (4/8 bytes)
		size_t nToOffset = 0;
		
		switch (pPrevious->m_Item.ti_Tag) 
		{
		case TAG_SKIP: 
			// just tag ID ?
			nToOffset += sizeof(uint32_t);
			//*tagItemPtr+=(unsigned int)(current->ti_Data)+1;
			break;
		case TAG_IGNORE:
			// ignore tag
			//*tagItemPtr+=1;
			nToOffset += sizeof(uint32_t)*2;
			break;
		case TAG_MORE:
			// tag data is another tag..?
			//*tagItemPtr=(struct TagItem *)current->ti_Data;
			nToOffset += sizeof(uint32_t);
			break;
		case TAG_DONE:
			// no more tags
			return nullptr;
		default:
			// unknown tag (user-tag?)
			//*tagItemPtr+=1;
			nToOffset += sizeof(uint32_t)*2;
			break;
		}
		
		Buffer.SetCurrentPos(Buffer.GetCurrentPos()+nToOffset);
	}
	
	XpkTag *pCurrent = new XpkTag();
	if (pPrevious != nullptr)
	{
		pPrevious->m_pNext = pCurrent;
	}
	
	pCurrent->m_Item.ti_Tag = (uint32_t)GetULong(Buffer.GetNext(4)); // tag ID
	pCurrent->m_Item.ti_Data = (void*)Buffer.GetAtCurrent(); // data of tag in buffer
	
	return pCurrent;
}
*/

/*
bool XpkTags::ReadTagData(CReadBuffer &Buffer, XpkTag *pTag)
{
	// temp!
	//size_t nSize = getChunkSize(pTag->m_Item.ti_Tag);
	
	return false;
}
*/

/*
void XpkTags::ParseTags(CReadBuffer &Buffer)
{
	XpkTag *pCurrent = nullptr;
	while (Buffer.IsEnd() == false)
	{
		XpkTag *pNext = NextTag(Buffer, pCurrent);
		if (pNext == nullptr)
		{
			break;
		}
		
		ReadTagData(Buffer, pNext);
		if (pCurrent == nullptr)
		{
			m_pFirstTag = pNext;
		}
		pCurrent = pNext;
	}
}
*/

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
		if (m_streamHeader.xsh_Flags & XPKSTREAMF_LONGHEADERS)
		{
			XpkChunkHdrLong *pHdr = (XpkChunkHdrLong*)Buffer.GetAtCurrent();
			
			pCurrent->m_Type = pHdr->xchl_Type;
			pCurrent->m_HChecksum = pHdr->xchl_HChk;
			pCurrent->m_ChunkChecksum = Swap2(pHdr->xchl_CChk);
			pCurrent->m_ChunkLength = Swap4(pHdr->xchl_CLen);
			pCurrent->m_ULen = Swap4(pHdr->xchl_ULen);
			
			pCurrent->m_nDataOffset += sizeof(XpkChunkHdrLong);
		}
		else
		{
			XpkChunkHdrWord *pHdr = (XpkChunkHdrWord*)Buffer.GetAtCurrent();
			
			pCurrent->m_Type = pHdr->xchw_Type;
			pCurrent->m_HChecksum = pHdr->xchw_HChk;
			pCurrent->m_ChunkChecksum = Swap2(pHdr->xchw_CChk);
			pCurrent->m_ChunkLength = Swap2(pHdr->xchw_CLen);
			pCurrent->m_ULen = Swap2(pHdr->xchw_ULen);

			pCurrent->m_nDataOffset += sizeof(XpkChunkHdrWord);
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
void XpkTags::ReadFileInfo(CReadBuffer &Buffer)
{
	Buffer.SetCurrentPos(0);
	
	// default
	m_formatType = XPKMODE_UPSTD;
	
	// "XPKF", 0x58504b46 (XPK_COOKIE, magic number)
	// note: support also XFD-packed files? ("XFDD")
	m_streamHeader.xsh_Pack = GetULong(Buffer.GetNext(4));
	
	// file length without IFF header (type+length = 8) ?
	m_streamHeader.xsh_CLen = GetULong(Buffer.GetNext(4));
	
	// packer type, e.g. "SQSH", "NUKE", "RLEN"..
	m_streamHeader.xsh_Type = GetULong(Buffer.GetNext(4));
	// uncompressed length?
	m_streamHeader.xsh_ULen = GetULong(Buffer.GetNext(4));

	// first 16 bytes of original file
	::memcpy(m_streamHeader.xsh_Initial, Buffer.GetNext(16), 16);
	
	// flags
	m_streamHeader.xsh_Flags = Buffer.GetNextByte();
	
	/*
	// also check "XFDD", 0x58464444 in file ID?
	if (m_streamHeader.xsh_Flags & XMF_XFD && m_streamHeader.xsh_Pack == "XFDD")
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
	m_streamHeader.xsh_HChk = Buffer.GetNextByte();
	
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
		throw ArcException("Header checksum error", m_streamHeader.xsh_HChk);
    }
    
    // has extended header?
    if (m_streamHeader.xsh_Flags & XPKSTREAMF_EXTHEADER)
    {
		// size of extended header if present?
		uint16_t extheaderLen = GetUWord(Buffer.GetNext(2));
		
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
        extheaderLen += sizeof(uint16_t);	/* for unwinding while XpkExamine */
        
        // note: increase buffer position by size of extended header?
	}
	
	// -> continue with chunks..
}

//////////// public methods

XpkTags::XpkTags()
    : m_nTotalSize(0)
    , m_streamHeader()
    , m_pFirst(nullptr)
{
}

XpkTags::~XpkTags()
{
	XpkChunk *pCurrent = m_pFirst;
	while (pCurrent != nullptr)
	{
		XpkChunk *pNext = pCurrent->m_pNext;
		delete pCurrent;
		pCurrent = pNext;
	}
}

void XpkTags::ParseToNodeList(CReadBuffer &Buffer)
{
	if (IsXpkFile(Buffer) == false)
	{
		// can't do anything about it
		//throw IOException("Not XPK file");
		return;
	}
	
	// keep original size
	m_nTotalSize = Buffer.GetCurrentPos();
	Buffer.SetCurrentPos(0); // start at beginning..

	// read&parse file header as "streamheader"
	ReadFileInfo(Buffer);

	// read chunks from file
	ReadChunks(Buffer);

	// set original position
	Buffer.SetCurrentPos(m_nTotalSize);
}

