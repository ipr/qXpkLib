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

	std::string m_fileName;
	size_t m_nFileSize; // filesize of archive in bytes

	CReadBuffer m_ReadBuffer;
	CReadBuffer m_DecrunchBuffer;
	
public:
    CUnZoom()
		: m_fileName()
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
};

#endif // UNZOOM_H
