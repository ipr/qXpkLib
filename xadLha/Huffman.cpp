/*
 * Based on Unix LhA source code: lha-1.14i-ac20040929
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
 */


#include "Huffman.h"

#include <stdlib.h>


/////////// CHuffmanTree


/* ------------------------------------------------------------------------ */
/* LHa for UNIX                                                             */
/*              maketbl.c -- makes decoding table                           */
/*                                                                          */
/*      Modified                Nobutaka Watazaki                           */
/*                                                                          */
/*  Ver. 1.14   Source All chagned              1995.01.14  N.Watazaki      */
/* ------------------------------------------------------------------------ */

void CHuffmanTree::make_table(
    const int16_t	nchar,
    uint8_t			bitlen[],
    int16_t			tablebits,
    uint16_t		table[])
{
	// note: loops below are 1..<=16 -> need 17 cells
    uint16_t  count[17];  /* count of bitlen */
    uint16_t  weight[17]; /* 0x10000ul >> bitlen */
    uint16_t  start[17];  /* first code of bitlen */
    uint32_t  iCount = 0;

    /* initialize */
	::memset(count, 0, sizeof(uint16_t)*17);
    for (iCount = 1; iCount <= 16; iCount++) 
	{
        weight[iCount] = 1 << (16 - iCount);
    }

    /* count */
    for (iCount = 0; iCount < nchar; iCount++)
	{
        count[bitlen[iCount]]++;
	}

    /* calculate first code */
    uint16_t total = 0;
    for (iCount = 1; iCount <= 16; iCount++) 
	{
        start[iCount] = total;
        total += weight[iCount] * count[iCount];
    }
    if ((total & 0xffff) != 0)
	{
        //error("make_table(): Bad table (5)");
		throw ArcException("make_table(): Bad table (5)", total);
	}

    /* shift data for make table. */
    int m = 16 - tablebits;
    for (iCount = 1; iCount <= tablebits; iCount++) 
	{
        start[iCount] >>= m;
        weight[iCount] >>= m;
    }

    /* initialize */
    int start_m = start[tablebits + 1] >> m;
    if (start_m != 0)
	{
		int kEnd = 1 << tablebits;
        for (int i = start_m; i < kEnd; i++)
		{
            table[i] = 0;
		}
	}

    int avail = nchar;

    /* create table and tree */
    for (int j = 0; j < nchar; j++) 
	{
		// unconditional set -> make scoped
        int k = bitlen[j];
        if (k == 0)
		{
            continue;
		}
		
        unsigned int l = start[k] + weight[k];
        if (k <= tablebits) 
		{
            /* code in table */
            for (unsigned int i = start[k]; i < l; i++)
			{
                table[i] = j;
			}
        }
        else 
		{
            /* code not in table */
			unsigned int i = start[k];
			
            uint16_t *pTbl = &table[i >> m];
            i <<= tablebits;
			
            int n = k - tablebits;
			
            /* make tree (n length) */
			make_table_tree(n, j, i, pTbl, avail);
        }
        start[k] = l;
    }
}

// split from above: this is called in loop from make_table(),
// only part in make_table() that uses left[] and right[] arrays
//
void CHuffmanTree::make_table_tree(const int treeLen, const int j, unsigned int &i, uint16_t *pTbl, int &avail)
{
	/* make tree (n length) */
	int nLen = treeLen;
    while (--nLen >= 0) 
	{
        if ((*pTbl) == 0) 
		{
            right[avail] = left[avail] = 0;
            (*pTbl) = avail++;
        }
        if (i & 0x8000)
		{
            pTbl = &right[(*pTbl)];
		}
        else
		{
            pTbl = &left[(*pTbl)];
		}
        i <<= 1;
    }
    (*pTbl) = j;
}


//// CShuffleHuffman


/* ------------------------------------------------------------------------ */
/* LHa for UNIX                                                             */
/*              shuf.c -- extract static Huffman coding                     */
/*                                                                          */
/*      Modified                Nobutaka Watazaki                           */
/*                                                                          */
/*  Ver. 1.14   Source All chagned              1995.01.14  N.Watazaki      */
/* ------------------------------------------------------------------------ */


