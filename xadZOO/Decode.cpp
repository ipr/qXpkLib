/*
 * unzoo.c by Martin Schoenert
 *
 * Modified for use in C++ library by Ilkka Prusi <ilkka.prusi@gmail.com>
 * 
 * Reuses code from qLhaLib and xadLHA, which are based on Unix LHA,
 * Copyrights:
 * LHarc    for UNIX  V 1.02  Copyright(C) 1989  Y.Tagawa
 * LHx      for MSDOS V C2.01 Copyright(C) 1990  H.Yoshizaki
 * LHx(arc) for OSK   V 2.01  Modified     1990  Momozou
 * LHa      for UNIX  V 1.00  Copyright(C) 1992  Masaru Oki
 * LHa      for UNIX  V 1.14  Modified     1995  Nobutaka Watazaki
 * LHa      for UNIX  V 1.14i Modified     2000  Tsugio Okamoto
 *                    Autoconfiscated 2001-2004  Koji Arai
 * 
 * Modifications for use in C++ library by Ilkka Prusi (2011).
 * 
 */


#include "Decode.h"


/****************************************************************************
**
*F  DecodeLzh() . . . . . . . . . . . . . . . extract a LZH compressed member
**
**  'DecodeLzh'  decodes  a LZH  (Lempel-Ziv 77  with dynamic Huffman coding)
**  encoded member from the archive to the output file.
**
**  Each member is encoded as a  series of blocks.  Each  block starts with a
**  16  bit field that contains the  number of codes  in this block <number>.
**  The member is terminated by a block with 0 codes.
**
**  Next each block contains the  description of three Huffman codes,  called
**  pre code, literal/length code, and log code.  The purpose of the pre code
**  is to encode the description of  the literal/length code.  The purpose of
**  the literal/length code and the  log code is   to encode the  appropriate
**  fields in the LZ code.   I am too stupid to  understand the format of the
**  description.
**
**  Then   each block contains  <number>  codewords.  There  are two kinds of
**  codewords, *literals* and *copy instructions*.
**
**  A literal represents a certain byte.  For  the moment imaging the literal
**  as having 9 bits.   The first bit  is zero, the other  8 bits contain the
**  byte.
**
**      +--+----------------+
**      | 0|     <byte>     |
**      +--+----------------+
**
**  When a  literal is  encountered, the byte  <byte> that  it represents  is
**  appended to the output.
**
**  A copy  instruction represents a certain  sequence of bytes that appeared
**  already  earlier in the output.  The  copy instruction  consists of three
**  parts, the length, the offset logarithm, and the offset mantissa.
**
**      +--+----------------+--------+--------------------+
**      | 1|   <length>-3   |  <log> |     <mantissa>     |
**      +--+----------------+--------+--------------------+
**
**  <length>  is  the  length  of the sequence   which  this copy instruction
**  represents.  We store '<length>-3', because <length> is never 0, 1, or 2;
**  such sequences are better represented by 0, 1, or  2 literals.  <log> and
**  <mantissa>  together represent the offset at  which the sequence of bytes
**  already  appeared.  '<log>-1'  is  the number of   bits in the <mantissa>
**  field, and the offset is $2^{<log>-1} + <mantissa>$.  For example
**
**      +--+----------------+--------+----------+
**      | 1|        9       |    6   | 0 1 1 0 1|
**      +--+----------------+--------+----------+
**
**  represents the sequence of 12 bytes that appeared $2^5 + 8 + 4  + 1 = 45$
**  bytes earlier in the output (so those 18 bits of input represent 12 bytes
**  of output).
**
**  When a copy instruction  is encountered, the  sequence of  <length> bytes
**  that appeared   <offset> bytes earlier  in the  output  is again appended
**  (copied) to   the output.   For this  purpose  the last  <max>  bytes are
**  remembered,  where  <max>  is the   maximal  used offset.   In 'zoo' this
**  maximal offset is $2^{13} =  8192$.  The buffer in  which those bytes are
**  remembered is  called   a sliding  window for   reasons  that  should  be
**  obvious.
**
**  To save even  more space the first 9  bits of each code, which  represent
**  the type of code and either the literal value or  the length, are encoded
**  using  a Huffman code  called the literal/length  code.   Also the next 4
**  bits in  copy instructions, which represent  the logarithm of the offset,
**  are encoded using a second Huffman code called the log code.
**
**  Those  codes  are fixed, i.e.,  not  adaptive, but  may  vary between the
**  blocks, i.e., in each block  literals/lengths and logs  may be encoded by
**  different codes.  The codes are described at the beginning of each block.
**
**  Haruhiko Okumura  wrote the  LZH code (originally for his 'ar' archiver).
*/
//
// I think this should be replaced with code from Unix Lha instead
// since it handles better different compilers etc.
//
bool DecodeZoo::MakeTablLzh(const int nchar, const unsigned char *bitlen, const int tablebits, uint16_t *table)
{
    uint16_t      count[17], weight[17], start[18];
    unsigned int  i = 0;

    for (i = 1; i <= 16; i++) // why first remains random value..?
    {
		count[i] = 0;
	}
    for (i = 0; i < nchar; i++) 
    {
		count[bitlen[i]]++;
	}

    start[1] = 0;
    for (i = 1; i <= 16; i++)
    {
        start[i + 1] = start[i] + (count[i] << (16 - i));
    }
    
    //if (start[17] != (uint16_t)((unsigned) 1 << 16)) // <- not this way..
    if ((start[17] & 0xffff) != 0)
    {
		// corrupted file,
		// wrong offset used
		// or bug in code
		throw IOException("make table failed");
    }

    unsigned int jutbits = 16 - tablebits;
    for (i = 1; i <= tablebits; i++) 
    {
        start[i] >>= jutbits;
        weight[i] = (1 << (tablebits - i));
    }
    while (i <= 16) 
    {
        weight[i] = (1 << (16 - i));
        i++;
    }

    //i = start[tablebits + 1] >> jutbits;
    //if (i != (uint16_t)((unsigned) 1 << 16)) // <- 16-bit shift and cast to 16-bit value?
    int32_t start_m = start[tablebits + 1] >> jutbits;
    if (start_m != 0) // 0xffff ?
    {
        //while (i != k) 
        int32_t kEnd = 1 << tablebits;
        for (int i = start_m; i < kEnd; i++)
        {
	        //table[i++] = 0;
            table[i] = 0;
	    }
    }

    unsigned int avail = nchar;
    unsigned int mask = (1 << (15 - tablebits));
    for (unsigned int ch = 0; ch < nchar; ch++) 
    {
		uint32_t len = bitlen[ch];
        if (len == 0) 
        {
			continue;
		}
		
        if (len <= tablebits) 
        {
            for (uint32_t i = 0; i < weight[len]; i++ )  
            {
				table[i+start[len]] = ch;
			}
        }
        else 
        {
            uint32_t k = start[len];
            uint16_t *p = &table[k >> jutbits];
            uint32_t i = len - tablebits;
            
            // make_table_tree() ?
            while (i != 0) 
            {
                if (*p == 0) 
                {
                    TreeRight[avail] = TreeLeft[avail] = 0;
                    *p = avail++;
                }
                if (k & mask) 
                {
					p = &TreeRight[*p];
				}
                else
                {
					p = &TreeLeft[*p];
				}
                k <<= 1;  
                i--;
            }
            *p = ch;
        }
        start[len] += weight[len];
    }

    return true;
}

