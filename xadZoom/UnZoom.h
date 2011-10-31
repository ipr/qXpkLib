#ifndef UNZOOM_H
#define UNZOOM_H

// use standard typedefs when possible
#include <stdint.h>

#include <string>
#include <vector>

// use wrapper(s) from parent-library
#include "AnsiFile.h"


class CUnZoom
{
private:

	std::string m_sourceFile; // temp, see changes planned to parent-library
	std::string m_destFile; // temp, see changes planned to parent-library
	size_t m_nFileSize; // filesize of archive in bytes

	CReadBuffer m_ReadBuffer;
	CReadBuffer m_DecrunchBuffer;
	
public:
    CUnZoom()
		: m_sourceFile()
		, m_destFile()
		, m_nFileSize(0)
		, m_ReadBuffer() 
		, m_DecrunchBuffer() 
	{}
	~CUnZoom()
	{}
    
    bool isSupported(CReadBuffer *buf) const
    {
		uint8_t *buffer = buf->GetBegin();
		if (buffer[0] == 'Z'
			&& buffer[1] == 'O'
			&& buffer[2] == 'O'
			&& buffer[3] == 'M')
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

#endif // UNZOOM_H
