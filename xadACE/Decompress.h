/* ------------------------------------------------------------------------ */
/*  ML - 01/2004: changed licence to GPL                                    */
/* ------------------------------------------------------------------------ */  
/*
 * Modified for C++ library version by Ilkka Prusi <ilkka.prusi@gmail.com>
*/

#ifndef DECOMPRESS_H
#define DECOMPRESS_H

// use ISO typedefs
#include <stdint.h>

// use wrapper(s) from parent-library
#include "AnsiFile.h"


class CDecompress
{
protected:

	//--------- (de-)compressor constants
	const int32_t maxdic		= 22;
	const int32_t maxwd_mn		= 11;
	const int32_t maxwd_lg		= 11;
	const int32_t maxwd_svwd	= 7;
	const int32_t maxlength		= 259;
	const int32_t maxdis2		= 255;
	const int32_t maxdis3		= 8191;
	const int32_t maxcode		= (255+4+maxdic);
	const int32_t svwd_cnt		=  15;
	const int32_t max_cd_mn		= (256+4+(maxdic+1)-1);
	const int32_t max_cd_lg		= (256-1);

	//--------- flags
	enum Flags
	{
		ERR_MEM      = 1,
		ERR_FILES    = 2,
		ERR_FOUND    = 3,
		ERR_FULL     = 4,
		ERR_OPEN     = 5,
		ERR_READ     = 6,
		ERR_WRITE    = 7,
		ERR_CLINE    = 8,
		ERR_CRC      = 9,
		ERR_OTHER    = 10,
		ERR_USER     = 255
	};

	//-------- decompressor variables
	int16_t		rpos          ;
	int16_t		dcpr_do       ;
	int16_t		dcpr_do_max   ;
	int16_t		blocksize     ;
	int16_t		dcpr_dic      ;
	int16_t		dcpr_oldnum   ;
	int16_t		bits_rd       ;
	int16_t		dcpr_frst_file;
	uint16_t	dcpr_code_mn[1 << maxwd_mn];
	uint16_t	dcpr_code_lg[1 << maxwd_lg];
	uint8_t		dcpr_wd_mn[maxcode + 2];
	uint8_t		dcpr_wd_lg[maxcode + 2];
	uint8_t		wd_svwd[svwd_cnt];
	uint32_t	dcpr_dpos      ;
	uint32_t	cpr_dpos2      ;
	uint32_t	dcpr_dicsiz    ;
	uint32_t	dcpr_dican     ;
	uint32_t	dcpr_size      ;
	uint32_t	dcpr_olddist[4];
	uint32_t	code_rd        ;
	int8_t		*dcpr_text     ;

	//-------- quicksort
	uint16_t sort_org[maxcode + 2];
	uint8_t sort_freq[(maxcode + 2) * 2];

	// exchange (swap) values, 8-bit
	inline void xchg_def(uint8_t &v1, uint8_t &v2) const
	{
		uint8_t dummy = v1;
					v1=v2;
					v2=dummy;
	}
	// exchange (swap) values, 16-bit
	inline void xchg_def(uint16_t &v1, uint16_t &v2) const
	{
		uint16_t dummy = v1;
					v1=v2;
					v2=dummy;
	}

	inline int32_t comm_cpr_hf(const int16_t a, const int16_t b) const
	{
		return (a+b);
	}

	int32_t calc_dectabs(void);
	void dcpr_comm(int32_t comm_size);
	int32_t read_wd(uint32_t maxwd, uint16_t *code, uint8_t *wd, int32_t max_el);
	int32_t dcpr_adds_blk(int8_t *buf, uint32_t len);
	void dcpr_init_file(void);

	void dcpr_init(void);

	// given by parent (shared for IO)
	CReadBuffer *m_pDecrunchBuffer;
	
public:
    CDecompress(CReadBuffer *pDecrunchBuffer);
};

#endif // DECOMPRESS_H
