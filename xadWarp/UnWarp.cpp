/*
	Amiga Warp disk-image unpacking code.

	Original by Dirk Stoecker:
	http://libxad.cvs.sourceforge.net/libxad/libxad/portable/clients/
	
	WinUAE version by Toni Wilen:
	https://github.com/tonioni/WinUAE/blob/master/archivers/wrp/
	
	Modifications to use in C++ library by Ilkka Prusi <ilkka.prusi@gmail.com>
*/

#include "UnWarp.h"


#include <time.h>

/* based on http://libxad.cvs.sourceforge.net/libxad/libxad/portable/clients/ by Dirk Stoecker */

#define XADERR_ILLEGALDATA 1
#define XADERR_DECRUNCH 2
#define XADERR_NOMEMORY 3

typedef uint32_t rledata;
/*
struct rledata 
{
	uint32_t rledatav;
};
*/

void putrle(uint8_t data, CReadBuffer *out, rledata &rled)
{
	int num = 0;

	uint32_t a = rled;
	if (a & 0x100) /* was RLE mode */
	{
		if (data == 0 || (data == 1 && (a & 0x80000000))) 
		{
			a = 0x90; 
			num = 1;
		} 
		else 
		{
			a &= 0xFF; 
			num = data - 1;
		}
	} 
	else if (data == 0x90) 
	{
		num = 0; a |= 0x100;
	} 
	else 
	{
		num = 1; a = data;
	}
	rled = a; // update back
	while (num--)
	{
		out->SetNextByte(a);
	}
}



uint32_t xadIOGetBitsLow(struct fout *io, uint8_t bits)
{
	uint32_t x;

	io->err = 0;
	while(io->xio_BitNum < bits)
	{
		int v = zfile_getc (io->zf);
		if (v == -1) {
			io->err = 1;
			return 0;
		}
		io->xio_BitBuf |= v << io->xio_BitNum;
		io->xio_BitNum += 8;
	}
	x = io->xio_BitBuf & ((1<<bits)-1);
	io->xio_BitBuf >>= bits;
	io->xio_BitNum -= bits;
	return x;
}

#define ARCSQSPEOF   256                /* special endfile token */
#define ARCSQNUMVALS 257                /* 256 data values plus SPEOF */

static int32_t ARCunsqueeze(struct zfile *in, struct zfile *out, struct rledata *rled)
{
	int32_t err = 0;
	int32_t i, numnodes;
	int16_t *node;
	struct fout io;

	io.zf = in;
	io.xio_BitBuf = 0;
	io.xio_BitNum = 0;
	io.err = 0;

	if((node = (int16_t *) xcalloc(int16_t, 2*ARCSQNUMVALS)))
	{
		numnodes = xadIOGetBitsLow(&io, 16);

		if(numnodes < 0 || numnodes >= ARCSQNUMVALS)
			err = XADERR_DECRUNCH;
		else
		{  /* initialize for possible empty tree (SPEOF only) */
			node[0] = node[1] = -(ARCSQSPEOF + 1);

			numnodes *= 2; i = 0;
			while(i < numnodes && !io.err)       /* get decoding tree from file */
			{
				node[i++] = xadIOGetBitsLow(&io, 16);
				node[i++] = xadIOGetBitsLow(&io, 16);
			}

			do
			{
				/* follow bit stream in tree to a leaf */
				i = 0;
				while(i >= 0 && !io.err)
					i = node[2*i + xadIOGetBitsLow(&io, 1)];

				i = -(i + 1); /* decode fake node index to original data value */

				if(i != ARCSQSPEOF)
					putrle (i, out, rled);
			} while(i != ARCSQSPEOF);
		}
		xfree(node);
	}
	else
		err = XADERR_NOMEMORY;

	return err;
}




#define UCOMPMAXCODE(n) (((uint32_t) 1 << (n)) - 1)
#define UCOMPBITS          16
#define UCOMPSTACKSIZE   8000
#define UCOMPFIRST        257           /* first free entry */
#define UCOMPCLEAR        256           /* table clear output code */
#define UCOMPINIT_BITS      9           /* initial number of bits/code */
#define UCOMPBIT_MASK    0x1f
#define UCOMPBLOCK_MASK  0x80

// is this really needed here..?
// same decompression already exists elsewhere..
//
struct UCompData 
{
	int16_t   clear_flg;
	uint16_t   n_bits;                 /* number of bits/code */
	uint16_t   maxbits;                /* user settable max # bits/code */
	uint32_t   maxcode;                /* maximum code, given n_bits */
	uint32_t   maxmaxcode;
	int32_t   free_ent;
	int32_t   offset;
	int32_t   size;
	uint16_t  *tab_prefixof;
	uint8_t   *tab_suffixof;
	uint8_t    stack[UCOMPSTACKSIZE];
	uint8_t    buf[UCOMPBITS];
	int	    insize;
	//struct    rledata *rled;
	rledata rled;
	