// these lookup-tables only used by -lh1- and -lh2-
const int CShuffleHuffman::fixed_method_lh1[16] = {3, 0x01, 0x04, 0x0c, 0x18, 0x30, 0};   // old compatible 
const int CShuffleHuffman::fixed_method_lh3[16] = {2, 0x01, 0x01, 0x03, 0x06, 0x0D, 0x1F, 0x4E, 0};    // 8K buf 


void CShuffleHuffman::fixed_method_pt_len(const int *tbl)
{
    //const int *tbl = fixed[method];
    int j = (*tbl);
	tbl++;
	
    unsigned int weight = 1 << (16 - j);
    unsigned int code = 0;
	
    for (int i = 0; i < m_np; i++) 
	{
        while ((*tbl) == i) 
		{
            j++;
            tbl++;
            weight >>= 1;
        }
        pt_len[i] = j;
        code += weight;
    }
}

/* ------------------------------------------------------------------------ */
/* lh1 */
void CShuffleHuffman::decode_start_fix()
{
	m_BitIo.init_getbits();

	// -lh1- shift by 6 for table configuration
    //m_np = 1 << (LZHUFF1_DICBIT - 6);
    m_np = SHUF_NP_LZHUFF1;
	
	CDynamicHuffman::init_num_max(SHUF_NUM_MAX_LH1);
    start_c_dyn(SHUF_MAXMATCH_LH1);

    fixed_method_pt_len(fixed_method_lh1);
    make_table(m_np, pt_len, 8, pt_table);
}

/* ------------------------------------------------------------------------ */
/* lh3 */
void CShuffleHuffman::decode_start_st0()
{
	m_BitIo.init_getbits();
	CDynamicHuffman::init_num_max(SHUF_NUM_MAX_LH3);
	
	// -lh3- shift by 7 
    //m_np = 1 << (LZHUFF3_DICBIT - 6);
    m_np = SHUF_NP_LZHUFF3;
}

/* ------------------------------------------------------------------------ */
/* lh3 */
uint16_t CShuffleHuffman::decode_c_st0()
{
    if (m_blocksize == 0)    /* read block head */
	{
        m_blocksize = m_BitIo.getbits(SHUF_BUFBITS);   /* read block blocksize */
		
        read_tree_c();
		
		uint16_t bit = m_BitIo.getbits(1);
        if (bit) 
		{
            read_tree_p();
        }
        else 
		{
		    fixed_method_pt_len(fixed_method_lh3);
        }
        make_table(PT_LEN_SIZE, pt_len, 8, pt_table);
    }
    m_blocksize--;
	
	uint16_t bit = m_BitIo.peekbits(12);
    int j = c_table[bit];
    if (j < SHUF_N1)
	{
        m_BitIo.fillbuf(c_len[j]);
	}
    else 
	{
        m_BitIo.fillbuf(12);
        
		// use inlinable helper..        
        shuf_decode_bitbuf(j, m_BitIo.bitbuf, SHUF_N1);
        
        m_BitIo.fillbuf(c_len[j] - 12);
    }
    
    if (j == SHUF_N1 - 1)
	{
        j += m_BitIo.getbits(SHUF_EXTRABITS);
	}
    return j;
}

/* ------------------------------------------------------------------------ */
/* lh1, 3 */
uint16_t CShuffleHuffman::decode_p_st0()
{
	uint16_t bit = m_BitIo.peekbits(8);
    int j = pt_table[bit];
    if (j < m_np) 
	{
        m_BitIo.fillbuf(pt_len[j]);
    }
    else 
	{
        m_BitIo.fillbuf(8);

		// use inlinable helper..        
        shuf_decode_bitbuf(j, m_BitIo.bitbuf, m_np);
        
        m_BitIo.fillbuf(pt_len[j] - 8);
    }
    return (j << 6) + m_BitIo.getbits(6);
}

void CShuffleHuffman::shuf_decode_bitbuf(int &j, const int16_t bitbuf, const int nCount)
{
	int16_t i = bitbuf;
	do 
	{
		if (i < 0)
		{
			j = right[j];
		}
		else
		{
			j = left[j];
		}
		i <<= 1;
	} while (j >= nCount);
}

