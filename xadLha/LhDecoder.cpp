///////////////////////////////////////////
//
// CLhDecoder : extract&decode file(s) from LhA-archive file to disk,
// interface to actual Huffman decoding of specific packing methods.
//
// See notes in header file
// 
// Ilkka Prusi 2011
//

#include "LhDecoder.h"

////////////////////// decoder base


void CLhDecoder::Decode()
{
	unsigned int c = DecodeC();
	if (c < 256) 
	{
		m_dtext[m_loc++] = c;
		if (m_loc == m_dicsiz) 
		{
			m_uiCrc = m_crcio.calccrc(m_uiCrc, m_dtext, m_dicsiz);
			GetWriteBuf()->Append(m_dtext, m_dicsiz);
			m_loc = 0;
		}
		m_decode_count++;
	}
	else
	{
		int iMatchLen = c - m_adjust;
		unsigned int uiMatchOff = DecodeP() + 1; // may modify loc?
		unsigned int matchpos = (m_loc - uiMatchOff) & m_dicsiz_1;
		
		m_decode_count += iMatchLen;
		for (unsigned int i = 0; i < iMatchLen; i++) 
		{
			c = m_dtext[(matchpos + i) & m_dicsiz_1];
			m_dtext[m_loc++] = c;
			if (m_loc == m_dicsiz) 
			{
				m_uiCrc = m_crcio.calccrc(m_uiCrc, m_dtext, m_dicsiz);
				GetWriteBuf()->Append(m_dtext, m_dicsiz);
				m_loc = 0;
			}
		}
	}
}


////////////////////// decoders

// (note: -lh0-, -lhd- and -lz4- are "store only", no compression)

// -lh1-
// (dynamic huffman, shuffle)
void CLhDecodeLh1::DecodeStart()
{
	// specific for this
	CShuffleHuffman::decode_start_fix();
}

unsigned short CLhDecodeLh1::DecodeC()
{
	return CDynamicHuffman::decode_c_dyn();
}

unsigned short CLhDecodeLh1::DecodeP()
{
	return CShuffleHuffman::decode_p_st0();
}

// -lh2-
// (dynamic huffman)
void CLhDecodeLh2::DecodeStart()
{
	// specific for this
	CDynamicHuffman::decode_start_dyn(m_enBit);
}

unsigned short CLhDecodeLh2::DecodeC()
{
	return CDynamicHuffman::decode_c_dyn();
}

unsigned short CLhDecodeLh2::DecodeP()
{
	return CDynamicHuffman::decode_p_dyn(m_decode_count);
}

// -lh3-
// (static huffman routine 0, shuffle)
void CLhDecodeLh3::DecodeStart()
{
	CShuffleHuffman::decode_start_st0();
}

unsigned short CLhDecodeLh3::DecodeC()
{
	return CShuffleHuffman::decode_c_st0();
}

unsigned short CLhDecodeLh3::DecodeP()
{
	return CShuffleHuffman::decode_p_st0();
}

// -lh4- .. -lh7- (same for each)
// (static huffman routine 1)
void CLhDecodeLh7::DecodeStart()
{
	CStaticHuffman::decode_start_st1(m_enBit);
}

unsigned short CLhDecodeLh7::DecodeC()
{
	return CStaticHuffman::decode_c_st1();
}

unsigned short CLhDecodeLh7::DecodeP()
{
	return CStaticHuffman::decode_p_st1();
}


// -lzs-

void CLhDecodeLzs::DecodeStart()
{
	//decode_start_lzs
	m_loc = 0;
    m_BitIo.init_getbits();
}

unsigned short CLhDecodeLzs::DecodeC()
{
	//decode_c_lzs
    if (m_BitIo.getbits(1)) 
	{
        return m_BitIo.getbits(8);
    }
    else 
	{
        m_matchpos = m_BitIo.getbits(11);
        return m_BitIo.getbits(4) + 0x100;
    }
}

unsigned short CLhDecodeLzs::DecodeP()
{
	//decode_p_lzs
    return (m_loc - m_matchpos - LZMAGIC0) & 0x7ff;
}

// -lz5-

void CLhDecodeLz5::DecodeStart()
{
	//decode_start_lz5
	m_loc = 0;
    flagcnt = 0;
    m_BitIo.init_getbits();
}

unsigned short CLhDecodeLz5::DecodeC()
{
	//decode_c_lz5
    int c = 0;
    if (flagcnt == 0) 
	{
        flagcnt = 8;
		flag = m_BitIo.m_pReadBuf->GetNextByte();
    }
	
    flagcnt--;
	
	c = m_BitIo.m_pReadBuf->GetNextByte();
    if ((flag & 1) == 0) 
	{
        m_matchpos = c;

		c = m_BitIo.m_pReadBuf->GetNextByte();
		
        m_matchpos += (c & 0xf0) << 4;
        c &= 0x0f;
        c += 0x100;
    }
	
    flag >>= 1;
    return c;
}

unsigned short CLhDecodeLz5::DecodeP()
{
	//decode_p_lz5
    return (m_loc - m_matchpos - LZMAGIC5) & 0xfff;
}

