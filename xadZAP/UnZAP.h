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

	CReadBuffer m_DecrunchBuffer;

	// TODO: as ptr for parent-instances instead?
	CReadBuffer *m_pInputBuffer;
	CReadBuffer *m_pOutputBuffer;
	
public:
    CUnZAP(CReadBuffer *pIn, CReadBuffer *pOut)
		: m_pInputBuffer(pIn)
		, m_pOutputBuffer(pOut)
		, m_DecrunchBuffer() 
	{}
	~CUnZAP()
	{}
    
    bool isSupported(CReadBuffer *buf) const
    {
		uint8_t *buffer = buf->GetAt(4);
		if (::memcmp(buffer, "ZAP V1.41", 9) == 0)
		{
			return true;
		}
		return false;
    }
    
	bool unpack();
};

#endif // UNZAP_H
