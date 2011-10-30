///////////////////////////
//
// UnDMS C++ library version, 
// modified by Ilkka Prusi <ilkka.prusi@gmail.com>
//
// based on undms-1.3.c:
// Author:	dt14uow.edu.au (David Tritscher)
//
/* $VER: undms 1.3 (12.2.98) */
/* Release 1.2: 12.2.98 */
/* Release 1.1: 10.2.98 */
/* Release 1.0: 6.2.98 */
/* Created: 30.1.98 */

/* DMS Extract in (supposedly) portable C.                               */

/* Compile with:                                                         */
/* gcc undms.c -oundms -O2                                               */

/* Thanks to Johan Erkers for beta testing.                              */

/* Thanks to Andre Rodrigues de la Rocha for helping me debug the quick  */
/* function and provided up to date variable names.                      */

/* I coded this to make sure it would work with unsigned bytes to try    */
/* and make it as portable as possible. It should be transparent to      */
/* byte order and alignment. Checking of EOF and number of tracks is     */
/* quite primitive and could no doubt be improved. The decrunch          */
/* functions all check for out of bounds data so no overwriting of       */
/* buffers should be possible, even for severly corrupt data.            */
/* Decryption is NOT supported at this time. (btw. it is very weak and   */
/* there is even code in some DMS programs which allow a brute force     */
/* scanning of passwords)                                                */

/* Ooops. Put the wrong value in quick function. Now it should work :^)  */

/* Sigh. More obvious bugs which I missed. Wrong check for fopen().      */

/* Fixed up the streams a little. Now gives EOF messages instead of      */
/* Error 0.                                                              */


#include "UnDMS.h"

#ifdef _DEBUG
#include <QDebug>
#endif


uint8_t quick_buffer[256];
uint8_t medium_buffer[16384];

uint8_t deep_buffer[16384];
uint16_t deep_weights[628];
uint16_t deep_symbols[628];
uint16_t deep_hash[942];

uint8_t heavy_buffer[8192];
uint16_t heavy_literal_table[5120];
uint16_t heavy_offset_table[320];
uint8_t heavy_literal_len[512];
uint8_t heavy_offset_len[32];

unsigned int quick_local;
unsigned int medium_local;
unsigned int deep_local;
unsigned int heavy_local;
unsigned int heavy_last_offset;

unsigned int no_clear_flag;

/* --------------------------------------------------------------------- */

static const unsigned short CRCTable[256]=
{
   0x0000,0xC0C1,0xC181,0x0140,0xC301,0x03C0,0x0280,0xC241,
   0xC601,0x06C0,0x0780,0xC741,0x0500,0xC5C1,0xC481,0x0440,
   0xCC01,0x0CC0,0x0D80,0xCD41,0x0F00,0xCFC1,0xCE81,0x0E40,
   0x0A00,0xCAC1,0xCB81,0x0B40,0xC901,0x09C0,0x0880,0xC841,
   0xD801,0x18C0,0x1980,0xD941,0x1B00,0xDBC1,0xDA81,0x1A40,
   0x1E00,0xDEC1,0xDF81,0x1F40,0xDD01,0x1DC0,0x1C80,0xDC41,
   0x1400,0xD4C1,0xD581,0x1540,0xD701,0x17C0,0x1680,0xD641,
   0xD201,0x12C0,0x1380,0xD341,0x1100,0xD1C1,0xD081,0x1040,
   0xF001,0x30C0,0x3180,0xF141,0x3300,0xF3C1,0xF281,0x3240,
   0x3600,0xF6C1,0xF781,0x3740,0xF501,0x35C0,0x3480,0xF441,
   0x3C00,0xFCC1,0xFD81,0x3D40,0xFF01,0x3FC0,0x3E80,0xFE41,
   0xFA01,0x3AC0,0x3B80,0xFB41,0x3900,0xF9C1,0xF881,0x3840,
   0x2800,0xE8C1,0xE981,0x2940,0xEB01,0x2BC0,0x2A80,0xEA41,
   0xEE01,0x2EC0,0x2F80,0xEF41,0x2D00,0xEDC1,0xEC81,0x2C40,
   0xE401,0x24C0,0x2580,0xE541,0x2700,0xE7C1,0xE681,0x2640,
   0x2200,0xE2C1,0xE381,0x2340,0xE101,0x21C0,0x2080,0xE041,
   0xA001,0x60C0,0x6180,0xA141,0x6300,0xA3C1,0xA281,0x6240,
   0x6600,0xA6C1,0xA781,0x6740,0xA501,0x65C0,0x6480,0xA441,
   0x6C00,0xACC1,0xAD81,0x6D40,0xAF01,0x6FC0,0x6E80,0xAE41,
   0xAA01,0x6AC0,0x6B80,0xAB41,0x6900,0xA9C1,0xA881,0x6840,
   0x7800,0xB8C1,0xB981,0x7940,0xBB01,0x7BC0,0x7A80,0xBA41,
   0xBE01,0x7EC0,0x7F80,0xBF41,0x7D00,0xBDC1,0xBC81,0x7C40,
   0xB401,0x74C0,0x7580,0xB541,0x7700,0xB7C1,0xB681,0x7640,
   0x7200,0xB2C1,0xB381,0x7340,0xB101,0x71C0,0x7080,0xB041,
   0x5000,0x90C1,0x9181,0x5140,0x9301,0x53C0,0x5280,0x9241,
   0x9601,0x56C0,0x5780,0x9741,0x5500,0x95C1,0x9481,0x5440,
   0x9C01,0x5CC0,0x5D80,0x9D41,0x5F00,0x9FC1,0x9E81,0x5E40,
   0x5A00,0x9AC1,0x9B81,0x5B40,0x9901,0x59C0,0x5880,0x9841,
   0x8801,0x48C0,0x4980,0x8941,0x4B00,0x8BC1,0x8A81,0x4A40,
   0x4E00,0x8EC1,0x8F81,0x4F40,0x8D01,0x4DC0,0x4C80,0x8C41,
   0x4400,0x84C1,0x8581,0x4540,0x8701,0x47C0,0x4680,0x8641,
   0x8201,0x42C0,0x4380,0x8341,0x4100,0x81C1,0x8081,0x4040
};

