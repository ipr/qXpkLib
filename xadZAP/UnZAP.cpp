/* This XAD client is (C) 2000-2001 Stuart Caie <kyzer@4u.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/* ZAP is a lesser-known Amiga disk archiver. This client is only tested
 * with ZAP v1.41. It uses byte-based run-length encoding and LZ77
 * compression, which is redundant because the LZ compressor handles
 * repeated byte runs really well. The archiver only supports normal
 * Amiga DOS disks (80*2*11*512 = 901120 bytes), and it compresses them
 * in 10 blocks of 8 cylinders. It also supports using the AmigaDOS
 * disk bitmap, but just by clearing those sectors before compression,
 * not some special format. Finally, it supports attaching a textfile/
 * banner to the archive, shown on extraction. ZAP is (C) 1990 GREMLIN of
 * MAYHEM.
 *
 * $VER: ZAP.c 1.2 (20.08.2001)";

 */

#include "UnZAP.h"


#define ZAP_VERSION     1
#define ZAP_REVISION    2


//#define XADBASE REG(a6, struct xadMasterBase *xadMasterBase)

/* work-doing macros */
#define SKIP(offset) if ((err = xadHookAccess(XADAC_INPUTSEEK, \
  (ULONG)(offset), NULL, ai))) goto exit_handler
#define SEEK(offset) SKIP((offset) - ai->xai_InPos)
#define READ(buffer,length) if ((err = xadHookAccess(XADAC_READ, \
  (ULONG)(length), (APTR)(buffer), ai))) goto exit_handler
#define WRITE(buffer,length) if ((err = xadHookAccess(XADAC_WRITE, \
  (ULONG)(length), (APTR)(buffer), ai))) goto exit_handler


#define READLONG(var) do { READ(&buffer,4); (var)=EndGetM32(buffer);} while(0)


/* main ZAP LZ77 decruncher */
struct ZAPdecr 
{
  /* bitbuffer, checksum, input pointer, input beginning, bits left, error */
  uint32_t bb; 
  uint32_t sum; 
  uint8_t *i; 
  uint8_t *in; 
  uint8_t bl; 
  uint8_t err;
};

uint32_t ZAP_getlong(struct ZAPdecr *zd) 
{
	uint8_t *in = (zd->i -= 4);
	if (in < zd->in) 
	{
		zd->err=XADERR_INPUT; 
		return 0;
	}
	return EndGetM32(in);
}

uint32_t ZAP_getbits(struct ZAPdecr *zd, int n) 
{
  int x=0; 
  uint32_t bb = zd->bb;
  while (n--) 
  {
    if (!zd->bl--) 
    { 
		zd->sum ^= (bb = ZAP_getlong(zd)); 
		zd->bl = 31; 
	}
    x = (x<<1) | (bb&1); bb >>= 1;
  }
  zd->bb = bb;
  return x;
}

#define BITS(n) (ZAP_getbits(&zd, (n)))
#define BIT     (ZAP_getbits(&zd, 1))
#define PUT(x)  if(--o < out) return XADERR_OUTPUT; else *o=(UBYTE)(x)

int32_t ZAP_decrunch(uint8_t *in, uint8_t *out, uint32_t inlen) 
{
  struct ZAPdecr zd;
  uint8_t *match, *o;
  int x, y;

  /* initialise state */
  zd.bb  = 0;
  zd.bl  = 0;
  zd.err = XADERR_OK;
  zd.in  = in;
  zd.i   = in + inlen;
  o      = out + ZAP_getlong(&zd);
  zd.sum = ZAP_getlong(&zd);

  /* throw away bits up to first set bit */
  while (!BIT) if (zd.err) break;

  while (o > out) {
    if ((x=BITS(2)) == 3) x = BIT ? (BIT ? -1 : (BITS(8)+19)) : (BITS(4)+3);
    if (x >= 0) do { PUT(BITS(8)); } while (x-- > 0);
    if (o <= out) break;

    if ((x = BITS(2)) == 3) {
      x = BIT ? (BIT ? -1 : (BITS(8)+20)) : (BITS(4)+4);
      if (x >= 0) y = BIT ? (BITS(13)+258) : (BITS(8)+2);
    }
    else {
      if (x) { x++; y = BIT ? (BITS(13)+258) : (BITS(8)+2); }
      else   { x=1; y = BITS(8)+2; }
    }
    match = o + y; if (x >= 0) do { PUT(*--match); } while (x-- > 0);
  }

  /* checksum should be 0 on exit */
  if (!zd.err && zd.sum) zd.err = XADERR_CHECKSUM;
  return (LONG) zd.err;
}


