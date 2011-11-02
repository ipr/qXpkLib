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

	CReadBuffer m_DecrunchBuffer;

	// TODO: as ptr for parent-instances instead?
	CReadBuffer *m_pInputBuffer;
	CReadBuffer *m_pOutputBuffer;
	
public:
    CUnZoom(CReadBuffer *pIn, CReadBuffer *pOut)
		: m_pInputBuffer(pIn)
		, m_pOutputBuffer(pOut)
		, m_DecrunchBuffer() 
	{}
	~CUnZoom()
	{}
    
    bool isSupported(CReadBuffer *buf) const
    {
		uint8_t *buffer = buf->GetBegin();
		if (buffer[0] == 'Z'
			&& buffer[1] == 'O'
			&& buffer[2] == 'M'
			&& buffer[3] == '5')
		{
			return true;
		}
		else if (buffer[0] == 'Z'
			&& buffer[1] == 'O'
			&& buffer[2] == 'O'
			&& buffer[3] == 'M')
		{
			return true;
		}
		return false;
    }

	bool unpack();
};

#endif // UNZOOM_H