/* --------------------------------------------------------------------- */

static const unsigned char table_one[256]=
{
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
   3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,5,5,5,5,5,5,5,5,
   6,6,6,6,6,6,6,6,7,7,7,7,7,7,7,7,8,8,8,8,8,8,8,8,9,9,9,9,9,9,9,9,
   10,10,10,10,10,10,10,10,11,11,11,11,11,11,11,11,
   12,12,12,12,13,13,13,13,14,14,14,14,15,15,15,15,
   16,16,16,16,17,17,17,17,18,18,18,18,19,19,19,19,
   20,20,20,20,21,21,21,21,22,22,22,22,23,23,23,23,
   24,24,25,25,26,26,27,27,28,28,29,29,30,30,31,31,
   32,32,33,33,34,34,35,35,36,36,37,37,38,38,39,39,
   40,40,41,41,42,42,43,43,44,44,45,45,46,46,47,47,
   48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63
};

static const unsigned char table_two[256]=
{
   3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
   4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
   4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
   5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
   5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
   6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
   7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
   7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8
};

/* --------------------------------------------------------------------- */

unsigned int mycrc(unsigned char *memory, unsigned int length)
{
   register unsigned int temp = 0;

   while(length--)
      temp = CRCTable[(temp ^ *memory++) & 255] ^ ((temp >> 8) & 255);

   return (temp & 65535);
}

/* --------------------------------------------------------------------- */

unsigned int mysimplecrc(unsigned char *memory, unsigned int length)
{
   register unsigned int temp = 0;

   while(length--) temp += *memory++;

   return (temp & 65535);
}

/* --------------------------------------------------------------------- */

int crunch_store(unsigned char *source, unsigned char *source_end,
                 unsigned char *destination, unsigned char *destination_end)
{
   while((destination < destination_end) && (source < source_end))
      *destination++ = *source++;

//if(DEBUG)printf(" ...store %s",((source != source_end) || (destination != destination_end)) ? "bad" : "good");
   return((source != source_end) || (destination != destination_end));
}

/* --------------------------------------------------------------------- */

int crunch_rle(unsigned char *source, unsigned char *source_end,
               unsigned char *destination, unsigned char *destination_end)
{
   register unsigned char temp;
   register int count;

   while((destination < destination_end) && (source < source_end))
   {
      if((temp = *source++) != 144) /* crunch_dle */
      {
         *destination++ = temp;
      }
      else if((count = *source++) == 0)
      {
         *destination++ = temp;
      }
      else
      {
         temp = *source++;
         if(count == 255)
         {
            count = *source++ << 8;
            count += *source++;
         }
         while((destination < destination_end) && (count--))
            *destination++ = temp;
      }
   } /* while */

//if(DEBUG)printf(" ...runlength %s",((source != source_end) || (destination != destination_end)) ? "bad" : "good");
   return((source != source_end) || (destination != destination_end));
}

