////////////////////////
//
// CUnDMS : DMS disk image unpacking/extraction
//
// This should have possibility to read/write to/from file/buffer,
// at first only to/from file maybe..
// 
//

#ifndef UNDMS_H
#define UNDMS_H

// use standard typedefs when possible
#include <stdint.h>

#include <string>
#include <vector>

// use wrapper(s) from parent-library
#include "AnsiFile.h"

// these were just hard-coded before..
enum PackMode
{
	PACK_STORE = 0,
	PACK_RLE = 1,
	PACK_QUICK = 2,
	PACK_MEDIUM = 3,
	PACK_DEEP = 4,
	PACK_HEAVY1 = 5,
	PACK_HEAVY2 = 6
	
};

const size_t BUFFERSIZE = 16384; // medium&deep packing modes

class CUnDMS
{
private:

	std::string m_sourceFile; // temp, see changes planned to parent-library
	std::string m_destFile; // temp, see changes planned to parent-library
	size_t m_nFileSize; // filesize of archive in bytes

	CReadBuffer m_ReadBuffer;
	CReadBuffer m_DecrunchBuffer;
	
protected:
	// TODO: use enum-types instead..
	bool decrunch(const int32_t pack_mode, uint8_t *pack_buffer);

public:
    CUnDMS()
		: m_sourceFile()
		, m_destFile()
		, m_nFileSize(0)
		, m_ReadBuffer(BUFFERSIZE) 
		, m_DecrunchBuffer(BUFFERSIZE) 
	{}
    ~CUnDMS()
	{}
	
    bool isSupported(const uint8_t *buffer) const
    {
		if((buffer[0] == 68) && (buffer[1] == 77) && /* DMS! */
			(buffer[2] == 83) && (buffer[3] == 33))
		{
			return true;
		}
		return false;
    }

	// TODO: allow to/from buffer/file unpacking..
	// implement necessary details..
	// better way to pass IOcontext from master to here
	// as planned, just need to determine interface..

	// file as source
	void setSourceFile(const std::string &szDms)
	{
		m_sourceFile = szDms;
	}
	// file as dest
	void setDestFile(const std::string &szDms)
	{
		m_destFile = szDms;
	}
	
	/*
	// TODO:
	// client-buffer as source..
	void setSourceBuffer(CReadBuffer *pIn)
	{}
	// client-buffer as dest..
	void setDestBuffer(CReadBuffer *pOut)
	{}
	*/

	bool unpack();
	
};

#endif // UNDMS_H