// prefer constexpr over preprocessor macros
//
const int32_t MAX_LIT         = 255;     /* maximal literal code            */
const int32_t MIN_LEN         = 3;       /* minimal length of match         */
const int32_t MAX_LEN         = 256;     /* maximal length of match         */
const int32_t MAX_CODE        = (MAX_LIT+1 + MAX_LEN+1 - MIN_LEN);
const int32_t BITS_CODE       = 9;       /* 2^BITS_CODE > MAX_CODE (+1?)    */
const int32_t MAX_OFF         = 8192;    /* 13 bit sliding directory        */
const int32_t MAX_LOG         = 13;      /* maximal log_2 of offset         */
const int32_t BITS_LOG        = 4;       /* 2^BITS_LOG > MAX_LOG (+1?)      */
const int32_t MAX_PRE         = 18;      /* maximal pre code                */
const int32_t BITS_PRE        = 5;       /* 2^BITS_PRE > MAX_PRE (+1?)      */

// use constants for buffer-sizes also where fixed-length
const int32_t c_TREE_SIZE = 2*MAX_CODE+1;
const int32_t c_TAB_CODE_SIZE = 4096;

uint16_t  TreeLeft [c_TREE_SIZE];        /* tree for codes   (upper half)   */
uint16_t  TreeRight[c_TREE_SIZE];        /* and  for offsets (lower half)   */
uint16_t TabCode  [c_TAB_CODE_SIZE];     /* table for fast lookup of codes  */
uint8_t   LenCode  [MAX_CODE+1];         /* number of bits used for code    */
uint16_t  TabLog   [256];                /* table for fast lookup of logs   */
uint8_t   LenLog   [MAX_LOG+1];          /* number of bits used for logs    */
uint16_t  TabPre   [256];                /* table for fast lookup of pres   */
uint8_t   LenPre   [MAX_PRE+1];          /* number of bits used for pres    */