/* --------------------------------------------------------------------- */

int crunch_quick(unsigned char *source, unsigned char *source_end,
               unsigned char *destination, unsigned char *destination_end)
{
   register unsigned int control = 0;
   register int shift = 0;
   int count, offset;

   quick_local += 5; /* i have no idea why it adds 5 */
   if(!no_clear_flag)
      for(quick_local = count = 0; count < 256; count++)
         quick_buffer[count] = 0;

   while((destination < destination_end) && (source < source_end))
   {
      control <<= 9; /* all codes are at least 9 bits long */
      if((shift += 9) > 0)
      {
         control += *source++ << (8 + shift);
         control += *source++ << shift;
         shift -= 16;
      }
      if(control & 16777216)
      {
         *destination++ = quick_buffer[quick_local++ & 255] = control >> 16;
      }
      else
      {
         control <<= 2; /* 2 extra bits for length */
         if((shift += 2) > 0)
         {
            control += *source++ << (8 + shift);
            control += *source++ << shift;
            shift -= 16;
         }
         count = ((control >> 24) & 3) + 2;
         offset = quick_local - ((control >> 16) & 255) - 1;
         while((destination < destination_end) && (count--))
            *destination++ = quick_buffer[quick_local++ & 255] =
                             quick_buffer[offset++ & 255];
      }
   } /* while */

//if(DEBUG)printf(" ...quick %s",((source > source_end) || (destination != destination_end)) ? "bad" : "good");
   return((source > source_end) || (destination != destination_end));
}

/* --------------------------------------------------------------------- */

int crunch_medium(unsigned char *source, unsigned char *source_end,
                unsigned char *destination, unsigned char *destination_end)
{
   register unsigned int control = 0;
   register int shift = 0;
   int count, offset, temp;

   medium_local += 66; /* i have no idea why it adds 66 */
   if(!no_clear_flag)
      for(medium_local = count = 0; count < 16384; count++)
         medium_buffer[count] = 0;

   while((destination < destination_end) && (source < source_end))
   {
      control <<= 9; /* all codes are 9 bits long */
      if((shift += 9) > 0)
      {
         control += *source++ << (8 + shift);
         control += *source++ << shift;
         shift -= 16;
      }
      if((temp = (control >> 16) & 511) >= 256)
      {
         *destination++ = medium_buffer[medium_local++ & 16383] = temp;
      }
      else
      {
         count = table_one[temp] + 3;
         temp = table_two[temp];
         control <<= temp;
         if((shift += temp) > 0)
         {
            control += *source++ << (8 + shift);
            control += *source++ << shift;
            shift -= 16;
         }
         temp = (control >> 16) & 255;
         offset = table_one[temp] << 8;
         temp = table_two[temp];
         control <<= temp;
         if((shift += temp) > 0)
         {
            control += *source++ << (8 + shift);
            control += *source++ << shift;
            shift -= 16;
         }
         offset += (control >> 16) & 255;
         offset = medium_local - offset - 1;
         while((destination < destination_end) && (count--))
            *destination++ = medium_buffer[medium_local++ & 16383] =
                             medium_buffer[offset++ & 16383];
      }
   } /* while */

//if(DEBUG)printf(" ...medium %s",((source > source_end) || (destination != destination_end)) ? "bad" : "good");
   return((source > source_end) || (destination != destination_end));
}

/* --------------------------------------------------------------------- */

void deep_clear()
{
   uint16_t count = 0;
   uint16_t temp = 627;
   for(count = 0; count < 314; count++)
   {
      deep_weights[count] = 1;
      deep_symbols[count] = temp;
      deep_hash[temp] = count;
      temp++;
   }
   temp = 0;
   for(count = 314; count < 627; count++)
   {
      deep_weights[count] = deep_weights[temp] + deep_weights[temp + 1];
      deep_symbols[count] = temp;
      deep_hash[temp] = deep_hash[temp + 1] = count;
      temp += 2;
   }
   deep_weights[count] = 65535;
   deep_hash[temp] = 0;

//if(DEBUG)printf(" ...clear");
}

/* --------------------------------------------------------------------- */