	// constructor
	UCompData()
	 : tab_prefixof(NULL)
	 , tab_suffixof(NULL)
	{
	}
	// destructor
	~UCompData()
	{
		if (tab_prefixof != NULL)
		{
			::free(tab_prefixof);
		}
		if (tab_suffixof != NULL)
		{
			::free(tab_suffixof);
		}
	}
	
	bool init()
	{
		tab_prefixof = ::malloc(sizeof(uint16_t)*maxmaxcode);
		if (tab_prefixof == NULL)
		{
			return false;
		}
		tab_suffixof = ::malloc(sizeof(uint8_t)*maxmaxcode);
		if (tab_suffixof == NULL)
		{
			return false;
		}
		/* Initialize the first 256 entries in the table. */
		for(code = 255; code >= 0; code--)
		{
			tab_suffixof[code] = code;
		}
		return true;
	}
};


/* Read one code from input. If EOF, return -1. */
static int32_t UCompgetcode(struct zfile *in, struct UCompData *cd)
{
	int32_t code, r_off, bits;
	uint8_t *bp = cd->buf;

	if(cd->clear_flg > 0 || cd->offset >= cd->size || cd->free_ent > cd->maxcode)
	{
		/*
		* If the next entry will be too big for the current code
		* size, then we must increase the size.  This implies reading
		* a new buffer full, too.
		*/
		if(cd->free_ent > cd->maxcode)
		{
			if(++cd->n_bits == cd->maxbits)
				cd->maxcode = cd->maxmaxcode;   /* won't get any bigger now */
			else
				cd->maxcode = UCOMPMAXCODE(cd->n_bits);
		}
		if(cd->clear_flg > 0)
		{
			cd->maxcode = UCOMPMAXCODE(cd->n_bits = UCOMPINIT_BITS);
			cd->clear_flg = 0;
		}

		/* This reads maximum n_bits characters into buf */
		cd->size = 0;
		while(cd->size < cd->n_bits) {
			int v;
			if (cd->insize == 0)
				break;
			v = zfile_getc (in);
			if (v == -1)
				break;
			cd->insize--;
			cd->buf[cd->size++] = v;
		}
		if(cd->size <= 0)
			return -1;

		cd->offset = 0;
		/* Round size down to integral number of codes */
		cd->size = (cd->size << 3) - (cd->n_bits - 1);
	}

	r_off = cd->offset;
	bits = cd->n_bits;

	/* Get to the first byte. */
	bp += (r_off >> 3);
	r_off &= 7;

	/* Get first part (low order bits) */
	code = (*bp++ >> r_off);
	bits -= (8 - r_off);
	r_off = 8 - r_off;                    /* now, offset into code word */

	/* Get any 8 bit parts in the middle (<=1 for up to 16 bits). */
	if(bits >= 8)
	{
		code |= *bp++ << r_off;
		r_off += 8;
		bits -= 8;
	}

	/* high order bits. */
	code |= (*bp & ((1<<bits)-1)) << r_off;
	cd->offset += cd->n_bits;

	return code;
}

// could remove this and use XFD-client to decompress it..
//
static uint32_t xadIO_UnCompress(struct zfile *in, struct zfile *out, int insize, rledata &rled, uint8_t bitinfo)
{
	int err = 0;

	if((bitinfo & UCOMPBIT_MASK) < UCOMPINIT_BITS)
		throw XADERR_ILLEGALDATA;

		UCompData cd;
		int finchar, code, oldcode, incode, blockcomp;
		uint8_t *stackp, *stack, *stackend;

		// Z ?
		stackp = stack = cd.stack;
		stackend = stack + UCOMPSTACKSIZE;
		cd.maxbits = bitinfo & UCOMPBIT_MASK;
		blockcomp = bitinfo & UCOMPBLOCK_MASK;
		cd.maxmaxcode = 1 << cd.maxbits;
		cd.maxcode = UCOMPMAXCODE(cd.n_bits = UCOMPINIT_BITS);
		cd.free_ent = blockcomp ? UCOMPFIRST : 256;
		cd.insize = insize;
		cd.rled = rled;

		if (cd.init() == false)
		{
			throw IOException("Failed to init uncompress data");
		}
				if((finchar = oldcode = UCompgetcode(in, cd)) == -1)
					return XADERR_DECRUNCH;
					
					putrle (finchar, out, cd->rled); /* first code must be 8 bits = uint8_t */

					while((code = UCompgetcode(in, cd)) > -1)
					{
						if((code == UCOMPCLEAR) && blockcomp)
						{
							for(code = 255; code >= 0; code--)
								cd->tab_prefixof[code] = 0;
							cd->clear_flg = 1;
							cd->free_ent = UCOMPFIRST - 1;
							if((code = UCompgetcode(in, cd)) == -1)
								break;                                /* O, untimely death! */
						}
						incode = code;

						/* Special case for KwKwK string. */
						if(code >= cd->free_ent)
						{
							if(code > cd->free_ent)
							{
								err = XADERR_ILLEGALDATA;
								break;
							}
							*stackp++ = finchar;
							code = oldcode;
						}

						/* Generate output characters in reverse order */
						while(stackp < stackend && code >= 256)
						{
							*stackp++ = cd->tab_suffixof[code];
							code = cd->tab_prefixof[code];
						}
						if(stackp >= stackend)
						{
							err = XADERR_ILLEGALDATA;
							break;
						}
						*(stackp++) = finchar = cd->tab_suffixof[code];

						/* And put them out in forward order */
						do
						{
							putrle (*(--stackp), out, cd->rled);
						} while(stackp > stack);

						/* Generate the new entry. */
						if((code = cd->free_ent) < cd->maxmaxcode)
						{
							cd->tab_prefixof[code] = (uint16_t) oldcode;
							cd->tab_suffixof[code] = finchar;
							cd->free_ent = code+1;
						}
						/* Remember previous code. */
						oldcode = incode;
					}
	return err;
}


