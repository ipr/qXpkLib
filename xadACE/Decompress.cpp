/* ------------------------------------------------------------------------ */
/*                                                                          */
/*      These are the decompression algorithms.                             */
/*      Don't change here anything (apart from memory allocation perhaps).  */
/*      Any changes will very likely cause bugs!                            */
/*                                                                          */
/* ------------------------------------------------------------------------ */
/*  ML - 01/2004: changed licence to GPL                                    */
/* ------------------------------------------------------------------------ */
/*
 * Modified for C++ library version by Ilkka Prusi <ilkka.prusi@gmail.com>
 *
 * Also attempt to fix bugs mentioned in original comments (see above)..
 *
*/

#include "Decompress.h"


//------------------------------ QUICKSORT ---------------------------------//

void sortrange(const INT left, const INT right)
{
   INT  zl = left,
        zr = right,
        hyphen;

   hyphen = sort_freq[right];

   //divides by hyphen the given range into 2 parts
   do
   {
      while (sort_freq[zl] > hyphen)
      {
         zl++;
      }
      while (sort_freq[zr] < hyphen)
      {
         zr--;
      }
      //found a too small (left side) and
      //a too big (right side) element-->exchange them
      if (zl <= zr)
      {
         xchg_def(sort_freq[zl], sort_freq[zr]);
         xchg_def(sort_org[zl], sort_org[zr]);
         zl++;
         zr--;
      }
   } while (zl < zr);

   //sort partial ranges - when very small, sort directly
   if (left < zr)
   {
      if (left < zr - 1)
      {
         sortrange(left, zr);
      }
      else if (sort_freq[left] < sort_freq[zr])
      {
         xchg_def(sort_freq[left], sort_freq[zr]);
         xchg_def(sort_org[left], sort_org[zr]);
      }
   }

   if (right > zl)
   {
      if (zl < right - 1)
      {
         sortrange(zl, right);
      }
      else if (sort_freq[zl] < sort_freq[right])
      {
         xchg_def(sort_freq[zl], sort_freq[right]);
         xchg_def(sort_org[zl], sort_org[right]);
      }
   }
}

// should be unsigned 16-bit?
void quicksort(const int16_t n)
{
   for (int16_t i = n + 1; i--;)
   {
      sort_org[i] = i;
   }
   sortrange(0, n);
}

//------------------------------ read bits ---------------------------------//
void readdat(void)
{
   UINT i;

   i = (size_rdb - 2) << 2;
   rpos -= size_rdb - 2;
   buf_rd[0] = buf_rd[size_rdb - 2];
   buf_rd[1] = buf_rd[size_rdb - 1];
   read_adds_blk((CHAR *) & buf_rd[2], i);
#ifdef HI_LO_BYTE_ORDER
   {
      ULONG *p;
      i>>=2;    // count LONGs not BYTEs
      p=&buf_rd[2];
      while (i--)
      {
         LONGswap(p);
         p++; 
      }
   }
#endif
}

#define addbits(bits)                                   \
{                                                       \
  rpos+=(bits_rd+=bits)>>5;                             \
  bits_rd&=31;                                          \
  if (rpos==(size_rdb-2)) readdat();                    \
  code_rd=(buf_rd[rpos] << bits_rd)                     \
    +((buf_rd[rpos+1] >> (32-bits_rd))&(!bits_rd-1));   \
}



//---------------------- COMMENT DECOMPRESSION -----------------------------//

void dcpr_comm_init(void)
{
   INT  i;

   i = comm_cpr_size > size_rdb * sizeof(LONG) ? size_rdb * sizeof(LONG) : comm_cpr_size;
   if (!f_err)
      memcpy(buf_rd, comm, i);
#ifdef HI_LO_BYTE_ORDER
   {
      ULONG *p;
      i>>=2;    // count LONGs not BYTEs
      p=buf_rd;
      while (i--)
      {
         LONGswap(p);
         p++; 
      }
   }
#endif
   code_rd = buf_rd[0];
   rpos = bits_rd = 0;
}