void deep_scale()
{
   int symbol, swap, temp, weight;

   temp = 0;
   for(symbol = 0; symbol < 627; symbol++)
   {
      if(deep_symbols[symbol] >= 627)
      {
         deep_weights[temp] = (deep_weights[symbol] + 1) >> 1;
         deep_symbols[temp] = deep_symbols[symbol];
         temp++;
      }
   }
   temp = 0;
   for(symbol = 314; symbol < 627; symbol++)
   {
      weight = deep_weights[temp] + deep_weights[temp + 1];
      for(swap = symbol; deep_weights[swap - 1] > weight; swap--)
      {
        deep_weights[swap] = deep_weights[swap - 1];
        deep_symbols[swap] = deep_symbols[swap - 1];
      }
      deep_weights[swap] = weight;
      deep_symbols[swap] = temp;
      temp += 2;
   }
   for(symbol = 0; symbol < 627; symbol++)
   {
      temp = deep_symbols[symbol];
      deep_hash[temp] = symbol; if(temp < 627) deep_hash[temp + 1] = symbol;
   }

//if(DEBUG)printf(" ...scale");
}

/* --------------------------------------------------------------------- */

int crunch_deep(unsigned char *source, unsigned char *source_end,
                unsigned char *destination, unsigned char *destination_end)
{
   register unsigned int control = 0;
   register int shift = 0;
   int count, offset, temp, symbol, swap, temp1, temp2;

   deep_local += 60; /* i have no idea why it adds 60 */
   if(!no_clear_flag)
   {
      deep_clear();
      for(deep_local = count = 0; count < 16384; count++)
         deep_buffer[count] = 0;
   }

   while((destination < destination_end) && (source < source_end))
   {
      count = deep_symbols[626]; /* start from the root of the trie */
      do
      {
         if(!shift++)
         {
            control += *source++ << 8;
            control += *source++;
            shift = -15;
         }
         control <<= 1;
         count += (control >> 16) & 1;
      } while((count = deep_symbols[count]) < 627);

      if(deep_weights[626] == 32768) /* scale the trie if the weight gets too large */
         deep_scale();

      symbol = deep_hash[count];
      do
      {
         deep_weights[symbol]++; /* increase the weight of this node */
         if(deep_weights[symbol + 1] < deep_weights[symbol])
         {
            temp1 = deep_weights[(swap = symbol)];
            do
            {
               swap++;
            } while(deep_weights[swap + 1] < temp1);
            deep_weights[symbol] = deep_weights[swap];
            deep_weights[swap] = temp1;
            temp1 = deep_symbols[symbol];
            temp2 = deep_symbols[swap];
            deep_symbols[swap] = temp1;
            deep_symbols[symbol] = temp2;
            deep_hash[temp1] = swap; if(temp1 < 627) deep_hash[temp1 + 1] = swap;
            deep_hash[temp2] = symbol; if(temp2 < 627) deep_hash[temp2 + 1] = symbol;
            symbol = swap;
         }
      } while(symbol = deep_hash[symbol]); /* repeat until we reach root */

      if((count -= 627) < 256)
      {
         *destination++ = deep_buffer[deep_local++ & 16383] = count;
      }
      else
      {
         count -= 253; /* length is always at least 3 characters */
         control <<= 8;
         if((shift += 8) > 0)
         {
            control += *source++ << (8 + shift);
            control += *source++ << shift;
            shift -= 16;
         }
         temp = (control >> 16) & 255;
         offset = table_one[temp] << 8;
         temp = table_two[temp];
         control <<= temp;
         if((shift += temp) > 0)
         {
            control += *source++ << (8 + shift);
            control += *source++ << shift;
            shift -= 16;
         }
         offset += (control >> 16) & 255;
         offset = deep_local - offset - 1;
         while((destination < destination_end) && (count--))
            *destination++ = deep_buffer[deep_local++ & 16383] =
                             deep_buffer[offset++ & 16383];
      }
   } /* while */

//if(DEBUG)printf(" ...deep %s",((source > source_end) || (destination != destination_end)) ? "bad" : "good");
   return((source > source_end) || (destination != destination_end));
}

/* --------------------------------------------------------------------- */