void CShuffleHuffman::read_tree_c()
{
	/* read tree from file */
    int i = 0;
    while (i < SHUF_N1) 
	{
		uint16_t bit = m_BitIo.getbits(1);
        if (bit)
		{
            c_len[i] = m_BitIo.getbits(SHUF_LENFIELD) + 1;
		}
        else
		{
            c_len[i] = 0;
		}
		
		// if this byte-combination is found, zeroize lengths,
		// set next bits to table and end loop
        if (++i == 3 && c_len[0] == 1 && c_len[1] == 1 && c_len[2] == 1) 
		{
            // zeroize byte-array
            // part of it? (size: NC_LEN)
            ::memset(c_len, 0, SHUF_N1);
            // TODO: clear entirely..?
            //::memset(c_len, 0, C_LEN_SIZE);

			// set next bits to table elements
            uint16_t c = m_BitIo.getbits(CBIT);
            bufferSet(c, c_table, C_TABLE_LEN);
            return;
        }
    }
    make_table(SHUF_N1, c_len, 12, c_table);
}

void CShuffleHuffman::read_tree_p()
{
	/* read tree from file */
    int i = 0;
    while (i < PT_LEN_SIZE) 
	{
        pt_len[i] = m_BitIo.getbits(SHUF_LENFIELD);

		// if this byte-combination is found, zeroize lengths,
		// set next bits to table and end loop
        if (++i == 3 && pt_len[0] == 1 && pt_len[1] == 1 && pt_len[2] == 1) 
		{
            // zeroize byte-array
            ::memset(pt_len, 0, PT_LEN_SIZE);
            
            uint16_t c = m_BitIo.getbits(LZHUFF3_DICBIT - 6);
            bufferSet(c, pt_table, PT_TABLE_LEN);
            return;
        }
    }
}


//// CDynamicHuffman

/* ------------------------------------------------------------------------ */
/* LHa for UNIX                                                             */
/*              dhuf.c -- Dynamic Hufffman routine                          */
/*                                                                          */
/*      Modified                H.Yoshizaki                                 */
/*                                                                          */
/*  Ver. 1.14   Source All chagned              1995.01.14  N.Watazaki      */
/* ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------ */
/* lh2 */
void CDynamicHuffman::decode_start_dyn(const tHuffBits enBit)
{
	m_BitIo.init_getbits();
	
	CDynamicHuffman::init_num_max(DYNH_NUM_MAX_LH2);
    start_c_dyn(DYNH_MAXMATCH_LH2); // shared with -lh1-
	
    freq[ROOT_P] = 1;
    child[ROOT_P] = ~(N_CHAR);
    s_node[N_CHAR] = ROOT_P;
    block[ROOT_P] = stock[avail++];
    edge[(block[ROOT_P])] = ROOT_P;
    most_p = ROOT_P;
    total_p = 0;

	// keep m_nn for: decode_p_dyn()
	m_nn = (1 << (int)enBit);
    m_nextcount = 64;
}

/* ------------------------------------------------------------------------ */
/* lh1, 2 */
uint16_t CDynamicHuffman::decode_c_dyn()
{
    int c = child[ROOT_C];
    int16_t buf = m_BitIo.bitbuf;
    int16_t cnt = 0;
    do 
	{
        c = child[c - (buf < 0)];
        buf <<= 1;
        if (++cnt == 16) 
		{
            m_BitIo.fillbuf(16);
            buf = m_BitIo.bitbuf;
            cnt = 0;
        }
    } while (c > 0);
	
    m_BitIo.fillbuf(cnt);
    c = ~c;
    dyn_update_c(c);
	
    if (c == m_n1)
	{
        c += m_BitIo.getbits(8);
	}
    return c;
}

/* ------------------------------------------------------------------------ */
/* lh2 */
uint16_t CDynamicHuffman::decode_p_dyn(size_t &decode_count)
{
    while (decode_count > m_nextcount) 
	{
        make_new_node(m_nextcount / 64);
        m_nextcount += 64;
        if (m_nextcount >= m_nn)
		{
            m_nextcount = 0xffffffff;
		}
    }
	
    int c = child[ROOT_P];
    int16_t buf = m_BitIo.bitbuf;
    int16_t cnt = 0;
    while (c > 0) 
	{
        c = child[c - (buf < 0)];
        buf <<= 1;
        if (++cnt == 16) 
		{
            m_BitIo.fillbuf(16);
            buf = m_BitIo.bitbuf;
            cnt = 0;
        }
    }
	
    m_BitIo.fillbuf(cnt);
    c = (~c) - N_CHAR;
    dyn_update_p(c);

    return (c << 6) + m_BitIo.getbits(6);
}

