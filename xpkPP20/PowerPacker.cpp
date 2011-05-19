////////////////////////////////////////////////
// PowerPacker.cpp
//
// See details of classes in header-file.
//
// Based on PP-Tools in Aminet:
// Author: david tritscher
// Uploader: davidtritscher@dodo.com.au
//
// Modified to C++ by Ilkka Prusi, 2011
// ilkka.prusi@gmail.com
//
// Unlimited distribution.
//

#include "PowerPacker.h"

void CPowerPacker::UnPowerpack()
{
	uint bit = 0;
	uint len = 0;
	uint ptr = 0;

	ulong code = 0;
	uint shift = 32;

	// pointers for processing
	m_in_list.pos = m_in_list.ptr + m_in_list.size; 
	m_in_list.pos_end = m_in_list.ptr;
	m_out_list.pos = m_out_list.ptr + m_out_list.size; 
	m_out_list.pos_end = m_out_list.ptr;

	if (Peek(m_MetaBits.bitrot, m_in_list, code, shift) == false)
	{
		return;
	}
	Shift(m_MetaBits.bitrot, code, shift);

	if (Peek(1, m_in_list, code, shift) == false)
	{
		return;
	}

	uint protect = code >> 31;
	for(;;) 
	{
		if (Peek(3, m_in_list, code, shift) == false)
		{
			return;
		}

		bit = code >> 31; 
		Shift(1, code, shift);
		if(bit == protect) 
		{
			len = (code >> 30) + 1; 
			Shift(2, code, shift);
			if(len == 4) 
			{
				do 
				{
					if (Peek(2, m_in_list, code, shift) == false)
					{
						return;
					}
					bit = code >> 30; 
					Shift(2, code, shift);
					len += bit;
				} while(bit == 3);
			}

			if (len > m_out_list.pos - m_out_list.pos_end) 
			{
				throw PPException("bad character length");
			}

			while(len > 0) 
			{
				/* copy characters */
				if (Peek(8, m_in_list, code, shift) == false)
				{
					return;
				}
				*--m_out_list.pos = code >> 24; 
				Shift(8, code, shift); 
				len--;
			}

			if (m_out_list.pos == m_out_list.pos_end) 
			{
				break;
			}
		}

		if (Peek(3, m_in_list, code, shift) == false)
		{
			return;
		}

		BitSwitch(bit, len, code, shift);

		if (Peek(bit, m_in_list, code, shift) == false)
		{
			return;
		}

		ptr = (code >> (32 - bit)) + 1;

		Shift(bit, code, shift);

		if (ptr > m_out_list.pos_end + m_out_list.size - m_out_list.pos) 
		{
			throw PPException("bad string pointer");
		}

		if (len == 5) 
		{
			do 
			{
				if (Peek(3, m_in_list, code, shift) == false)
				{
					return;
				}
				bit = code >> 29; 
				Shift(3, code, shift);
				len += bit;
			} while(bit == 7);
		}

		if (len > m_out_list.pos - m_out_list.pos_end) 
		{
			throw PPException("bad string length");
		}

		uchar *str = m_out_list.pos + ptr;
		while(len > 0) 
		{
			*--m_out_list.pos = *--str; /* copy string */
			len--;
		}

		if (m_out_list.pos == m_out_list.pos_end)
		{
			break;
		}
	}
}

void CPowerPacker::LoadBuffer(const uchar *pData, const ulong nSize)
{
	// keep first four, check type
	::memcpy(m_Meta.meta, pData, 4);
	
	if (m_Meta.IsSupportedFiletype() == false)
	{
		throw PPException("file type is not supported");
	}

	::memcpy(m_Meta.meta + 6, pData + 4, 4);
	
	// get size for buffer allocation
	// (decrement what was already read at header)
	m_in_list.size = (nSize - 8);
	
	// limited to 16MB ?
	// also size must be in power of four?
	if (m_in_list.size < 8 || m_in_list.size > 0x1000000 || m_in_list.size % 4 != 0) 
	{
		throw PPException("bad compressed size");
	}

	// allocate separate buffer (should remove this..)
	m_in_list.ptr = (uchar*)malloc(m_in_list.size);
	::memcpy(m_in_list.ptr, pData + 8, m_in_list.size);

	// size is in last 4 bytes?
	m_in_list.size -= 4;
	::memcpy(m_Meta.meta + 10, m_in_list.ptr + m_in_list.size, 4);

	/* metadata */
	m_Meta.CopyMetaBits(m_MetaBits);
	if (m_MetaBits.IsMetadataOk() == false)
	{
		throw PPException("bad metadata");
	}

	m_out_list.size = m_Meta.GetUncompressedSize();
	if (m_out_list.size == 0) 
	{
		throw PPException("bad uncompressed size");
	}

	// allocate output (TODO: change..)
	m_out_list.ptr = (uchar*)malloc(m_out_list.size);
	if (m_out_list.ptr == 0) 
	{
		throw PPException("failed to allocate memory for output");
	}
}