int make_decode_table(int number_symbols, int table_size,
                      unsigned char *length, unsigned short *table)
{
   register unsigned char bit_num = 0;
   register int symbol;
   unsigned int table_mask, bit_mask, pos, fill, next_symbol, leaf;
   int abort = 0;

   pos = 0;
   fill = 0;
   bit_mask = table_mask = 1 << table_size;

   while((!abort) && (bit_num <= table_size))
   {
      for(symbol = 0; symbol < number_symbols; symbol++)
      {
         if(length[symbol] == bit_num)
         {
            if((pos += bit_mask) > table_mask)
            {
               abort = 1;
               break; /* we will overrun the table! abort! */
            }
            while(fill < pos)
               table[fill++] = symbol;
         }
      }
      bit_mask >>= 1;
      bit_num++;
   }

   if((!abort) && (pos != table_mask))
   {
      for(; fill < table_mask; fill++)
         table[fill] = 0; /* clear the rest of the table */
      next_symbol = table_mask >> 1;
      pos <<= 16;
      table_mask <<= 16;
      bit_mask = 32768;

      while((!abort) && (bit_num <= 18))
      {
         for(symbol = 0; symbol < number_symbols; symbol++)
         {
            if(length[symbol] == bit_num)
            {
               leaf = pos >> 16;
               for(fill = 0; fill < bit_num - table_size; fill++)
               {
                  if(table[leaf] == 0)
                  {
                     table[(next_symbol << 1)] = 0;
                     table[(next_symbol << 1) + 1] = 0;
                     table[leaf] = next_symbol++;
                  }
                  leaf = table[leaf] << 1;
                  leaf += (pos >> (15 - fill)) & 1;
               }
               table[leaf] = symbol;
               if((pos += bit_mask) > table_mask)
               {
                  abort = 1;
                  break; /* we will overrun the table! abort! */
               }
            }
         }
         bit_mask >>= 1;
         bit_num++;
      }
   }

//if(DEBUG)printf(" ...table %s",((pos != table_mask) || abort) ? "bad" : "good");
   return((pos != table_mask) || abort);
}

/* --------------------------------------------------------------------- */

int crunch_heavy(unsigned char *source, unsigned char *source_end,
                unsigned char *destination, unsigned char *destination_end,
                int flag, int special)
{
   register unsigned int control = 0;
   register int shift = 0;
   int count, offset, temp;

   if(!no_clear_flag) heavy_local = 0;

   if(flag)
   {
      flag = 0;

/* read the literal table */
      if(!flag)
      {
         for(count = 0; count < 512; count++)
            heavy_literal_len[count] = 255;
         control <<= 9; /* get number of literals */
         if((shift += 9) > 0)
         {
            control += *source++ << (8 + shift);
            control += *source++ << shift;
            shift -= 16;
         }
         if(offset = (control >> 16) & 511)
            for(count = 0; count < offset; count++)
            {
               control <<= 5; /* get the length of this literal */
               if((shift += 5) > 0)
               {
                  control += *source++ << (8 + shift);
                  control += *source++ << shift;
                  shift -= 16;
               }
               temp = (control >> 16) & 31;
               heavy_literal_len[count] = (temp ? temp : 255);
            }
         else
         {
            control <<= 9; /* get the defined literal */
            if((shift += 9) > 0)
            {
               control += *source++ << (8 + shift);
               control += *source++ << shift;
               shift -= 16;
            }
            heavy_literal_len[(control >> 16) & 511] = 0;
         }

         flag = make_decode_table(512, 12, heavy_literal_len, heavy_literal_table);
      }

/* read the offset table */
      if(!flag)
      {
         for(count = 0; count < 32; count++)
            heavy_offset_len[count] = 255;

         control <<= 5; /* get number of offsets */
         if((shift += 5) > 0)
         {
            control += *source++ << (8 + shift);
            control += *source++ << shift;
            shift -= 16;
         }
         if(offset = (control >> 16) & 31)
            for(count = 0; count < offset; count++)
            {
               control <<= 4; /* get the length of this offset */
               if((shift += 4) > 0)
               {
                  control += *source++ << (8 + shift);
                  control += *source++ << shift;
                  shift -= 16;
               }
               temp = (control >> 16) & 15;
               heavy_offset_len[count] = (temp ? temp : 255);
            }
         else
         {
            control <<= 5; /* get the defined offset */
            if((shift += 5) > 0)
            {
               control += *source++ << (8 + shift);
               control += *source++ << shift;
               shift -= 16;
            }
            heavy_offset_len[(control >> 16) & 31] = 0;
         }
         temp = heavy_offset_len[special];
         heavy_offset_len[special] = heavy_offset_len[31];
         heavy_offset_len[31] = temp;

         flag = make_decode_table(32, 8, heavy_offset_len, heavy_offset_table);
      }

   } /* if(flag) */

   if(!flag)
   {
/* prefetch 12 bits for fast huffman decoding */
      control <<= 12;
      if((shift += 12) > 0)
      {
         control += *source++ << (8 + shift);
         control += *source++ << shift;
         shift -= 16;
      }

      while((destination < destination_end) && (source < source_end))
      {

/* get a literal */
         if((count = heavy_literal_table[(control >> 16) & 4095]) >= 512)
         {
            do /* literal is longer than 12 bits */
            {
               if(!shift++)
               {
                  control += *source++ << 8;
                  control += *source++;
                  shift = -15;
               }
               control <<= 1;
               count = heavy_literal_table[((control >> 16) & 1) + (count << 1)];
            } while(count >= 512);
            temp = 12; /* skip the original 12 bits */
         }
         else
         {
            temp = heavy_literal_len[count];
         }
         control <<= temp;
         if((shift += temp) > 0)
         {
            control += *source++ << (8 + shift);
            control += *source++ << shift;
            shift -= 16;
         }

/* less than 256 = literal, otherwise = length of string */
         if(count < 256)
         {
            *destination++ = heavy_buffer[heavy_local++ & 8191] = count;
         }
         else /* must have been a string */
         {
            count -= 253; /* length is always at least 3 characters */
            if((offset = heavy_offset_table[(control >> 20) & 255]) >= 32)
            {
               do /* offset is longer than 8 bits */
               {
                  if(!shift++)
                  {
                     control += *source++ << 8;
                     control += *source++;
                     shift = -15;
                  }
                  control <<= 1;
                  offset = heavy_offset_table[((control >> 20) & 1) + (offset << 1)];
               } while(offset >= 32);
               temp = 8; /* skip the original 8 bits */
            }
            else
            {
               temp = heavy_offset_len[offset];
            }
            control <<= temp;
            if((shift += temp) > 0)
            {
               control += *source++ << (8 + shift);
               control += *source++ << shift;
               shift -= 16;
            }

            if(offset == 31)
            {
               offset = heavy_last_offset;
            }
            else
            {
               if(offset)
               {
                  temp = offset - 1;
                  offset = ((control & 268369920) | 268435456) >> (28 - temp);
                  control <<= temp;
                  if((shift += temp) > 0)
                  {
                     control += *source++ << (8 + shift);
                     control += *source++ << shift;
                     shift -= 16;
                  }
               }
               heavy_last_offset = offset;
            }
            offset = heavy_local - offset - 1;
            while((destination < destination_end) && (count--))
               *destination++ = heavy_buffer[heavy_local++ & 8191] =
                                heavy_buffer[offset++ & 8191];
         } /* if(string) */
      }
   } /* if(!flag) */

	//if(DEBUG)printf(" ...heavy %s",((source > source_end) || (destination != destination_end) || flag) ? "bad" : "good");
   return((source > source_end) || (destination != destination_end) || flag);
}

