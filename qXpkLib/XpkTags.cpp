#include "XpkTags.h"


//////////////////
//
// XPK fileformat:
// chunk-based, crunched data in chunks.
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
	
	m_pFirst = new XpkChunk();
	m_pFirst->m_nDataOffset = 16; // include first 16 of original or not?
	m_pFirst->m_ChunkID = m_FileHeader.m_ChunkID;
	
	// note: above might give unpacked length of original
	// instead of common chunk-length..
	m_pFirst->m_ChunkLen = m_FileHeader.m_ChunkLen;
}


void XpkTags::ReadStreamHeader(CReadBuffer &Buffer)
{
	// note: could read&memcpy() but aligning is a pain also..
	// also need byteswaps -> just read and keep track of count

	/*
	XpkFib Fib;
	XpkStreamHeader Shead;
	
	*/
}

/////////////
// these may be tricky to convert..
//
TagItem *XpkTags::NextTagItem(CReadBuffer &Buffer, TagItem *pCurrent)
{
	// first tag
	if (pCurrent == nullptr)
	{
		pCurrent = new TagItem();
	}
	else if (pCurrent != nullptr)
	{
		pCurrent->next = new TagItem();
		pCurrent = pCurrent->next;
	}

	pCurrent->ti_Tag = (TagType)GetUWord(Buffer.GetNext(2));
	pCurrent->ti_Data = (void*)Buffer.GetAtCurrent();
	
    switch (pCurrent->ti_Tag) 
	{
    case TAG_SKIP:
      *tagItemPtr+=(unsigned int)(current->ti_Data)+1;
      break;
    case TAG_IGNORE:
      *tagItemPtr+=1;
      break;
    case TAG_MORE:
      *tagItemPtr=(struct TagItem *)current->ti_Data;
      break;
    case TAG_DONE:
      *tagItemPtr=0;
      break;
    default:
      *tagItemPtr+=1;
      break;
    }
	
	return pCurrent;
}

void XpkTags::ParseTags(CReadBuffer &Buffer)
{
	TagItem *pCurrent = nullptr;
	while (Buffer.IsEnd() == false)
	{
		pCurrent = NextTagItem(Buffer, pCurrent);
		if (pCurrent == nullptr)
		{
			break;
		}
	}
	
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
		return;
	}
	
	// keep original size
	//const size_t nOrigin = Buffer.GetCurrentPos();
	m_nTotalSize = Buffer.GetCurrentPos();
	
	// while processing
	Buffer.SetCurrentPos(0);


	// debugging of file format..
	ReadFileInfo(Buffer);
	
	//ReadStreamHeader(Buffer);
	//ParseTags(Buffer);
	


	// set original position
	Buffer.SetCurrentPos(m_nTotalSize);
}

