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
	XpkTag *pCurrent = m_pFirst;
	Buffer.SetCurrentPos(m_pFirst->m_nDataOffset);
	while (Buffer.IsEnd() == false)
	{
		XpkChunkHeader *pHdr = (XpkChunkHeader*)Buffer.GetAtCurrent();
	
	
	
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
//
// First chunk:
// - 
// (TODO)
//

void XpkTags::ReadFileInfo(CReadBuffer &Buffer)
{
	m_FileHeader.m_ID = GetULong(Buffer.GetNext(4));
	m_FileHeader.m_FileLen = GetULong(Buffer.GetNext(4));
	m_FileHeader.m_ChunkID = GetULong(Buffer.GetNext(4));
	
	// note: unpacked-length of original instead of normal length of chunk?
	m_FileHeader.m_ChunkLen = GetULong(Buffer.GetNext(4));

	// first 16 bytes of original file
	::memcpy(m_FileHeader.m_Orig_Head, Buffer.GetNext(16), 16);
	
	// TODO: is there more here before first chunk?
	// offsets seems wrong somehow..

	m_pFirst = new XpkChunk();
	m_pFirst->m_nDataOffset = 16; // include first 16 of original or not?
	m_pFirst->m_ChunkID = m_FileHeader.m_ChunkID;
	
	// note: above might give unpacked length of original
	// instead of common chunk-length..
	m_pFirst->m_ChunkLen = m_FileHeader.m_ChunkLen;
}

//////////// public methods

XpkTags::XpkTags()
    : m_nTotalSize(0)
{
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