// access track from buffer: caller determines where it is regardless of how much in RAM (read or mapped)
//
uint8_t *CUnDMS::decrunch(const Track_Header *track, uint8_t *pack_buffer)
{
	uint8_t *unpack_buffer = m_DecrunchBuffer.GetAtCurrent();
	uint8_t *buffer = nullptr;
	
	switch(track->pack_mode) /* pack_mode */
	{
	case PACK_STORE: /* store */
		{
		if(!crunch_store(pack_buffer, pack_buffer + track->pack_size, unpack_buffer, unpack_buffer + track->unpack_size))
		buffer = unpack_buffer;
		break;
		}
	case PACK_RLE: /* rle */
		{
		if(!crunch_rle(pack_buffer, pack_buffer + track->pack_size, unpack_buffer, unpack_buffer + track->unpack_size))
		buffer = unpack_buffer;
		break;
		}
	case PACK_QUICK: /* quick */
		{
		if(!crunch_quick(pack_buffer, pack_buffer + track->pack_size + 16, unpack_buffer, unpack_buffer + track->rle_size))
			if(!crunch_rle(unpack_buffer, unpack_buffer + track->rle_size, pack_buffer, pack_buffer + track->unpack_size))
				buffer = pack_buffer;
		break;
		}
	case PACK_MEDIUM: /* medium */
		{
		if(!crunch_medium(pack_buffer, pack_buffer + track->pack_size + 16, unpack_buffer, unpack_buffer + track->rle_size))
			if(!crunch_rle(unpack_buffer, unpack_buffer + track->rle_size, pack_buffer, pack_buffer + track->unpack_size))
				buffer = pack_buffer;
		break;
		}
	case PACK_DEEP: /* deep */
		{
		if(!crunch_deep(pack_buffer, pack_buffer + track->pack_size + 16, unpack_buffer, unpack_buffer + track->rle_size))
			if(!crunch_rle(unpack_buffer, unpack_buffer + track->rle_size, pack_buffer, pack_buffer + track->unpack_size))
				buffer = pack_buffer;
		break;
		}
	case PACK_HEAVY1: /* heavy1 */
		{
		if(!crunch_heavy(pack_buffer, pack_buffer + track->pack_size + 16, unpack_buffer, unpack_buffer + track->rle_size, track->pack_flag & 2, 13))
		{
			if(track->isPackFlagHeavyRle()) /* pack_flag */
			{
			if(!crunch_rle(unpack_buffer, unpack_buffer + track->rle_size, pack_buffer, pack_buffer + track->unpack_size))
				buffer = pack_buffer;
			}
			else
				buffer = unpack_buffer;
		}
		break;
		}
	case PACK_HEAVY2: /* heavy2 */
		{
		if(!crunch_heavy(pack_buffer, pack_buffer + track->pack_size + 16, unpack_buffer, unpack_buffer + track->rle_size, track->pack_flag & 2, 14))
		{
			if(track->isPackFlagHeavyRle()) /* pack_flag */
			{
			if(!crunch_rle(unpack_buffer, unpack_buffer + track->rle_size, pack_buffer, pack_buffer + track->unpack_size))
				buffer = pack_buffer;
			}
			else
				buffer = unpack_buffer;
		}
		break;
		}
	default:
		// silence GCC..
		break;
	} /* switch */
	
	// keep collecting until done
	m_DecrunchBuffer.SetCurrentPos(m_DecrunchBuffer.GetCurrentPos() + track->unpack_size);

	return buffer;
}

