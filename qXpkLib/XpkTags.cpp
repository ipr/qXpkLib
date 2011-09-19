#include "XpkTags.h"


// determine size of data following tag id
// so we can skip/use it..
//
// struct-based has too many problems porting
// to 32/64 bit cpus so avoid them..
//
size_t XpkTags::getChunkSize(uint32_t tagID) const
{
	size_t nSize = 0;
	
	// note: some of these might not be stored..
	// no way to know yet..
	switch (tagID)
	{
	case XPK_InName:
		break;
	case XPK_InFH:
		break;
	case XPK_InBuf:
		break;
	case XPK_InHook:
		break;
	case XPK_OutName:
		break;
	case XPK_OutFH:
		break;
	case XPK_OutBuf:
		break;
	case XPK_GetOutBuf:
		break;
	case XPK_OutHook:
		break;
	case XPK_InLen:
		break;
	case XPK_OutBufLen:
		break;
	case XPK_GetOutLen:
		break;
	case XPK_GetOutBufLen:
		break;
	case XPK_Password:
		break;
	case XPK_GetError:
		break;
	case XPK_OutMemType:
		break;
	case XPK_PassThru:
		break;
	case XPK_StepDown:
		break;
	case XPK_ChunkHook:
		break;
	case XPK_PackMethod:
		break;
	case XPK_ChunkSize:
		break;
	case XPK_PackMode:
		break;
	case XPK_NoClobber:
		break;
	case XPK_Ignore:
		break;
	case XPK_TaskPri:
		break;
	case XPK_FileName:
		break;
	case XPK_ShortError: // obsolete? will we find this in old files?
		break;
	case XPK_PackersQuery:
		// temp!
		nSize = sizeof(XpkPackerList);
		break;
	case XPK_PackerQuery:
		// temp!
		nSize = sizeof(XpkPackerInfo);
		break;
	case XPK_ModeQuery:
		// temp!
		nSize = sizeof(XpkMode);
		break;
	case XPK_LossyOK:
		break;
	case XPK_NoCRC:
		break;
	case XPK_Key16:
		break;
	case XPK_Key32:
		break;
	case XPK_NeedSeek:
		break;
	case XPK_UseXfdMaster:
		break;
	case XPK_UseExternals:
		break;
	case XPK_PassRequest:
		break;
	case XPK_Preferences:
		break;
	case XPK_ChunkReport:
		break;
	case XPK_MARGIN:
		break;
	}
	return nSize;
}


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
	
	// TEST
	//pCurrent->m_Item.ti_Tag = Swap4(pCurrent->m_Item.ti_Tag);
	
	return pCurrent;
}

bool XpkTags::ReadTagData(CReadBuffer &Buffer, XpkTag *pTag)
{
	// temp!
	size_t nSize = getChunkSize(pTag->m_Item.ti_Tag);
	
	//// TEST!!
	// temp!
	switch (pTag->m_Item.ti_Tag)
	{
	case XPK_PackersQuery:
		{
		XpkPackerList *pMode = (XpkPackerList*)pTag->m_Item.ti_Tag;
		}
		break;
	case XPK_PackerQuery:
		{
		XpkPackerInfo *pMode = (XpkPackerInfo*)pTag->m_Item.ti_Tag;
		}
		break;
	case XPK_ModeQuery:
		{
		XpkMode *pMode = (XpkMode*)pTag->m_Item.ti_Tag;
		}
		break;
	case XPK_PackMethod:
		{
		unsigned int id = idfromname((char*)pTag->m_Item.ti_Tag);
		}
		break;
	case XPK_PackMode:
		{
		unsigned int *packmode = (unsigned int*)(pTag->m_Item.ti_Data);
		}
		break;
	case XPK_Preferences:
		{
		unsigned int *pPref = (unsigned int*)(pTag->m_Item.ti_Data);
		}
		break;
	}
	if (nSize > 0)
	{
		//pTag->m_pData = new uint8_t[nSize];
		//::memcpy(pTag->m_pData, Buffer.GetAtCurrent(), nSize);
		Buffer.SetCurrentPos(Buffer.GetCurrentPos() + nSize);
	}
	
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
	
	
	ParseTags(Buffer);


	// set original position
	Buffer.SetCurrentPos(m_nTotalSize);
}