void dcpr_comm(INT comm_size)
{
   SHORT hash[comm_cpr_hf(255, 255) + 1];
   INT  dpos = 0,
        c,
        pos,
        len,
        hs;

   memset(&hash, 0, sizeof(hash));
   if (comm_cpr_size)
   {
      dcpr_comm_init();
      len = code_rd >> (32 - 15);
      addbits(15);
      if (len >= comm_size)
         len = comm_size - 1;
      if (read_wd(maxwd_mn, dcpr_code_mn, dcpr_wd_mn, max_cd_mn))
         do
         {
            if (dpos > 1)
            {
               pos = hash[hs = comm_cpr_hf(comm[dpos - 1], comm[dpos - 2])];
               hash[hs] = dpos;
            }
            addbits(dcpr_wd_mn[(c = dcpr_code_mn[code_rd >> (32 - maxwd_mn)])]);
            if (rpos == size_rdb - 3)
               rpos = 0;
            if (c > 255)
            {
               c -= 256;
               c += 2;
               while (c--)
                  comm[dpos++] = comm[pos++];
            }
            else
            {
               comm[dpos++] = c;
            }
         }
         while (dpos < len);
      comm[len] = 0;
   }
}

//------------------------- LZW DECOMPRESSION ------------------------------//
void wrchar(CHAR ch)
{
   dcpr_do++;

   dcpr_text[dcpr_dpos] = ch;
   dcpr_dpos++;
   dcpr_dpos &= dcpr_dican;
}

void copystr(LONG d, INT l)
{
   INT  mpos;

   dcpr_do += l;

   mpos = dcpr_dpos - d;
   mpos &= dcpr_dican;

   if ((mpos >= dcpr_dicsiz - maxlength) || (dcpr_dpos >= dcpr_dicsiz - maxlength))
   {
      while (l--)
      {
         dcpr_text[dcpr_dpos] = dcpr_text[mpos];
         dcpr_dpos++;
         dcpr_dpos &= dcpr_dican;
         mpos++;
         mpos &= dcpr_dican;
      }
   }
   else
   {
      while (l--)
         dcpr_text[dcpr_dpos++] = dcpr_text[mpos++];
      dcpr_dpos &= dcpr_dican;
   }
}

void decompress(void)
{
   INT  c,
        lg,
        i,
        k;
   ULONG dist;

   while (dcpr_do < dcpr_do_max)
   {
      if (!blocksize)
         if (calc_dectabs() == false)
         {
            return;
         }

      addbits(dcpr_wd_mn[(c = dcpr_code_mn[code_rd >> (32 - maxwd_mn)])]);
      blocksize--;
      if (c > 255)
      {
         if (c > 259)
         {
            if ((c -= 260) > 1)
            {
               dist = (code_rd >> (33 - c)) + (1L << (c - 1));
               addbits(c - 1);
            }
            else
               dist = c;
            dcpr_olddist[(dcpr_oldnum = (dcpr_oldnum + 1) & 3)] = dist;
            i = 2;
            if (dist > maxdis2)
            {
               i++;
               if (dist > maxdis3)
                  i++;
            }
         }
         else
         {
            dist = dcpr_olddist[(dcpr_oldnum - (c &= 255)) & 3];
            for (k = c + 1; k--;)
               dcpr_olddist[(dcpr_oldnum - k) & 3] = dcpr_olddist[(dcpr_oldnum - k + 1) & 3];
            dcpr_olddist[dcpr_oldnum] = dist;
            i = 2;
            if (c > 1)
               i++;
         }
         addbits(dcpr_wd_lg[(lg = dcpr_code_lg[code_rd >> (32 - maxwd_lg)])]);
         dist++;
         lg += i;
         copystr(dist, lg);
      }
      else
         wrchar(c);
   }
}