bool CUnDMS::unpack()
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
	m_ReadBuffer.SetCurrentPos(4); // after identifier
	
	// just access directly until checking done
	uint8_t *archive_header = m_ReadBuffer.GetNext(52);
	m_header.parseBuf(archive_header);
	
	if (mycrc(archive_header, 50) != m_header.header_sum) /* header_sum */
	{
		throw IOException("Invalid CRC (archive header)");
	}
	if (m_header.extract_ver > ExtractVersion) /* extract_ver */
	{
		throw IOException("Need newer version..");
	}
	
	// DMS/ADF disk-images should be less than 1MB when unpacked,
	// we should have that much RAM -> simplify checks
	const int32_t max_image_size = (1*1024*1024);
	
	m_DecrunchBuffer.PrepareBuffer(max_image_size, false);
	
	// max 80 tracks allowed/supported
	high_track = (m_header.hightrack > 80) ? 80 : m_header.hightrack;
	
	if (m_tracks != nullptr)
	{
		delete[] m_tracks;
	}
	m_tracks = new Track_Header[m_header.hightrack]; // keep until processed
	
	// loop from here for each track? 
	for (int i = m_header.lowtrack; i < m_header.hightrack; i++)
	{
		Track_Header *track = &m_tracks[i];

		// use until checking done
		uint8_t *track_header = m_ReadBuffer.GetNext(20);
		if (track->isTR(track_header) == false)
		{
			throw IOException("Invalid track count");
		}
		// keep related info
		track->parseBuf(track_header);

		if (track->pack_mode < 0 || track->pack_mode >= 7)
		{
			throw IOException("Invalid packing mode");
		}
		if (track->pack_size > max_image_size || track->rle_size > max_image_size || track->unpack_size > max_image_size)
		{
			throw IOException("Invalid size for data");
		}
		if (mycrc(track_header, 18) != track->header_sum) /* header_sum */
		{
			throw IOException("Invalid CRC (track header)");
		}
		
		if (mycrc(m_ReadBuffer.GetAtCurrent(), track->pack_size) != track->data_sum) /* data_sum */
		{
			throw IOException("Invalid CRC (pack buffer)");
		}
		
		// check we still have enough space (if invalid guess before..)
		m_DecrunchBuffer.Reserve(track->unpack_size);
	
		// decrunch etc.
		uint8_t *decrbuf = decrunch(track, m_ReadBuffer.GetNext(track->pack_size));
		if (!decrbuf)
		{
			throw IOException("Extract failed");
		}
		if (mysimplecrc(decrbuf, track->unpack_size) != track->unpack_sum) /* unpack_sum */
		{
			throw IOException("CRC error (unpack data)");
		}
		
		if ((track->current_track < 1000) && (track->unpack_size >= 8192)) /* try and skip illegal tracks */
		{
			// unpacking of track expects this to be checked here it seems..
			if (track->current_track < high_track)
			{
				no_clear_flag = track->pack_flag & 1; // pack_flag 
			}
		
			// skip this, write entirely when done
			/*
			actual = fwrite(buffer, 1, unpack_size, out_file);
			if(actual == unpack_size)
			{
			 if(current_track < high_track)
			 {
			  no_clear_flag = track_header[12] & 1; // pack_flag 
			  //abort = 0; // continue 
			 }
			 else
				return true;
			  //result = 0; // successful completion 
			}
			*/
		}
		else
		{
			//qDebug() << "illegal track"
		}
	}
	
	// finally, output
	// (TODO: IOcontext later)
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