// same stuff as in actual LhA/Lzh library,
// are there differences to worry about?
//
bool DecodeZoo::DecodeLzh(ZooEntry *pEntry, CAnsiFile &archive, CAnsiFile &outFile)
{
	size_t size = pEntry->compressed_size;
	
    unsigned long       cnt;            /* number of codes in block        */
    //unsigned long       cnt2;           /* number of stuff in pre code     */
    //unsigned long       code;           /* code from the Archive           */
    //unsigned long       len;            /* length of match                 */
    //unsigned long       log;            /* log_2 of offset of match        */
    //unsigned long       off;            /* offset of match                 */
    //unsigned long       pre;            /* pre code                        */
    //char *              cur;            /* current position in BufFile     */
    char *              pos;            /* position of match               */
    //char *              end;            /* pointer to the end of BufFile   */
    char *              stp;            /* stop pointer during copy        */
    //unsigned long       crc;            /* cyclic redundancy check value   */
    //unsigned long       i;              /* loop variable                   */
    //unsigned long       bits;           /* the bits we are looking at      */
    //unsigned long       bitc;           /* number of bits that are valid   */

    /* initialize bit source, output pointer, and crc                      */
    unsigned long bits = 0;  /* the bits we are looking at      */
    unsigned long bitc = 0;  /* number of bits that are valid   */
    FLSH_BITS(0, bits, bitc);
    
    const char *BufFile = (char*)m_DecrunchBuffer->GetBegin();
    char *cur = BufFile;
    char *end = cur + MAX_OFF;

    /* loop until all blocks have been read                                */
    cnt = PEEK_BITS( 16, bits, bitc );  
    FLSH_BITS( 16, bits, bitc );
    while ( cnt != 0 ) 
    {
        /* read the pre code                                               */
        uint32_t pc_count = PEEK_BITS( BITS_PRE, bits, bitc );  
        FLSH_BITS( BITS_PRE, bits, bitc );
        if ( pc_count == 0 ) 
        {
            uint32_t pre = PEEK_BITS( BITS_PRE, bits, bitc );  
            FLSH_BITS( BITS_PRE, bits, bitc );
            
            bufferSet(pre, TabPre, 256);
			
			// just memset() it to zero..
			::memset(LenPre, 0, MAX_PRE +1);
        }
        else 
        {
            uint32_t i = 0;
            while ( i < pc_count ) 
            {
                uint32_t len = PEEK_BITS( 3, bits, bitc );  
                FLSH_BITS( 3, bits, bitc );
                if ( len == 7 ) {
                    while ( PEEK_BITS( 1, bits, bitc ) ) 
                    { 
						len++; 
						FLSH_BITS( 1, bits, bitc ); 
					}
                    FLSH_BITS( 1, bits, bitc );
                }
                LenPre[i++] = len;
                if ( i == 3 ) 
                {
                    len = PEEK_BITS( 2, bits, bitc );  
                    FLSH_BITS( 2, bits, bitc );
                    while ( 0 < len-- )  
                    {
						LenPre[i++] = 0;
					}
                }
            }
            while ( i <= MAX_PRE )  
            {
				LenPre[i++] = 0;
			}
            if (MakeTablLzh( MAX_PRE+1, LenPre, 8, TabPre ) == false) 
            {
                throw IOException("pre code description corrupted");
            }
        }

        /* read the code (using the pre code)                              */
        pc_count = PEEK_BITS( BITS_CODE, bits, bitc );  
        FLSH_BITS( BITS_CODE, bits, bitc );
        if ( pc_count == 0 ) 
        {
            uint32_t code = PEEK_BITS( BITS_CODE, bits, bitc );  
            FLSH_BITS( BITS_CODE, bits, bitc );
            
            bufferSet(code, TabCode, 4096);
            ::memset(LenCode, 0, MAX_CODE +1);
        }
        else 
        {
            uint32_t i = 0;
            while ( i < pc_count ) 
            {
                uint32_t len = TabPre[ PEEK_BITS( 8, bits, bitc ) ];
                if ( len <= MAX_PRE ) 
                {
                    FLSH_BITS( LenPre[len], bits, bitc );
                }
                else 
                {
                    FLSH_BITS( 8, bits, bitc );
                    do 
                    {
                        if ( PEEK_BITS( 1, bits, bitc ) ) 
                        {
							len = TreeRight[len];
						}
                        else
                        {
							len = TreeLeft [len];
						}
                        FLSH_BITS( 1, bits, bitc );
                    } while ( MAX_PRE < len );
                }
                if ( len <= 2 ) 
                {
                    if ( len == 0 ) 
                    {
                        len = 1;
                    }
                    else if ( len == 1 ) 
                    {
                        len = PEEK_BITS(4, bits, bitc)+3;  
                        FLSH_BITS(4, bits, bitc);
                    }
                    else 
                    {
                        len = PEEK_BITS(BITS_CODE, bits, bitc)+20; 
                        FLSH_BITS(BITS_CODE, bits, bitc);
                    }
                    
                    while ( 0 < len-- )  
                    {
						LenCode[i++] = 0;
					}
                }
                else 
                {
                    LenCode[i++] = len - 2;
                }
            }
            while ( i <= MAX_CODE )  
            {
				LenCode[i++] = 0;
			}
            if (MakeTablLzh(MAX_CODE+1, LenCode, 12, TabCode) == false) 
            {
                throw IOException("literal/length code description corrupted");
            }
        }

        /* read the log_2 of offsets                                       */
        pc_count = PEEK_BITS( BITS_LOG, bits, bitc );  
        FLSH_BITS( BITS_LOG, bits, bitc );
        if ( pc_count == 0 ) 
        {
            uint32_t log = PEEK_BITS( BITS_LOG, bits, bitc );  
            FLSH_BITS( BITS_LOG, bits, bitc );
            
            bufferSet(log, TabLog, 256);
            ::memset(LenLog, 0, MAX_LOG +1);
        }
        else 
        {
            uint32_t i = 0;
            while ( i < pc_count ) 
            {
                uint32_t len = PEEK_BITS( 3, bits, bitc );  
                FLSH_BITS( 3, bits, bitc );
                if ( len == 7 ) 
                {
                    while ( PEEK_BITS( 1, bits, bitc ) ) 
                    { 
						len++; 
						FLSH_BITS( 1, bits, bitc ); 
					}
                    FLSH_BITS( 1, bits, bitc );
                }
                LenLog[i++] = len;
            }
            while ( i <= MAX_LOG )  
            {
				LenLog[i++] = 0;
			}
            if (MakeTablLzh( MAX_LOG+1, LenLog, 8, TabLog) == false) 
            {
                throw IOException("log code description corrupted");
            }
        }

        /* read the codes                                                  */
        while ( 0 < cnt-- ) 
        {
            /* try to decode the code the fast way                         */
            uint32_t code = TabCode[ PEEK_BITS( 12, bits, bitc ) ];

            /* if this code needs more than 12 bits look it up in the tree */
            if ( code <= MAX_CODE ) 
            {
                FLSH_BITS( LenCode[code], bits, bitc );
            }
            else 
            {
                FLSH_BITS( 12, bits, bitc );
                do 
                {
                    if ( PEEK_BITS( 1, bits, bitc ) )  
                    {
						code = TreeRight[code];
					}
                    else
                    {
						code = TreeLeft [code];
					}
                    FLSH_BITS( 1, bits, bitc );
                } while ( MAX_CODE < code );
            }

            if ( code <= MAX_LIT ) 
            {
	            /* if the code is a literal, stuff it into the buffer          */
	            
                *cur++ = code;
                m_crc.m_Crc = m_crc.CRC_BYTE(m_crc.m_Crc, code );
                if ( cur == end ) 
                {
				    if (outFile.Write(BufFile, cur-BufFile) == false)
                    {
		                throw IOException("cannot write output file");
                    }
                    cur = m_ReadBuffer->GetBegin();
                }
            }
            else 
            {
	            /* otherwise compute match length and offset and copy          */
                uint32_t len = code - (MAX_LIT+1) + MIN_LEN;

                /* try to decodes the log_2 of the offset the fast way     */
                uint32_t log = TabLog[ PEEK_BITS( 8, bits, bitc ) ];
                /* if this log_2 needs more than 8 bits look in the tree   */
                if ( log <= MAX_LOG ) 
                {
                    FLSH_BITS( LenLog[log], bits, bitc );
                }
                else 
                {
                    FLSH_BITS( 8, bits, bitc );
                    do {
                        if ( PEEK_BITS( 1, bits, bitc ) )  
                        {
							log = TreeRight[log];
						}
                        else
                        {
							log = TreeLeft [log];
						}
                        FLSH_BITS( 1, bits, bitc );
                    } while ( MAX_LOG < log );
                }

                /* compute the offset                                      */
                uint32_t off = 0;
                if ( log != 0 ) 
                {
                    off = ((unsigned)1 << (log-1)) + PEEK_BITS( log-1, bits, bitc );
                    FLSH_BITS( log-1, bits, bitc );
                }

                /* copy the match (this accounts for ~ 50% of the time)    */
                pos = BufFile + (((cur-BufFile) - off - 1) & (MAX_OFF - 1));
                if ( cur < end-len && pos < end-len ) 
                {
                    stp = cur + len;
                    do 
                    {
                        code = *pos++;
		                m_crc.m_Crc = m_crc.CRC_BYTE(m_crc.m_Crc, code );
                        *cur++ = code;
                    } while ( cur < stp );
                }
                else 
                {
                    while ( 0 < len-- ) 
                    {
                        code = *pos++;
		                m_crc.m_Crc = m_crc.CRC_BYTE(m_crc.m_Crc, code );
                        *cur++ = code;
                        if ( pos == end ) 
                        {
                            pos = BufFile;
                        }
                        if ( cur == end ) 
                        {
						    if (outFile.Write(BufFile, cur-BufFile) == false)
                            {
				                throw IOException("cannot write output file");
                            }
                            cur = BufFile;
                        }
                    }
                }
            }
        }

        cnt = PEEK_BITS( 16, bits, bitc );  
        FLSH_BITS( 16, bits, bitc );
    }

    /* write out the rest of the buffer                                    */
    if (outFile.Write(BufFile, cur-BufFile) == false)
    {
        throw IOException("cannot write output file");
    }
    return true;
}
