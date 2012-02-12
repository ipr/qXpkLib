///////////////////////////////////////////
//
// CLhDecoder : extract&decode file(s) from LhA-archive file to disk,
// interface to actual Huffman decoding of specific packing methods.
//
// Ilkka Prusi 2011
//
//
//////////// note on actual decoding implementations:
// three "stages":
// - start (initialize)
// - decode C (multiple "chunks")
// - decode P ("finalize")
//
// some per-stage handling are specific to some packing-method
// and others are shared: 
//
// CLhDecoder-derived classes are interfaces to packing methods
// so file-extraction does not need to (and should not know)
// which method actually is being used.
//
///////// stages by packing method
//// decode start
// -lh1- and -lh2- have specific cases
// -lh3-, static huffman (0)
// -lh4- .. -lh7-, static huffman (1)
//// decode C
// -lh1- and -lh2-, dynamic huffman
// -lh3-, static huffman (0)
// -lh4- .. -lh7-, static huffman (1)
//// decode P
// -lh2-, dynamic huffman
// -lh1- and -lh3-, static huffman (0)
// -lh4- .. -lh7-, static huffman (1)
//


#ifndef LHDECODER_H
#define LHDECODER_H

#include <stdint.h>
#include <limits.h>

#include <QObject>
#include <QString>
#include <QMap>

#include "AnsiFile.h"
#include "LhaTypeDefs.h"
#include "LhHeader.h"
#include "crcio.h"
#include "Huffman.h"


//////// decoder base

// interface/base class for decoding methods,
// must be abstract
//
class CLhDecoder
{
protected:
	
	CCrcIo m_crcio;
	unsigned int m_uiCrc;

	size_t m_decode_count;
	
	// buffer for dictionary text
    CIOBuffer m_DictionaryText;
	
	// dictionary-related
	unsigned long m_dicsiz;
	uint8_t *m_dtext;
	unsigned int m_dicsiz_1;
	unsigned int m_adjust;
	tHuffBits m_enBit;

	// was global.. only actually used with -lzs- and -lz5-
    unsigned long m_loc;
	
public:
	CLhDecoder(void)
		: m_crcio()
		, m_uiCrc(0)
	    , m_decode_count(0)
		, m_DictionaryText(1024)
		, m_dicsiz(0)
		, m_dtext(nullptr)
		, m_dicsiz_1(0)
		, m_adjust(0)
		, m_loc(0)
		, m_enBit(LARC5_DICBIT) // default for backwards compatibility
	{}
	virtual ~CLhDecoder(void)
	{}

	// called before reusing
	void InitClear()
	{
		m_uiCrc = 0;
		m_decode_count = 0;
		
		m_dicsiz = 0;
		m_dtext = nullptr; // only helper-pointer, don't delete (see buffer m_DictionaryText)
		m_dicsiz_1 = 0;
		m_adjust = 0;
		m_loc = 0;
		m_enBit = LARC5_DICBIT;
	}
	
	void InitDictionary(const tCompressionMethod enCompression, const tHuffBits enHuffBit)
	{
		m_enBit = enHuffBit;
		
		m_dicsiz = (1L << (int)m_enBit); // 
		
		// verify allocation sufficient, zeroing entire buffer
		m_DictionaryText.PrepareBuffer(m_dicsiz, false); // can remain larger, doesn't matter
		
		m_dtext = m_DictionaryText.GetBegin();

		// clear dictionary
		//memset(m_dtext, 0, m_dicsiz); // for broken archive only? why?
		memset(m_dtext, ' ', m_dicsiz);
		
		m_dicsiz_1 = m_dicsiz - 1; // why is this separate?
		
		m_adjust = 256 - THRESHOLD;
		if (enCompression == LARC_METHOD_NUM)
		{
			m_adjust = 256 - 2;
		}
	}
	
	// need to implement in inherited (bitio by inheritance..)
    virtual void InitBitIo(const size_t nPackedSize, const size_t nOriginalSize, CIOBuffer *pReadBuf, CIOBuffer *pWriteBuf)
	{
		// call implementation in inherited class
		BitIo *pIo = GetBitIo();
		
		// set to inherited bitio-handler
		pIo->compsize = nPackedSize;
		pIo->origsize = nOriginalSize;
		pIo->m_pReadBuf = pReadBuf;
		pIo->m_pWriteBuf = pWriteBuf;
	
		// use this here, remove elsewhere..
		//pIo->init_getbits();
	}
	
	// may be used during decoding
    virtual CIOBuffer *GetReadBuf() = 0;
    virtual CIOBuffer *GetWriteBuf() = 0;
	virtual BitIo *GetBitIo() = 0;
	
	virtual void DecodeStart() = 0; // keep public
	virtual unsigned short DecodeC() = 0; // could be protected..
	virtual unsigned short DecodeP() = 0; // could be protected..

	virtual void DecodeFinish() // keep public
	{
		if (m_loc != 0) 
		{
			m_uiCrc = m_crcio.calccrc(m_uiCrc, m_dtext, m_loc);
			GetWriteBuf()->Append(m_dtext, m_loc);
		}
	}

	unsigned int GetCrc() const
	{
		return m_uiCrc;
	}
	
	size_t GetDecodeCount() const
	{
		return m_decode_count;
	}
	
	// call in loop to decode in chunks,
	// this will call actual C/P decode methods in derived
	virtual void Decode();
};