//-------------------------- HUFFMAN ROUTINES ------------------------------//
bool  makecode(UINT maxwd, UINT size1_t, UCHAR * wd, USHORT * code)
{
   UINT maxc,
        size2_t,
        l,
        c,
        i,
        max_make_code;

   memcpy(&sort_freq, wd, (size1_t + 1) * sizeof(CHAR));
   if (size1_t)
   {
      quicksort(size1_t);
   }
   else
   {
      sort_org[0] = 0;
   }
   sort_freq[size1_t + 1] = size2_t = c = 0;
   while (sort_freq[size2_t])
   {
      size2_t++;
   }
   if (size2_t < 2)
   {
      i = sort_org[0];
      wd[i] = 1;
      size2_t += (size2_t == 0);
   }
   size2_t--;

   max_make_code = 1 << maxwd;
   for (i = size2_t + 1; i-- && c < max_make_code;)
   {
      maxc = 1 << (maxwd - sort_freq[i]);
      l = sort_org[i];
      if (c + maxc > max_make_code)
      {
         dcpr_do = dcpr_do_max;
         return false;
      }
      memset16(&code[c], l, maxc);
      c += maxc;
   }
   return true;
}

bool  read_wd(UINT maxwd, USHORT * code, UCHAR * wd, INT max_el)
{
   UINT c,
        i,
        j,
        num_el,
        l,
        uplim,
        lolim;

   memset(wd, 0, max_el * sizeof(CHAR));
   memset(code, 0, (1 << maxwd) * sizeof(SHORT));

   num_el = code_rd >> (32 - 9);
   addbits(9);
   if (num_el > max_el)
   {
      num_el = max_el;
   }

   lolim = code_rd >> (32 - 4);
   addbits(4);
   uplim = code_rd >> (32 - 4);
   addbits(4);

   for (i = -1; ++i <= uplim;)
   {
      wd_svwd[i] = code_rd >> (32 - 3);
      addbits(3);
   }
   if (makecode(maxwd_svwd, uplim, wd_svwd, code) == false)
   {
      return false;
   }
   j = 0;
   while (j <= num_el)
   {
      c = code[code_rd >> (32 - maxwd_svwd)];
      addbits(wd_svwd[c]);
      if (c < uplim)
      {
         wd[j++] = c;
      }
      else
      {
         l = (code_rd >> 28) + 4;
         addbits(4);
         while (l-- && j <= num_el)
         {
            wd[j++] = 0;
         }
      }
   }
   if (uplim)
      for (i = 0; ++i <= num_el;)
         wd[i] = (wd[i] + wd[i - 1]) % uplim;
   for (i = -1; ++i <= num_el;)
      if (wd[i])
         wd[i] += lolim;

   return (makecode(maxwd, num_el, wd, code));

}

bool CDecompress::calc_dectabs(void)
{
   if (read_wd(maxwd_mn, dcpr_code_mn, dcpr_wd_mn, max_cd_mn) == false
       || read_wd(maxwd_lg, dcpr_code_lg, dcpr_wd_lg, max_cd_lg) == false)
   {
      return false;
   }

   blocksize = code_rd >> (32 - 15);
   addbits(15);

   return true;
}

//---------------------------- BLOCK ROUTINES ------------------------------//
int16_t CDecompress::decompress_blk(CHAR * buf, UINT len)
{
   int32_t old_pos = dcpr_dpos;

   dcpr_do = 0;
   if ((dcpr_do_max = len - maxlength) > dcpr_size)
   {
      dcpr_do_max = dcpr_size;
   }
   if ((LONG) dcpr_size > 0 && dcpr_do_max)
   {
      decompress();
      if (dcpr_do <= len)
      {
         if (old_pos + dcpr_do > dcpr_dicsiz)
         {
            int16_t i = dcpr_dicsiz - old_pos;
            memcpy(buf, &dcpr_text[old_pos], i);
            memcpy(&buf[i], dcpr_text, dcpr_do - i);
         }
         else
            memcpy(buf, &dcpr_text[old_pos], dcpr_do);
      }
   }
   dcpr_size -= dcpr_do;
   return (dcpr_do);
}