/* ------------------------------------------------------------------------ */

void CDynamicHuffman::start_c_dyn(const uint16_t maxmatch)
{
	// keep m_n1 for: decode_c_dyn()
    m_n1 = (n_max >= 256 + maxmatch - THRESHOLD + 1) ? 512 : n_max - 1;
    for (int n = 0; n < TREESIZE_C; n++) 
	{
        stock[n] = n;
        block[n] = 0;
    }
	
    int i, j;
    for (i = 0, j = n_max * 2 - 2; i < n_max; i++, j--) 
	{
        freq[j] = 1;
        child[j] = ~i;
        s_node[i] = j;
        block[j] = 1;
    }
	
    avail = 2;
    edge[1] = n_max - 1;
    i = n_max * 2 - 2;
	
    while (j >= 0) 
	{
        int f = freq[j] = freq[i] + freq[i - 1];
        child[j] = i;
        parent[i] = parent[i - 1] = j;
        if (f == freq[j + 1]) 
		{
            edge[block[j] = block[j + 1]] = j;
        }
        else 
		{
            edge[block[j] = stock[avail++]] = j;
        }
        i -= 2;
        j--;
    }
}

void CDynamicHuffman::reconst(int start, int end)
{
    int i, j;
	int b;

    for (i = j = start; i < end; i++) 
	{
		int k = child[i];
        if (k < 0) 
		{
            freq[j] = (freq[i] + 1) / 2;
            child[j] = k;
            j++;
        }
		b = block[i];
        if (edge[b] == i) 
		{
            stock[--avail] = b;
        }
    }
    j--;
    i = end - 1;
	
    int l = end - 2;
	
    while (i >= start) 
	{
        while (i >= l) 
		{
            freq[i] = freq[j];
            child[i] = child[j];
            i--, j--;
        }
		
        unsigned int f = freq[l] + freq[l + 1];
		
		int k = 0;
        for (k = start; f < freq[k]; k++); // note: empty loop, locate..
		
        while (j >= k) 
		{
            freq[i] = freq[j];
            child[i] = child[j];
            i--, j--;
        }
		
        freq[i] = f;
        child[i] = l + 1;
        i--;
        l -= 2;
    }
	
    unsigned int f = 0;
    for (i = start; i < end; i++) 
	{
		j = child[i];
        if (j < 0)
		{
            s_node[~j] = i;
		}
        else
		{
            parent[j] = parent[j - 1] = i;
		}
		
        if (f == freq[i]) 
		{
            block[i] = b;
        }
        else 
		{
			b = block[i] = stock[avail++];
            edge[b] = i;
            f = freq[i];
        }
    }
}

int CDynamicHuffman::swap_inc(int p)
{
    int b = block[p];
	int q = edge[b]; 
    if (q != p)
	{
		/* swap for leader */
        int r = child[p];
        int s = child[q];
        child[p] = s;
        child[q] = r;
		
        if (r >= 0)
		{
            parent[r] = parent[r - 1] = q;
		}
        else
		{
            s_node[~r] = q;
		}
		
        if (s >= 0)
		{
            parent[s] = parent[s - 1] = p;
		}
        else
		{
            s_node[~s] = p;
		}
        p = q;
		
		swap_inc_Adjust(p, b);
    }
    else if (b == block[p + 1]) 
	{
		swap_inc_Adjust(p, b);
    }
    else if (++freq[p] == freq[p - 1]) 
	{
        stock[--avail] = b; /* delete block */
        block[p] = block[p - 1];
    }
    return parent[p];
}

// was a goto-segment..
void CDynamicHuffman::swap_inc_Adjust(int &p, int &b)
{
	edge[b]++;
	if (++freq[p] == freq[p - 1]) 
	{
		block[p] = block[p - 1];
	}
	else 
	{
		/* create block */
		block[p] = stock[avail++];
		edge[(block[p])] = p;
	}
}

