#ifndef UNZAP_H
#define UNZAP_H

// use standard typedefs when possible
#include <stdint.h>

#include <string>
#include <vector>

// use wrapper(s) from parent-library
#include "AnsiFile.h"

class CUnZAP
{
private:

	std::string m_sourceFile; // temp, see changes planned to parent-library
	std::string m_destFile; // temp, see changes planned to parent-library
	size_t m_nFileSize; // filesize of archive in bytes

	CReadBuffer m_ReadBuffer;
	CReadBuffer m_DecrunchBuffer;
	
public:
    CUnZAP()
		: m_sourceFile()
		, m_destFile()
		, m_nFileSize(0)
		, m_ReadBuffer() 
		, m_DecrunchBuffer() 
	{}
	~CUnZAP()
	{}
    
    bool isSupported(CReadBuffer *buf) const
    {
		uint8_t *buffer = buf->GetAt(4);
		if (::memcmp(buffer , "ZAP V1.41", 9) == 0)
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

#endif // UNZAP_H
