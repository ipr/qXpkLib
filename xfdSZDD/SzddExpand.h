/* SZDD-expand: a portable Microsoft COMPRESS.EXE file decruncher
 * Copyright (C) 2000 Stuart Caie <kyzer@4u.net>
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 *
 * Files in the SZDD file format are created with Microsoft's COMPRESS.EXE
 * program, and decompressed with Microsoft's EXPAND.EXE program. The file,
 * when compressed, usually has an underscore as the last character in its
 * filename, ie HELLO.EX_ or VBRUN666.DL_
 *
 * File header format:
 * 4 bytes : "SZDD" (0x53,0x5A,0x44,0x44) - magic ID
 * 4 bytes : 0x88,0xF0,0x27,0x33 - more magic ID
 * 1 byte  : compression mode - only 'A' (0x41) known
 * 1 byte  : the character missing from the end of the filename (0=unknown)
 * 4 bytes : A,B,C,D. unpacked size of file is (D<<24)|(C<<16)|(B<<8)|(A)
 * n bytes : the compressed data
 *
 * The compressed data format is basically LZSS
 * - one control byte, whose bits dictate the format of the next 8 elements,
 *   in the order of LSB to MSB:
 *   - if a bit is set, the next element is a single byte literal
 *   - otherwise, the next element is a repeat: two bytes, %AAAAAAAA %BBBBCCCC
 *     where offset = %BBBBAAAAAAAA and length is %CCCC + 3
 * - this repeats until enough data has been unpacked
 *
 * A 4096 byte wraparound buffer must be kept by the decruncher. A wraparound
 * pointer starts at 4096-16 and each outputted byte must be written at that
 * position and the pointer advanced by one. The offsets of the compressed
 * data refer to an absolute position in this buffer, which may be before the
 * current pointer, or after it. The buffer's initial state is that all
 * elements are the space character (0x20).
 *
 * Also, it seems that there can often be superfluous data at the end of a
 * file - I can only go by the uncompressed size, so I can only really test
 * running out of source data. Some files end in the middle of a run, so I
 * can't test if they exactly reach the endpoint and no more.
 *
 * This code was based on code found in a document describing the Windows Help
 * file format, written by M. Winterhoff <100326.2776@compuserve.com>.
 *
 * C++ class by Ilkka Prusi (ilkka.prusi@gmail.com)
 * for XPK/XFD library implementation (2011).
 */


#ifndef SZDDEXPAND_H
#define SZDDEXPAND_H

#include <stdint.h>

#include <string>
#include <exception>

// exception-class for error cases
class SzddException : public std::exception
{
public:
	SzddException(void)
		: std::exception()
	{
	}

	SzddException(const char *szMessage)
		: std::exception(szMessage)
	{
	}
};

// metadata at file header
struct tSzddHeader
{
	uint32_t m_id;
	uint32_t m_out_len;

	tSzddHeader()
	{
		m_id = 0;
		m_out_len = 0;
	}

	bool IsSupportedFiletype(const uint8_t *in)
	{
		// "LPAK" might be supported also..?
		
	    if (in[0] == 'S' && in[1] == 'Z' && in[2] == 'D' && in[3]=='D') 
	    {
			// keep ID..
			m_id = (in[0x00] << 24) | (in[0x01] << 16) | (in[0x02] << 8) | in[0x03];
			return true;
		}
		return false;
	}
	
	// must be 14 bytes or more (caller must check)
	void setValues(const uint8_t *in)
	{
		m_out_len = in[10] | (in[11]<<8) | (in[12]<<16) | (in[13]<<24);
	}
};


// TODO: inherit from common "decruncher" or move to library..?
class CSzddExpand
{
protected:
	// buffer descriptor for in/out buffers
	struct tBuffer 
	{
		tBuffer()
		{
			ptr = NULL;
			size = 0;
	
			pos = NULL;
			pos_end = NULL;
		};
	
		uint8_t *ptr; // buffer of data
		uint32_t size; // size of buffer
	
		// position pointers for decompression
		uint8_t *pos;
		uint8_t *pos_end;
	};

	// buffer descriptors for input and output buffers
	struct tBuffer m_in_list;
	struct tBuffer m_out_list;

	// metadata at start of file (header)
	struct tSzddHeader m_HeaderMetadata;

	// actual decompressing
	bool Expand();

	void LoadBuffer(const uint8_t *pData, const uint32_t nSize);

	void Clean()
	{
		if (m_in_list.ptr) 
		{
			free(m_in_list.ptr);
			m_in_list.ptr = 0;
		}
		if (m_out_list.ptr) 
		{
			free(m_out_list.ptr);
			m_out_list.ptr = 0;
		}
	}

public:
    CSzddExpand()
		: m_HeaderMetadata()
		, m_in_list()
		, m_out_list()
    {}
	~CSzddExpand()
	{
		Clean();
	}

	// allow unpacking already read file from a buffer
	bool UnpackBuffer(const uint8_t *pData, const uint32_t nSize)
	{
		LoadBuffer(pData, nSize);
		return Expand();
	}

	// access unpacked data
	uint8_t *GetUnpackedData()
	{
		return m_out_list.ptr;
	}
	uint32_t GetUnpackedSize()
	{
		return m_out_list.size;
	}
};

#endif // SZDDEXPAND_H
