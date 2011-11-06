/////////////////////////////////
//
// This code is placed in the Public Domain by its author, Stuart Caie 
//
// C to C++ class changes by Ilkka Prusi, 2011
//
// Main difference is ability to use in GUI-apps (no console-output).
//


#ifndef IMPLODEREXPLODER_H
#define IMPLODEREXPLODER_H

#include <stdint.h>

#include <string>
#include <exception>

// exception-class for error cases
class ImplException : public std::exception
{
public:
	ImplException(void)
		: std::exception()
	{
	}

	ImplException(const char *szMessage)
		: std::exception(szMessage)
	{
	}
};


// metadata at file header
struct tImploderHeader
{
	uint32_t m_id;
	uint32_t m_out_len;
	uint32_t m_end_off;

	uint32_t m_comp_len; // <- filled in later..
	
	tImploderHeader()
	{
		m_id = 0;
		m_out_len = 0;
		m_end_off = 0;
		m_comp_len = 0;
	}

	// must be 12 bytes or more (caller must check)
	void setValues(uint8_t *in)
	{
		m_id = (in[0x00] << 24) | (in[0x01] << 16) | (in[0x02] << 8) | in[0x03];
		m_out_len = (in[0x04] << 24) | (in[0x05] << 16) | (in[0x06] << 8) | in[0x07];
		m_end_off = (in[0x08] << 24) | (in[0x09] << 16) | (in[0x0A] << 8) | in[0x0B];
	}
	
	// check file type ID if is supported file type
	bool IsSupportedFiletype()
	{
		/* 0x44494D50 == DImp == Disk Imploder */ 
		
		/* check for magic ID 'IMP!', or one of the other IDs used by Imploder 
		 clones; ATN!, BDPI, CHFI, Dupa, EDAM, FLT!, M.H., PARA and RDC9 */ 
		if (m_id != 0x494d5021 && m_id != 0x41544e21 && m_id != 0x42445049 && 
			m_id != 0x43484649 && m_id != 0x44757061 && m_id != 0x4544414d && 
			m_id != 0x464c5421 && m_id != 0x4d2e482e && m_id != 0x50415241 && 
			m_id != 0x52444339) 
		{
			return false;
		}
		return true;
	}
};

// TODO: inherit from common "decruncher" or move to library..?
class CImploderExploder
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
	struct tImploderHeader m_HeaderMetadata;
	
	//uint32_t compressed_size; // <- added to header metadata
	//uint32_t uncompressed_size; // <- already in header metadata

	void init_matchbase(uint32_t *match_base, const uint8_t *table) const
	{
		for (uint32_t x = 0; x < 8; x++) 
		{
			match_base[x] = (table[x*2] << 8) | table[x*2 + 1];
		}
	}
	
	//inline void Explode_Getbit(uchar &bit_buffer, uchar &bit, uchar &bit2);

	// TODO: use member buffers instead..	
	bool explode(uint8_t *buffer,
				uint8_t *table,
				uint32_t comp_len);
	
	
	// actual decompressing
	void DeplodeImploder();
	
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

	//CIOBuffer *m_pInput;
	//CIOBuffer *m_pOutput;
	
public:
	/*
	CImploderExploder(CIOBuffer *pInput)
		: m_pInput(pInput)
		*/
    CImploderExploder()
		: m_HeaderMetadata()
		, m_in_list()
		, m_out_list()
		//, compressed_size(0)
		//, uncompressed_size(0)
	{}
	~CImploderExploder()
	{
		Clean();
	}
	
	// allow unpacking already read file from a buffer
	bool UnpackBuffer(const uint8_t *pData, const uint32_t nSize)
	{
		LoadBuffer(pData, nSize);
		DeplodeImploder();
		return true;
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

#endif // IMPLODEREXPLODER_H
