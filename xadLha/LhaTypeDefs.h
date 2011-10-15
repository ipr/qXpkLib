#ifndef LHATYPEDEFS_H
#define LHATYPEDEFS_H


// all compression methods
// define -> enum by IPr
/* Added N.Watazaki ..V */
enum tCompressionMethod
{
	LZHUFF0_METHOD_NUM  = 0,
	LZHUFF1_METHOD_NUM  = 1,
	LZHUFF2_METHOD_NUM  = 2,
	LZHUFF3_METHOD_NUM  = 3,
	LZHUFF4_METHOD_NUM  = 4,
	LZHUFF5_METHOD_NUM  = 5,
	LZHUFF6_METHOD_NUM  = 6,
	LZHUFF7_METHOD_NUM  = 7,
	LARC_METHOD_NUM     = 8,
	LARC5_METHOD_NUM    = 9,
	LARC4_METHOD_NUM    = 10,
	LZHDIRS_METHOD_NUM  = 11,
	
	LZ_UNKNOWN  = 99 // unknown/unsupported method
};
/* Added N.Watazaki ..^ */

// huffman encoding dictionary bits
//
enum tHuffBits
{
	LZHUFF0_DICBIT          = 0,      /* no compress */
	LZHUFF1_DICBIT          = 12,      /* 2^12 =  4KB sliding dictionary */
	LZHUFF2_DICBIT          = 13,      /* 2^13 =  8KB sliding dictionary */
	LZHUFF3_DICBIT          = 13,      /* 2^13 =  8KB sliding dictionary */
	LZHUFF4_DICBIT          = 12,      /* 2^12 =  4KB sliding dictionary */
	LZHUFF5_DICBIT          = 13,      /* 2^13 =  8KB sliding dictionary */
	LZHUFF6_DICBIT          = 15,      /* 2^15 = 32KB sliding dictionary */
	LZHUFF7_DICBIT          = 16,      /* 2^16 = 64KB sliding dictionary */
	LARC_DICBIT             = 11,      /* 2^11 =  2KB sliding dictionary */
	LARC5_DICBIT            = 12,      /* 2^12 =  4KB sliding dictionary */
	LARC4_DICBIT            = 0,      /* no compress */
	
	MAX_DICBIT          = LZHUFF7_DICBIT,      /* lh7 use 16bits */
	MAX_DICSIZ          = (1L << MAX_DICBIT)
};

// -lzs- and -lz5- decoders
enum tLzDecoderMagicNumbers
{
	LZMAGIC0     = 18,
	LZMAGIC5     = 19
};


#endif // LHATYPEDEFS_H