#define COMPBUF 30000

struct zfile *unwarp(struct zfile *zf)
{
	int err = 0;
	uint8_t buf[26] = { 0 };
	int algo, side, track;
	int pos, dstpos, olddstpos;
	uint32_t size;
	struct zfile *nf = NULL, *tmpf = NULL;
	uint8_t *data;
	int outsize = 11 * 512;
	int outsize2 = 11 * (512 + 16);
	int emptysize = 1760 * 512;
	//struct rledata rled;
	

	uint8_t *zero = ::malloc(sizeof(uint8_t)*outsize2);
	//::memset(zero, 0, sizeof(uint8_t)*outsize2);
	olddstpos = 0;
	for (;;) {
		if (zfile_fread (buf, sizeof buf, 1, zf) == 0)
			break;
		if (!iswrp (buf))
			break;
		if (!nf) {
			nf = zfile_fopen_empty (zf, L"zipped.wrp", emptysize);
			tmpf = zfile_fopen_empty (zf, L"tmp", outsize2);
		}
		track = (buf[10] << 8) | buf[11];
		algo = buf[19];
		side = -1;
		if (!memcmp (buf + 12, "BOT\0", 4))
			side = 1;
		if (!memcmp (buf + 12, "TOP\0", 4))
			side = 0;
		uint16_t crc = (buf[20] << 8) | buf[21];
		pos = zfile_ftell (zf);
		dstpos = -1;
		size = (buf[22] << 24) | (buf[23] << 16) | (buf[24] << 8) | buf[25];
		if (side >= 0 && track >= 0 && track <= 79)
			dstpos = track * 22 * 512 + (side * 11 * 512);
			
		// write from temp-file to dest-file ?
		zfile_fseek (tmpf, 0, SEEK_SET); 
		zfile_fwrite (zero, outsize2, 1, tmpf); // 
		zfile_fseek (tmpf, 0, SEEK_SET); // 
		
		err = 0;
		
		rledata rled = 0;
		//memset (&rled, 0, sizeof rled);

		switch (algo)
		{
		case 1:
			if (zfile_getc (zf) != 12) // offset 26 ?
				err = XADERR_ILLEGALDATA;
			else
				err = xadIO_UnCompress (zf, tmpf, size - 1, rled, 12 | UCOMPBLOCK_MASK);
			break;
		case 2:
			err = ARCunsqueeze (zf, tmpf, rled);
			break;
		case 0:
		case 3:
			{
				for (int i = 0; i < size; i++) 
				{
					uint8_t v = zfile_getc (zf);
					putrle (v, tmpf, algo == 3 ? rled : NULL);
				}
			}
			break;
		default:
			throw IOException("WRP unknown compression method"); //%d, track=%d,size=%d\n", algo, track, side);
			//goto end;
			break;
		}
		if (err) 
		{
			throw IOException("WRP corrupt data"); //, track=%d,side=%d,err=%d\n", track, side, err);
		} 
		else 
		{
			int offset = zfile_ftell (tmpf);
			data = zfile_getdata (tmpf, 0, offset);
			uint16_t crc2 = m_crc16.wrpcrc16(data, offset);
			if (crc != crc2)
			{
				throw IOException("WRP crc error"); // %04x<>%04x, track=%d,side=%d\n", crc, crc2, track, side);
			}
			xfree (data);
		}
		if (dstpos >= 0) 
		{
			zfile_fseek (nf, dstpos, SEEK_SET);
			data = zfile_getdata (tmpf, 0, outsize);
			zfile_fwrite (data, outsize, 1, nf);
		}
		zfile_fseek (zf, pos + size, SEEK_SET);
	}
end:
	xfree (zero);
	zfile_fclose (tmpf);
	if (nf) {
		zfile_fclose (zf);
		zf = nf;
	}
	return zf;
}


