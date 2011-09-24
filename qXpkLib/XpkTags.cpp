#include "XpkTags.h"




/////////////
// these may be tricky to convert..
//
// old stuff uses tag ID to decide type of struct there is
// and they won't compile correctly on 32/64 bit cpus..
// get rid of them entirely instead of hacking something together
// and replace with something more easily portable.
//
XpkTag *XpkTags::NextTag(CReadBuffer &Buffer, XpkTag *pPrevious)
{
	// first tag
	/*
	if (pCurrent == nullptr)
	{
		pCurrent = new TagItem();
	}
	else if (pCurrent != nullptr)
	{
		pCurrent->next = new TagItem();
		pCurrent = pCurrent->next;
	}
	*/
	
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

bool XpkTags::ReadTagData(CReadBuffer &Buffer, XpkTag *pTag)
{
	// temp!
	//size_t nSize = getChunkSize(pTag->m_Item.ti_Tag);
	
	return false;
}

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


void XpkTags::ReadChunks(CReadBuffer &Buffer)
{
	m_pFirst = new XpkChunk();
	m_pFirst->m_nDataOffset = Buffer.GetCurrentPos();

	XpkChunk *pCurrent = m_pFirst;
	while (Buffer.IsEnd() == false)
	{
		size_t nChunkHdrSize = 0;
		size_t nChunkCompSize = 0; // compressed data size of chunk
		
		if (m_streamHeader.xsh_Flags & XPKSTREAMF_LONGHEADERS)
		{
			XpkChunkHdrLong *pHdr = (XpkChunkHdrLong*)Buffer.GetAtCurrent();
			
			pCurrent->m_chunkHeader.m_Type = pHdr->xchl_Type;
			pCurrent->m_chunkHeader.m_HChk = pHdr->xchl_HChk;
			pCurrent->m_chunkHeader.m_CChk = GetUWord(&(pHdr->xchl_CChk));
			pCurrent->m_chunkHeader.m_CLen = GetULong(&(pHdr->xchl_CLen));
			pCurrent->m_chunkHeader.m_ULen = GetULong(&(pHdr->xchl_ULen));
			
			nChunkHdrSize = sizeof(XpkChunkHdrLong);
			nChunkCompSize = pHdr->xchl_CLen;
		}
		else
		{
			XpkChunkHdrWord *pHdr = (XpkChunkHdrWord*)Buffer.GetAtCurrent();
			
			pCurrent->m_chunkHeader.m_Type = pHdr->xchw_Type;
			pCurrent->m_chunkHeader.m_HChk = pHdr->xchw_HChk;
			pCurrent->m_chunkHeader.m_CChk = GetUWord(&(pHdr->xchw_CChk));
			pCurrent->m_chunkHeader.m_CLen = GetUWord(&(pHdr->xchw_CLen));
			pCurrent->m_chunkHeader.m_ULen = GetUWord(&(pHdr->xchw_ULen));

			nChunkHdrSize = sizeof(XpkChunkHdrWord);
			nChunkCompSize = pHdr->xchw_CLen;
		}
		
		// .. process chunk

		pCurrent->m_nDataOffset += nChunkHdrSize;
		Buffer.SetCurrentPos(nNextChunkOffset);
		

		
		// offset to start of next chunk..?
		size_t nNextChunkOffset = pCurrent->m_nDataOffset + nChunkCompSize;
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
// (more details??)
// 
// File header:
// starts as common IFF-style header:
// - 4-byte ID, 'XPKF'
// - 4-byte int for filesize minus header (8)
// - 4-byte sub-type (cruncher-ID) e.g. 'SQSH'
// - 4-byte int for chunk-size (actually: unpacked length?)
// - first 16-bytes of original file?
// -> not exactly this but close.. 
//
// First chunk:
// - 
// (TODO)
//

void XpkTags::ReadFileInfo(CReadBuffer &Buffer)
{
	Buffer.SetCurrentPos(0);
	
	// "XPKF"
	m_streamHeader.xsh_Pack = GetULong(Buffer.GetNext(4));
	// compressed length?
	m_streamHeader.xsh_CLen = GetULong(Buffer.GetNext(4));
	// packer type, e.g. "SQSH", "NUKE", "RLEN"..
	m_streamHeader.xsh_Type = GetULong(Buffer.GetNext(4));
	// uncompressed length?
	m_streamHeader.xsh_ULen = GetULong(Buffer.GetNext(4));

	// first 16 bytes of original file
	::memcpy(m_streamHeader.xsh_Initial, Buffer.GetNext(16), 16);
	
	// flags
	m_streamHeader.xsh_Flags = Buffer.GetNextByte();
	
	// ..no idea..
	m_streamHeader.xsh_HChk = Buffer.GetNextByte();
	
	// minor&major version of XPK master/cruncher?
	m_streamHeader.xsh_SubVrs = Buffer.GetNextByte();
	m_streamHeader.xsh_MasVrs = Buffer.GetNextByte();
	
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

	// debugging of file format..
	ReadFileInfo(Buffer);
	
	ReadChunks(Buffer);
	
	//ParseTags(Buffer);


	// set original position
	Buffer.SetCurrentPos(m_nTotalSize);
}