void CDynamicHuffman::dyn_update_c(int p)
{
    if (freq[ROOT_C] == 0x8000) 
	{
        reconst(0, n_max * 2 - 1);
    }
    freq[ROOT_C]++;

    int q = s_node[p];
    do 
	{
        q = swap_inc(q);
    } while (q != ROOT_C);
}

void CDynamicHuffman::dyn_update_p(int p)
{
    if (total_p == 0x8000) 
	{
        reconst(ROOT_P, most_p + 1);
        total_p = freq[ROOT_P];
        freq[ROOT_P] = 0xffff;
    }
	
    int q = s_node[p + N_CHAR];
    while (q != ROOT_P) 
	{
        q = swap_inc(q);
    }
    total_p++;
}

void CDynamicHuffman::make_new_node(int p)
{
    int r = most_p + 1;
    int q = r + 1;
	
	child[r] = child[most_p];
    s_node[~(child[r])] = r;
    child[q] = ~(p + N_CHAR);
    child[most_p] = q;
    freq[r] = freq[most_p];
    freq[q] = 0;
    block[r] = block[most_p];
	
    if (most_p == ROOT_P) 
	{
        freq[ROOT_P] = 0xffff;
        edge[(block[ROOT_P])]++;
    }
	
    parent[r] = parent[q] = most_p;
    block[q] = stock[avail++];
    edge[(block[q])] = s_node[p + N_CHAR] = most_p = q;
    dyn_update_p(p);
}


//// CStaticHuffman


/* ------------------------------------------------------------------------ */
/* LHa for UNIX                                                             */
/*              huf.c -- new static Huffman                                 */
/*                                                                          */
/*      Modified                Nobutaka Watazaki                           */
/*                                                                          */
/*  Ver. 1.14   Source All chagned              1995.01.14  N.Watazaki      */
/*  Ver. 1.14i  Support LH7 & Bug Fixed         2000.10. 6  t.okamoto       */
/* ------------------------------------------------------------------------ */


void CStaticHuffman::read_pt_len(int16_t nn, int16_t nbit, int16_t i_special)
{
    int16_t n = m_BitIo.getbits(nbit);
    if (n == 0) 
	{
        // zeroize byte-array
        // only part of it?
        // (size: PT_LEN_SIZE)
        ::memset(pt_len, 0, nn);

		// set table elements to next bits
        uint16_t c = m_BitIo.getbits(nbit);
		bufferSet(c, pt_table, PT_TABLE_LEN);
        return; // nothing more to do here
    }
    else 
	{
        int i = 0;
        while (i < n) 
		{
            int c = m_BitIo.peekbits(3);
            if (c != 7)
			{
                m_BitIo.fillbuf(3);
			}
            else 
			{
                uint16_t mask = 1 << (16 - 4);
                while (mask & m_BitIo.bitbuf) 
				{
                    mask >>= 1;
                    c++;
                }
                m_BitIo.fillbuf(c - 3);
            }

            pt_len[i++] = c;
            if (i == i_special) 
			{
                c = m_BitIo.getbits(2);
                while (--c >= 0)
				{
                    pt_len[i++] = 0;
				}
            }
        }
        
        // clear only part of the array??
        while (i < nn)
		{
            pt_len[i++] = 0;
		}
        make_table(nn, pt_len, 8, pt_table);
    }
}