void CUnWarp::decrunchRle(WarpDiskInfo *info)
{
	// just check once, before looping
	if (info->algo == ALGO_RLE2) 
	{
		rledata rled = 0;
		for (uint32_t i = 0; i < info->size; i++) 
		{
			uint8_t v = m_ReadBuffer.GetNextByte();
			putrle(v, &m_DecrunchBuffer, rled);
		}
	}
	else
	{
		for (uint32_t i = 0; i < info->size; i++) 
		{
			uint8_t v = m_ReadBuffer.GetNextByte();
			putrle(v, &m_DecrunchBuffer, NULL); <- fix this
		}
	}
}

void CUnWarp::unsqueezeArc(WarpDiskInfo *info)
{
	// TODO:
}

void CUnWarp::uncompress(WarpDiskInfo *info)
{
	// TODO:
	if (m_ReadBuffer.GetNextByte() != 12) // some kinda marker in there?
	{
	}
}

bool CUnWarp::unpack()
{
	// for now, just file-input (buffer later)
	if (m_sourceFile.length() == 0)
	{
		throw IOException("Input was not given");
	}
	// for now, just file-input (buffer later)
	if (m_destFile.length() == 0)
	{
		throw IOException("Output was not given");
	}

	// just read it to buffer, should not be too large..
	// TODO: should check type before reading entirely..?
	if (m_sourceFile.length() > 0)
	{
		CAnsiFile archive(m_sourceFile);
		if (archive.IsOk() == false)
		{
			throw ArcException("Failed opening file", m_sourceFile);
		}
		m_nFileSize = archive.GetSize();
		m_ReadBuffer.PrepareBuffer(m_nFileSize, false);
		if (archive.Read(m_ReadBuffer.GetBegin(), m_nFileSize) == false)
		{
			throw ArcException("Failed reading file", m_sourceFile);
		}
		archive.Close(); // destructor should close already..
	}
	// otherwise just use buffer as input
	if (isSupported(m_ReadBuffer.GetBegin()) == false)
	{
		throw IOException("Unsupported file");
	}
	m_ReadBuffer.SetCurrentPos(0); // update later..

	// prepare for entire disk image for decrunching,
	// less than 1MB anyway..
	m_DecrunchBuffer.PrepareBuffer(max_imagesize, false);
	
	// temp, keep somewhere
	// TODO: loop this for each track?
	//while info.nDestPos >= 0 ??
	
	WarpDiskInfo info;
	info.parseBuf(m_ReadBuffer.GetAtCurrent());
	info.nOffset = 26;
	info.setDestOffset();
	
	if (info.algo < 0 || info.algo > 3)
	{
		throw IOException("Unknown packing method");
	}
	
	m_ReadBuffer.SetCurrentPos(26); // after identifier and other skipped data
	
	size_t nDecrPos = m_DecrunchBuffer.GetCurrentPos(); // keep pos for crc-check
	
	switch (info.algo)
	{
	case ALGO_RLE1:
	case ALGO_RLE2:
		decrunchRle(&info);
		break;
	case ALGO_Z:
		uncompress(&info);
		break;
	case ALGO_RLE2:
		unsqueezeArc(&info);
		break;
	default:
		// silence GCC (we checked already)
		break;
	}

	// check crc on decrunched data
	// (which was added to decrunch-buffer)
	uint16_t crc = m_crc16.wrpcrc16(m_DecrunchBuffer.GetAt(nDecrPos), 
									m_DecrunchBuffer.GetCurrentPos() - nDecrPos);
	if (crc != info.crc)
	{
		throw IOException("WRP crc error");
	}
	
	if (info.nDestPos >= 0) 
	{
		m_ReadBuffer.SetCurrentPos(info.nDestPos);
		//zfile_fseek (nf, dstpos, SEEK_SET);
		//data = zfile_getdata (tmpf, 0, outsize);
		//zfile_fwrite (data, outsize, 1, nf);
	}
	// loop ends here, continue on next track?

	// finally, output: write all at once when ready (less IO)
	if (m_destFile.length() > 0)
	{
		CAnsiFile output(m_destFile, true);
		if (output.IsOk() == false)
		{
			throw ArcException("Failed opening file", m_destFile);
		}
		// check where to keep actual used size in bytes..
		// (buffer may be larger for overrun)
		if (output.Write(m_DecrunchBuffer.GetBegin(), m_DecrunchBuffer.GetSize()) == false)
		{
			throw ArcException("Failed writing file", m_destFile);
		}
		output.Flush();
		output.Close();
	}
	// otherwise just expect user to access the buffer..

	return true;
}