int16_t CDecompress::unstore(int8_t * buf, const size_t len)
{
   uint16_t rd = 0,
        i,
        pos = 0;
        
	// just memcpy() buffer to buffer instead of following stuff?
	//m_pDecrunchBuffer->Append(m_ReadBuffer.GetAtCurrent(), m_ReadBuffer.GetCurrentPos());
	
   while ((i = read_adds_blk((CHAR *) buf_rd, (INT) ((i = ((len > dcpr_size) ? dcpr_size : len)) > size_rdb ? size_rdb : i))) != 0)
   {
      rd += i;
      len -= i;
      dcpr_size -= i;
      memcpy(&buf[pos], buf_rd, i);
      pos += i;
   }
   
   for (uint16_t i = 0; i < rd; i++)
   {
      dcpr_text[dcpr_dpos] = buf[i];
      dcpr_dpos++;
      dcpr_dpos &= dcpr_dican;
   }
   return rd;
}

int16_t CDecompress::dcpr_adds_blk(int8_t *buf, const size_t len)
{
   int16_t r = 0;

   switch (fhead.TECH.TYPE)
   {
      case TYPE_STORE: // no compresion? (store-only)
         r = unstore(buf, len);
         break;
      case TYPE_LZ1:
         r = decompress_blk(buf, len);
         break;
      default:
		throw IOException("Unknown compression method");
   }
   m_Crc.updatecrc(buf, r);
   return r;
}


//----------------------------- INIT ROUTINES ------------------------------//

void CDecompress::dcpr_init_file(void)
{
   //rd_crc = CRC_MASK;
	m_Crc.ClearCrc();
	
   dcpr_size = fhead.SIZE;
   if (fhead.TECH.TYPE == TYPE_LZ1)
   {
      if ((fhead.TECH.PARM & 15) + 10 > dcpr_dic)
      {
         //printf("\nNot enough memory or dictionary of archive too large.\n");
         throw IOException("archive too large");
      }

      UINT i = size_rdb * sizeof(int32_t);
      read_adds_blk((CHAR *) buf_rd, i);
#ifdef HI_LO_BYTE_ORDER
      {
         ULONG *p;
         i>>=2;    // count LONGs not BYTEs
         p=buf_rd;
         while (i--)
         {
            LONGswap(p);
            p++; 
         }
      }
#endif
      code_rd = buf_rd[0];
      bits_rd = rpos = 0;

      blocksize = 0;
   }
   if (!adat.sol || dcpr_frst_file)
   {
      dcpr_dpos = 0;
   }

   dcpr_oldnum = 0;
   memset(&dcpr_olddist, 0, sizeof(dcpr_olddist));

   dcpr_frst_file = 0;
}

void CDecompress::dcpr_init(void)
{
	dcpr_frst_file = 1; // multi-volume archives? just bool ?

	dcpr_dic = 20;
	dcpr_dicsiz = (int32_t)(1 << dcpr_dic);

	// simplify, we are screwed either way if we can't allocate such small amount of memory..
	// 
	m_pDecrunchBuffer->PrepareBuffer(dcpr_dicsiz, false); // don't keep old data
	
	// this just tries allocation in loop until it succeeds?
	/*
	while ((dcpr_text = malloc(dcpr_dicsiz = (LONG) 1 << dcpr_dic))==NULL)
	{
		dcpr_dic--;
	}
	*/
	dcpr_dican = dcpr_dicsiz - 1;
}


///////// public methods

CDecompress::CDecompress(CReadBuffer *pReadBuffer, CReadBuffer *pDecrunchBuffer)
	: m_pRead(pReadBuffer)
	, m_pDecrunch(pDecrunchBuffer)
	, m_Crc()
	, dcpr_text(nullptr)
	, rpos(0)
	, dcpr_do(0)
	, dcpr_do_max(0)
	, blocksize(0)
	, dcpr_dic(0)
	, dcpr_oldnum(0)
	, bits_rd(0)
	, dcpr_frst_file(0)
	, dcpr_dpos(0)
	, cpr_dpos2(0)
	, dcpr_dicsiz(0)
	, dcpr_dican(0)
	, dcpr_size(0)
	, code_rd(0)
{
	::memset(dcpr_olddist, 0, 4*sizeof(uint32_t));
	
	size_rdb = m_pRead->GetSize();
	buf_rd = (uint32_t*)m_pRead->GetBegin();
	
	dcpr_init();
}


