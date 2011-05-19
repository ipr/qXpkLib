#include "XpkTags.h"

void XpkTags::ReadFileHeader(CReadBuffer &Buffer)
{
	// note: could read&memcpy() but aligning is a pain also..
	// also need byteswaps -> just read and keep track of count
	
	
	m_Header.xsh_Pack = GetULong(Buffer.GetNext(4));
	m_Header.xsh_CLen = GetULong(Buffer.GetNext(4));
	m_Header.xsh_Type = GetULong(Buffer.GetNext(4));
	m_Header.xsh_ULen = GetULong(Buffer.GetNext(4));

	// some kinda buffer in between..
	Buffer.GetNext(16);

	m_Header.xsh_Flags = (*Buffer.GetNext(1));
	m_Header.xsh_HChk = (*Buffer.GetNext(1));
	m_Header.xsh_SubVrs = (*Buffer.GetNext(1));
	m_Header.xsh_MasVrs = (*Buffer.GetNext(1));
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
	
	
	ReadFileHeader(Buffer);
	


	


	// set original position
	Buffer.SetCurrentPos(m_nTotalSize);
}