/* RLE decoder for compressed blocks */
static void ZAP_rledecode(uint8_t *in, uint8_t *out) {
  uint8_t *i = in+9, *o = out, *end=out + EndGetM32(in+4);
  int code=in[8], x, y;

  while (o < end) {
    if ((x = *i++) == code) {
      if ((x = *i++)) { y=*i++; x+=3; while (x--) *o++=y; } else *o++ = code;
    }
    else *o++ = x;
  }
}


int32_t ZAP_GetInfo(xadArchiveInfo *ai)
{
  uint8_t buffer[4], *textin = NULL;
  struct xadTextInfo *ti;
  int32_t err = XADERR_OK;
  uint32_t tfsize;


  /* read and skip header ID text */
  READLONG(tfsize); 
  SKIP(tfsize);

  /* read attached textfile (if it exists) */
  READLONG(tfsize);
  xdi->xdi_DataPos = ai->xai_InPos + tfsize;

  if (tfsize) {
    ti = new xadTextInfo();
    xdi->xdi_TextInfo = ti;

	textin = malloc(tfsize);
    READ(textin, tfsize);
    ti->xti_Size = EndGetM32(textin+tfsize-4);
    ti->xti_Text = malloc(ti->xti_Size+1);
    if ((err = ZAP_decrunch(textin, ti->xti_Text, tfsize))) 
		// -> throw
		goto exit_handler;
    ti->xti_Text[ti->xti_Size] = '\0';
  }
  else {
    xdi->xdi_TextInfo = NULL;
  }
    
exit_handler:
  if (textin) FREE(textin);

  if (err) {
    if (!xdi) return err;
    ai->xai_Flags |= XADAIF_FILECORRUPT;
    ai->xai_LastError = err;
  }
  return XADERR_OK;
}



const size_t ZAP_CYLSIZE   = (22*512);    /* size of one cylinder */
const size_t ZAP_BLOCKSIZE = (22*512*8);  /* size of one block    */

struct ZAPstate 
{
  uint8_t data[ZAP_BLOCKSIZE+260];
  uint8_t temp[ZAP_BLOCKSIZE];
  uint8_t block;
};

int32_t ZAP_UnArchive(xadArchiveInfo *ai)
{
  ZAPstate *zs;
  uint8_t buffer[4], cyl, block;
  int32_t err = XADERR_OK;
  uint32_t crlen;

  if (!(zs = (struct ZAPstate *) ai->xai_PrivateClient)) 
  {
	zs = new ZAPstate();
    ai->xai_PrivateClient = (void*) zs;
    zs->block = 99; /* sentinel value */
  }
  
  for (cyl = ai->xai_LowCyl; cyl <= ai->xai_HighCyl; cyl++) {
    /* if the current block isn't the one needed... */
    if ((block = cyl >> 3) != zs->block) {

      /* if it's not just the next block we need */
      if (zs->block != (block-1)) {
        /* go back to start of disk and skip forward to appropriate block */
        int skip = block;
        SEEK(ai->xai_CurDisk->xdi_DataPos);
        while (skip--) { READLONG(crlen); SKIP(crlen); }
      }

      /* read (possibly crunched) block into block buffer */
      READLONG(crlen);
      if (crlen > ZAP_BLOCKSIZE) 
		throw "crunched block too large";
		
      READ(zs->data, crlen);

      /* if the block is crunched */
      if (crlen != ZAP_BLOCKSIZE) {
        /* decrunch it to temp buffer */
        if (EndGetM32(zs->data+crlen-4) > ZAP_BLOCKSIZE) ERROR(OUTPUT);
        if ((err = ZAP_decrunch(zs->data, zs->temp, crlen))) goto exit_handler;

        /* un-RLE back to block buffer */
        ZAP_rledecode(zs->temp, zs->data);
      }
      zs->block = block;
    }

    WRITE(zs->data + ((cyl & 7) * ZAP_CYLSIZE), ZAP_CYLSIZE);
  }

exit_handler:
  return err;
}


// constant format information basically?
// file is just "raw" data without metadata?
//
struct ZapDiskInfo
{
  const uint16_t xdi_EntryNumber  = 1;
  const uint16_t xdi_SectorSize   = 512;
  const uint16_t xdi_TotalSectors = 80 * 22;
  const uint16_t xdi_Cylinders    = 80;
  const uint16_t xdi_CylSectors   = 22;
  const uint16_t xdi_Heads        = 2;
  const uint16_t xdi_TrackSectors = 11;
  const uint16_t xdi_LowCyl       = 0;
  const uint16_t xdi_HighCyl      = 79;
};

bool CUnZAP::unpack()
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
}