/* --------------------------------------------------------------------- */

/* Absolutely for the reader ONLY. This is basically hardcoded into */
/* the program. You don't like it? TS */

/* These structures are completely UNUSED by the program */

/* struct Info_Header
{
   LONG ID;             0  : 'DMS!'
} */ /* SIZE = 4 */

/* general purpose flags */

#define GENERAL_NOZERO    1<<0
#define GENERAL_ENCRYPT   1<<1
#define GENERAL_OPTIMIZED 1<<2
#define GENERAL_BANNER    1<<3

/* serial number of creator */

#define SERIAL_NUMBER 0    /* generic */

/* cpu type of creator */

#define CPU_MC68000   1
#define CPU_MC68010   2
#define CPU_MC68020   3
#define CPU_MC68030   4
#define CPU_MC68040   5
#define CPU_MC68050   6
#define CPU_IAPX86    7
#define CPU_IAPX88    8
#define CPU_IAPX188   9
#define CPU_IAPX186   10
#define CPU_IAPX286   11
#define CPU_IAPX386SX 12
#define CPU_IAPX386   13
#define CPU_IAPX486   14
#define CPU_IAPX586   15

/* coprocessor of creator */

#define COPRO_MC68881    1
#define COPRO_MC68882    2
#define COPRO_IAPX87     3
#define COPRO_IAPX287    4
#define COPRO_IAPX387SX  5
#define COPRO_IAPX387    6

/* machine type of creator */

#define MACHINE_AMIGA  1
#define MACHINE_PC     2
#define MACHINE_ATARI  3
#define MACHINE_MAC    4

/* disk types */

#define DISKTYPE_AMIGA1_OFS  1
#define DISKTYPE_AMIGA1_FFS  2
#define DISKTYPE_MSDOS       3
#define DISKTYPE_AMAX        4
#define DISKTYPE_MAC         5

#define DISKTYPE_INTL_OFS    3
#define DISKTYPE_INTL_FFS    4
#define DISKTYPE_CACHE_OFS   5
#define DISKTYPE_CACHE_FFS   6
#define DISKTYPE_FMS_DEVICE  7  /* Virtual disk device */

/* struct Archive_Header
{
   LONG extra;          0  : for future expansion
   LONG general;        4  : general purpose flags
   LONG timestamp;      8  : creation date of archive (seconds since 00:00:00 01-Jan-78)
   SHORT lowtrack;      12 : first track (0-85)
   SHORT hightrack;     14 : last track (0-85)
   LONG pack_size;      16 : total size of data after compression
   LONG unpack_size;    20 : total size of data before compression
   LONG serialno;       24 : serial number of creator
   SHORT cpu;           28 : CPU type of creator
   SHORT copro;         30 : CPU coprocessor of creator
   SHORT machine;       32 : machine of creator
   SHORT mextra;        34 : extra ID information (machine specific)
   SHORT speed;         36 : CPU speed (approx)
   LONG  timecreate;    38 : time to create archive
   SHORT create_ver;    42 : version of creator
   SHORT extract_ver;   44 : version needed to extract
   SHORT disktype;      46 : disk type of archive
   SHORT crunchmode;    48 : compression mode (generally) used
   SHORT header_sum;    50 : header CRC
} */ /* SIZE = 52 */

/* crunch mode */

#define CRUNCH_NOCOMP   0  /* Compression modes */
#define CRUNCH_RLE      1
#define CRUNCH_QUICK    2
#define CRUNCH_MEDIUM   3
#define CRUNCH_DEEP     4
#define CRUNCH_TESTPACK 5

#define CRUNCH_DLE   0x90

/* struct Track_Header
{
   SHORT ID;            0  : 'TR'
   SHORT track;         2  : track number -1=text
   SHORT flags;         4  : general flags
   SHORT pack_size;     6  : packed length
   SHORT rle_size;      8  : rle-packed size
   SHORT unpack_size;   10 : unpacked size
   BYTE pack_flag;      12 : general purpose compression flag
   BYTE pack_mode;      13 : compression mode
   SHORT unpack_sum;    14 : unpacked CRC
   SHORT data_sum;      16 : packed CRC
   SHORT header_sum;    18 : header CRC
} */ /* SIZE = 20 */

/* --------------------------------------------------------------------- */
