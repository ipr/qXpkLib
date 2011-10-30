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

	std::string m_fileName;
	size_t m_nFileSize; // filesize of archive in bytes

	CReadBuffer m_ReadBuffer;
	CReadBuffer m_DecrunchBuffer;
	
public:
    CUnZAP()
		: m_fileName()
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
};

#endif // UNZAP_H