void CStaticHuffman::read_c_len()
{
    int16_t n = m_BitIo.getbits(CBIT);
    if (n == 0) 
	{
		// zeroize byte-array..
		::memset(c_len, 0, C_LEN_SIZE); 

		// set table elements to next bits
        uint16_t c = m_BitIo.getbits(CBIT);
        bufferSet(c, c_table, C_TABLE_LEN);
        return; // nothing more to do here
    } 
	else 
	{
        int16_t i = 0;
        while (i < n) 
		{
			uint16_t bit = m_BitIo.peekbits(8);
			
            // note: needs to be signed here..
            int16_t c = pt_table[bit];
            if (c >= NT_LEN) 
			{
				// reduce duplication, use inline-helper
				// TODO: check, here c needs to be signed, how is other cases?
				//decode_mask_bitbuf(c, NT_LEN, (16 - 9));
				
				uint16_t mask = (1 << (16 - 9));
				do 
				{
					if (m_BitIo.bitbuf & mask)
					{
						c = right[c];
					}
					else
					{
						c = left[c];
					}
					mask >>= 1;
				} while (c >= NT_LEN);
            }
			
            m_BitIo.fillbuf(pt_len[c]);
			
            if (c <= 2) 
			{
                if (c == 0)
				{
                    c = 1;
				}
                else if (c == 1)
				{
                    c = m_BitIo.getbits(4) + 3;
				}
                else
				{
                    c = m_BitIo.getbits(CBIT) + 20;
				}
                while (--c >= 0)
				{
                    c_len[i++] = 0;
				}
            }
            else
			{
                c_len[i++] = c - 2;
			}
        }
		
        while (i < C_LEN_SIZE)
		{
            c_len[i++] = 0;
		}
        make_table(C_LEN_SIZE, c_len, 12, c_table);
    }
}

/* ------------------------------------------------------------------------ */
/* lh4, 5, 6, 7 */
uint16_t CStaticHuffman::decode_c_st1()
{
    if (m_blocksize == 0) 
	{
        m_blocksize = m_BitIo.getbits(16);
        read_pt_len(NT_LEN, 5, 3);
        read_c_len();
        read_pt_len(m_np_dict, m_dict_bit, -1);
    }
    m_blocksize--;
	
	uint16_t bit = m_BitIo.peekbits(12);
    uint16_t j = c_table[bit];
    if (j < C_LEN_SIZE)
	{
        m_BitIo.fillbuf(c_len[j]);
	}
    else 
	{
        m_BitIo.fillbuf(12);
		
		decode_mask_bitbuf(j, C_LEN_SIZE, (16 - 1));
		
        m_BitIo.fillbuf(c_len[j] - 12);
    }
    return j;
}

/* ------------------------------------------------------------------------ */
/* lh4, 5, 6, 7 */
uint16_t CStaticHuffman::decode_p_st1()
{
	uint16_t bit = m_BitIo.peekbits(8);
    uint16_t j = pt_table[bit];
    if (j < m_np_dict)
	{
        m_BitIo.fillbuf(pt_len[j]);
	}
    else 
	{
        m_BitIo.fillbuf(8);
		
		decode_mask_bitbuf(j, m_np_dict, (16 - 1));
		
        m_BitIo.fillbuf(pt_len[j] - 8);
    }
	
    if (j != 0)
	{
        j = (1 << (j - 1)) + m_BitIo.getbits(j - 1);
	}
    return j;
}

/* ------------------------------------------------------------------------ */
/* lh4, 5, 6, 7 */
void CStaticHuffman::decode_start_st1(const tHuffBits enBit)
{
	bool bRet = false;
    switch (enBit) 
	{
    case LZHUFF4_DICBIT:
    case LZHUFF5_DICBIT: 
		m_dict_bit = 4; 
		m_np_dict = LZHUFF5_DICBIT + 1; 
		bRet = true; // supported
		break;
		
    case LZHUFF6_DICBIT: 
		m_dict_bit = 5; 
		m_np_dict = LZHUFF6_DICBIT + 1; 
		bRet = true; // supported
		break;
		
    case LZHUFF7_DICBIT: 
		m_dict_bit = 5; 
		m_np_dict = LZHUFF7_DICBIT + 1; 
		bRet = true; // supported
		break;
		
	default: // silence some compilers
		break;
    }

	// unknown/unsupport here
	if (bRet == false)
	{
		throw ArcException("Cannot use dictionary bytes", (int)enBit);
	}
	
    m_BitIo.init_getbits();
    m_blocksize = 0;
}

// reduce duplication
void CStaticHuffman::decode_mask_bitbuf(uint16_t &j, const int nCount, const int masksize)
{
	uint16_t mask = (1 << masksize);
	do 
	{
		if (m_BitIo.bitbuf & mask)
		{
			j = right[j];
		}
		else
		{
			j = left[j];
		}
		mask >>= 1;
	} while (j >= nCount);
}

