/*
*  unzoo.c by Martin Schoenert
*
*  @(#)$Id: unzoo.c,v 4.4 2000/05/29 08:56:57 sal Exp $
*
*  This file is in the Public Domain.
*
* Modified for use in C++ library by Ilkka Prusi <ilkka.prusi@gmail.com>
*
*/
/****************************************************************************
**
*V  Crc . . . . . . . . . . . . . . . . current cyclic redundancy check value
*F  CRC_BYTE(<crc>,<byte>)  . . . . . cyclic redundancy check value of a byte
*F  InitCrc() . . . . . . . . . . . . initialize cylic redundancy check table
**
**  'Crc'  is used by  the  decoding  functions to  communicate  the computed
**  CRC-16 value to the calling function.
**
**  'CRC_BYTE' returns the new value that one gets by updating the old CRC-16
**  value <crc> with the additional byte  <byte>.  It is  used to compute the
**  ANSI CRC-16 value for  each member of the archive.   They idea is that if
**  not  too many bits  of a member have corrupted,  then  the CRC-16 will be
**  different, and so the corruption can be detected.
**
**  'InitCrc' initialize the table that 'CRC_BYTE' uses.   You must call this
**  before using 'CRC_BYTE'.
**
**  The  ANSI CRC-16  value  for a sequence of    bits of lenght  <length> is
**  computed by shifting the bits through the following shift register (where
**  'O' are the latches and '+' denotes logical xor)
**
**                  bit          bit            ...  bit   bit   bit   -->-
**                     <length>     <length>-1          3     2     1     |
**                                                                        V
**      -<-------<---------------------------------------------------<----+
**      |       |                                                   |     ^
**      V       V                                                   V     |
**      ->O-->O-+>O-->O-->O-->O-->O-->O-->O-->O-->O-->O-->O-->O-->O-+>O-->-
**       MSB                                                         LSB
**
**  Mathematically we compute in the polynomial ring $GF(2)[x]$ the remainder
**
**      $$\sum_{i=1}^{i=length}{bit_i x^{length+16-i}} mod crcpol$$
**
**  where  $crcpol = x^{16}  + x^{15}  +  x^2 +  1$.  Then  the  CRC-16 value
**  consists  of the  coefficients   of  the remainder,  with    the constant
**  coefficient being  the most significant bit (MSB)  and the coefficient of
**  $x^{15}$ the least significant bit (LSB).
**
**  Changing  a  single bit will  always cause  the  CRC-16  value to change,
**  because $x^{i} mod crcpol$ is never zero.
**
**  Changing two  bits  will cause the CRC-16   value to change,  unless  the
**  distance between the bits is a multiple  of 32767, which  is the order of
**  $x$ modulo $crcpol = (x+1)(x^{15} + x + 1)$ ($x^{15}+x+1$ is primitive).
**
**  Changing  16 adjacent  bits will always  cause the  CRC value  to change,
**  because $x^{16}$ and $crcpol$ are relatively prime.
**
**  David Schwaderer provided the CRC-16 calculation in PC Tech Journal 4/85.
*/

#ifndef CRCSUM_H
#define CRCSUM_H

class crcsum
{
public:

	unsigned long m_Crc; // 16-bit should suffice..

	unsigned long m_CrcTab[256];

	inline unsigned long CRC_BYTE(unsigned int crc, unsigned int byte)
	{
		return ((crc >> 8) ^ m_CrcTab[ ((crc)^(byte))&0xff ]);
	}

	void InitCrc()
	{
		for ( unsigned long i = 0; i < 256; i++ ) 
		{
			m_CrcTab[i] = i;
			for ( unsigned long k = 0; k < 8; k++ )
			{
				m_CrcTab[i] = (m_CrcTab[i] >> 1) ^ ((m_CrcTab[i] & 1) ? 0xa001 : 0);
			}
		}
	}

public:
    crcsum()
		: m_Crc(0)
    {
		InitCrc();
    }
    
    void ClearCrc() // just to detect places
    {
		m_Crc = 0;
    }
    unsigned long getCrc()
    {
		return m_Crc;
    }
    unsigned long UpdateCrc(char *buf, unsigned long count)
    {
        for ( i = 0; i < count; i++ )
        {
            m_Crc = CRC_BYTE( m_Crc, buf[i] );
        }
        return m_Crc;
    }
};

#endif // CRCSUM_H