//////// decoders

// (note: -lh0-, -lhd- and -lz4- are "store only", no compression)

// -lh1-
// (dynamic huffman)
class CLhDecodeLh1 : public CLhDecoder, protected CShuffleHuffman
{
public:
	CLhDecodeLh1(void)
		: CLhDecoder()
		, CShuffleHuffman()
	{}
	virtual ~CLhDecodeLh1(void)
	{}

    virtual CIOBuffer *GetReadBuf()
	{
		return m_BitIo.m_pReadBuf;
	}
    virtual CIOBuffer *GetWriteBuf()
	{
		return m_BitIo.m_pWriteBuf;
	}
	virtual BitIo *GetBitIo()
	{
		return &m_BitIo;
	}
	
	virtual void DecodeStart();
	virtual unsigned short DecodeC();
	virtual unsigned short DecodeP();

};

// -lh2-
// (dynamic huffman)
class CLhDecodeLh2 : public CLhDecoder, protected CDynamicHuffman
{
public:
	CLhDecodeLh2(void)
		: CLhDecoder()
		, CDynamicHuffman()
	{}
	virtual ~CLhDecodeLh2(void)
	{}

    virtual CIOBuffer *GetReadBuf()
	{
		return m_BitIo.m_pReadBuf;
	}
    virtual CIOBuffer *GetWriteBuf()
	{
		return m_BitIo.m_pWriteBuf;
	}
	virtual BitIo *GetBitIo()
	{
		return &m_BitIo;
	}
	
	virtual void DecodeStart();
	virtual unsigned short DecodeC();
	virtual unsigned short DecodeP();
	
};

// -lh3-
// (static huffman routine 0 -> shuffle)
class CLhDecodeLh3 : public CLhDecoder, protected CShuffleHuffman
{
public:
	CLhDecodeLh3(void)
		: CLhDecoder()
		, CShuffleHuffman()
	{}
	virtual ~CLhDecodeLh3(void)
	{}

    virtual CIOBuffer *GetReadBuf()
	{
		return m_BitIo.m_pReadBuf;
	}
    virtual CIOBuffer *GetWriteBuf()
	{
		return m_BitIo.m_pWriteBuf;
	}
	virtual BitIo *GetBitIo()
	{
		return &m_BitIo;
	}
	
	virtual void DecodeStart();
	virtual unsigned short DecodeC();
	virtual unsigned short DecodeP();
};

/*
// -lh4- .. -lh7- (check virtual methods)
class CLhDecodeLh4 : public CLhDecoder
// -lh4- .. -lh7- (check virtual methods)
class CLhDecodeLh5 : public CLhDecoder
// -lh4- .. -lh7- (check virtual methods)
class CLhDecodeLh6 : public CLhDecoder
*/

// -lh4- .. -lh7- (same for each)
// (static huffman routine 1)
class CLhDecodeLh7 : public CLhDecoder, protected CStaticHuffman
{
public:
	CLhDecodeLh7(void)
		: CLhDecoder()
		, CStaticHuffman()
	{}
	virtual ~CLhDecodeLh7(void)
	{}

    virtual CIOBuffer *GetReadBuf()
	{
		return m_BitIo.m_pReadBuf;
	}
    virtual CIOBuffer *GetWriteBuf()
	{
		return m_BitIo.m_pWriteBuf;
	}
	virtual BitIo *GetBitIo()
	{
		return &m_BitIo;
	}

	virtual void DecodeStart();
	virtual unsigned short DecodeC();
	virtual unsigned short DecodeP();
};

// -lzs-
class CLhDecodeLzs : public CLhDecoder
{
protected:
	int flag, flagcnt;
	int m_matchpos;
	
	BitIo m_BitIo;
	
public:
	CLhDecodeLzs(void)
		: CLhDecoder()
		, flag(0)
		, flagcnt(0)
		, m_matchpos(0)
		, m_BitIo()
	{}
	virtual ~CLhDecodeLzs(void)
	{}
	
    virtual CIOBuffer *GetReadBuf()
	{
		return m_BitIo.m_pReadBuf;
	}
    virtual CIOBuffer *GetWriteBuf()
	{
		return m_BitIo.m_pWriteBuf;
	}
	virtual BitIo *GetBitIo()
	{
		return &m_BitIo;
	}

	virtual void DecodeStart();
	virtual unsigned short DecodeC();
	virtual unsigned short DecodeP();
};

// -lz5-
class CLhDecodeLz5 : public CLhDecoder
{
protected:
	int flag, flagcnt;
	int m_matchpos;

	BitIo m_BitIo;
	
public:
	CLhDecodeLz5(void)
		: CLhDecoder()
		, flag(0)
		, flagcnt(0)
		, m_matchpos(0)
		, m_BitIo()
	{}
	virtual ~CLhDecodeLz5(void)
	{}
	
    virtual CIOBuffer *GetReadBuf()
	{
		return m_BitIo.m_pReadBuf;
	}
    virtual CIOBuffer *GetWriteBuf()
	{
		return m_BitIo.m_pWriteBuf;
	}
	virtual BitIo *GetBitIo()
	{
		return &m_BitIo;
	}
	
	virtual void DecodeStart();
	virtual unsigned short DecodeC();
	virtual unsigned short DecodeP();
};


#endif // LHDECODER_H
