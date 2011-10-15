//////////////////////////////////////
//
// UnLzx.cpp
//
// Ilkka Prusi
// ilkka.prusi@gmail.com
//
// Based on: 
// unlzx.c 1.1 (03.4.01, Erik Meusel)
//

//#include "stdafx.h"
#include "UnLzx.h"


// pre-created decoding tables
const unsigned char CLzxDecoder::table_one[32]=
{
 0,0,0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9,10,10,11,11,12,12,13,13,14,14
};

const unsigned int CLzxDecoder::table_two[32]=
{
 0,1,2,3,4,6,8,12,16,24,32,48,64,96,128,192,256,384,512,768,1024,
 1536,2048,3072,4096,6144,8192,12288,16384,24576,32768,49152
};

const unsigned int CLzxDecoder::table_three[16]=
{
 0,1,3,7,15,31,63,127,255,511,1023,2047,4095,8191,16383,32767
};

const unsigned char CLzxDecoder::table_four[34]=
{
 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,
 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16
};


/* Build a fast huffman decode table from the symbol bit lengths.         */
/* There is an alternate algorithm which is faster but also more complex. */
int CLzxDecoder::make_decode_table(int number_symbols, int table_size, unsigned char *length, unsigned short *table)
{
	unsigned char bit_num = 1; // truly start at 1 (skip one increment..)
	unsigned int pos = 0; /* consistantly used as the current position in the decode table */
	unsigned int table_mask = (1 << table_size);
	unsigned int bit_mask = (table_mask >> 1); // don't do the first number (note: may zero bits in shifting)

	while (bit_num <= table_size)
	{
		for(int symbol = 0; symbol < number_symbols; symbol++)
		{
			if(length[symbol] == bit_num)
			{
				/* reverse the order of the position's bits */
				unsigned int leaf = reverse_position(table_size, pos);
				
				if((pos += bit_mask) > table_mask)
				{
					//throw IOException("overrun table!");
					return 1; /* we will overrun the table! abort! */
				}
				
				unsigned int fill = bit_mask;
				unsigned int next_symbol = 1 << bit_num;
				do
				{
					table[leaf] = symbol;
					leaf += next_symbol;
				} while(--fill);
			}
		}
		bit_mask >>= 1;
		bit_num++;
	}

	if (pos != table_mask)
	{
		for(int symbol = pos; symbol < table_mask; symbol++) /* clear the rest of the table */
		{
			/* reverse the order of the position's bits */
			unsigned int leaf = reverse_position(table_size, symbol);
			table[leaf] = 0;
		}
		
		unsigned int next_symbol = table_mask >> 1;
		pos <<= 16;
		table_mask <<= 16;
		
		// unconditional set value -> make scoped
		unsigned int bit_mask = 32768;

		while (bit_num <= 16)
		{
			for(int symbol = 0; symbol < number_symbols; symbol++)
			{
				if(length[symbol] == bit_num)
				{
					/* reverse the order of the position's bits */
					unsigned int leaf = reverse_position(table_size, pos >> 16);
					for(unsigned int fill = 0; fill < bit_num - table_size; fill++)
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
						//throw IOException("overrun table incomplete!");
						return 1; /* we will overrun the table! abort! */
					}
				}
			}
			bit_mask >>= 1;
			bit_num++;
		}
	}

	if (pos != table_mask) 
	{
		// TODO: throw exception instead?
		//throw IOException("table is incomplete!");
		return 1; /* the table is incomplete! */
	}
	return 0;
}


/* Read and build the decrunch tables. There better be enough data in the */
/* source buffer or it's stuffed. */
int CLzxDecoder::read_literal_table(unsigned int &control, int &shift, unsigned int &decrunch_method, unsigned int &decrunch_length)
{
	/* fix the control word if necessary */
	if (shift < 0)
	{
		fix_shift_control_word(shift, control);
	}

	/* read the decrunch method */
	read_decrunch_method(shift, control, decrunch_method);

	/* Read and build the offset huffman table */
	if(decrunch_method == 3)
	{
		if (read_build_offset_table(shift, control) != 0)
		{
			return 1;
		}
	}

	/* read decrunch length */
	read_decrunch_length(shift, control, decrunch_length);

	// note: no need to continue -> cleanup below
	if (decrunch_method == 1)
	{
		return 0;
	}

	/* read and build the huffman literal table */
	if (decrunch_method != 1)
	{
		// only used in this scope..
		unsigned int pos = 0;
		unsigned int fix = 1;
		unsigned int max_symbol = 256;

		unsigned int count = 0;
		
		do
		{
			build_huffman_len_table(shift, control);
			
			if (make_decode_table(20, 6, huffman20_len, huffman20_table) != 0)
			{
				//throw IOException("table is corrupt!");
				//break; /* argh! table is corrupt! */
				return 1;
			}

			do
			{
				unsigned int temp = 0;

				unsigned int symbol = huffman20_table[control & 63];
				if(symbol >= 20)
				{
					symbol_longer_than_6_bits(shift, control, symbol);
					temp = 6;
				}
				else
				{
					temp = huffman20_len[symbol];
				}
				
				control >>= temp;
				if((shift -= temp) < 0)
				{
					fix_shift_control_word(shift, control);
				}

				switch(symbol)
				{
				case 17:
				case 18:
				{
					if(symbol == 17)
					{
						temp = 4;
						count = 3;
					}
					else /* symbol == 18 */
					{
						temp = 6 - fix;
						count = 19;
					}
					
					count += (control & table_three[temp]) + fix;
					control >>= temp;
					if((shift -= temp) < 0)
					{
						fix_shift_control_word(shift, control);
					}
					
					while((pos < max_symbol) && (count--))
					{
						literal_len[pos++] = 0;
					}
					break;
				} // case 17, case 18

				case 19:
				{
					count = (control & 1) + 3 + fix;
					if(!shift--)
					{
						fix_shift_control_long(shift, control);
					}
					
					control >>= 1;
					symbol = huffman20_table[control & 63];
					if(symbol >= 20)
					{
						symbol_longer_than_6_bits(shift, control, symbol);
						temp = 6;
					}
					else
					{
						temp = huffman20_len[symbol];
					}
					
					control >>= temp;
					if((shift -= temp) < 0)
					{
						fix_shift_control_word(shift, control);
					}
					
					symbol = table_four[literal_len[pos] + 17 - symbol];
					while((pos < max_symbol) && (count--))
					{
						literal_len[pos++] = symbol;
					}
					break;
				} // case 19

				default:
				{
					symbol = table_four[literal_len[pos] + 17 - symbol];
					literal_len[pos++] = symbol;
					break;
				} // default
				} // switch
			} while(pos < max_symbol);
			
			fix--;
			max_symbol += 512;
		} while(max_symbol == 768);

		if (make_decode_table(768, 12, literal_len, literal_table) != 0)
		{
			return 1;
		}
	}

	return 0;
}

/* Fill up the decrunch buffer. Needs lots of overrun for both destination */
/* and source buffers. Most of the time is spent in this routine so it's  */
/* pretty damn optimized. */
void CLzxDecoder::decrunch(unsigned int &control, int &shift, unsigned int &last_offset, const unsigned int &decrunch_method)
{
	do
	{
		unsigned int symbol = literal_table[control & 4095];
		if(symbol >= 768)
		{
			control >>= 12;
			if((shift -= 12) < 0)
			{
				fix_shift_control_word(shift, control);
			}
			
			/* literal is longer than 12 bits */
			do
			{
				symbol = literal_table[(control & 1) + (symbol << 1)];
				if(!shift--)
				{
					fix_shift_control_long(shift, control);
				}
				control >>= 1;
			} while(symbol >= 768);
		}
		else
		{
			unsigned int temp = literal_len[symbol];
			control >>= temp;
			if((shift -= temp) < 0)
			{
				fix_shift_control_word(shift, control);
			}
		}

		if(symbol < 256)
		{
			*destination++ = symbol;
		}
		else
		{
			// must make copy of previous n symbols to decrunch-buffer?

			symbol -= 256;
			
			unsigned int temp = symbol & 31;
			unsigned int count = table_two[temp];
			temp = table_one[temp];
			if((temp >= 3) && (decrunch_method == 3))
			{
				temp -= 3;
				count += ((control & table_three[temp]) << 3);
				control >>= temp;
				if((shift -= temp) < 0)
				{
					fix_shift_control_word(shift, control);
				}
				temp = offset_table[control & 127];
				count += temp;
				temp = offset_len[temp];
			}
			else
			{
				count += control & table_three[temp];
				if(!count)
				{
					count = last_offset;
				}
			}
			
			control >>= temp;
			if((shift -= temp) < 0)
			{
				fix_shift_control_word(shift, control);
			}
			last_offset = count;

			temp = (symbol >> 5) & 15;
			count = table_two[temp] + 3;
			temp = table_one[temp];
			count += (control & table_three[temp]);
			control >>= temp;
			
			if((shift -= temp) < 0)
			{
				fix_shift_control_word(shift, control);
			}

			copy_decrunch_string(last_offset, count);
		}
	} while((destination < destination_end) && (source < source_end));
}

///////////// CUnLzx

// create path and open file for writing
void CUnLzx::PrepareEntryForWriting(CArchiveEntry &Entry, CAnsiFile &OutFile)
{
	// this should have proper path-ending already..
	std::string szTempPath = m_szExtractionPath;
	szTempPath += Entry.m_szFileName;

	if (CPathHelper::MakePathToFile(szTempPath) == false)
	{
		// ignore only?
		// -> should fail when creating file there..
	}

	// open for writing
	if (OutFile.Open(szTempPath, true) == false)
	{
		// failed to open for writing
		// -> exception (critical error)
		throw ArcException("FOpen(Out)", szTempPath);
	}
}

void CUnLzx::OpenArchiveFile(CAnsiFile &ArchiveFile)
{
	if (ArchiveFile.Open(m_szArchive) == false)
	{
		throw IOException("FOpen(Archive)");
	}

	m_nFileSize = ArchiveFile.GetSize();
	if (m_nFileSize < sizeof(tLzxInfoHeader))
	{
		// (just ignore like old one?)
		throw ArcException("Not enough data to read header", m_szArchive);
	}

	if (ArchiveFile.Read(m_InfoHeader.info_header, 10) == false)
	{
		throw ArcException("Not enough data to read info header", m_szArchive);
	}

	if (m_InfoHeader.IsLzx() == false)
	{
		throw ArcException("Info_Header: Bad ID, not supported LZX", m_szArchive);
	}
}

CArchiveEntry *CUnLzx::ReadEntryHeader(CAnsiFile &ArchiveFile, const long lOffset)
{
	// zeroize buffer
	m_ReadBuffer.PrepareBuffer(256, false);
	unsigned char *pBuf = m_ReadBuffer.GetBegin();

	// read entry header from archive
	if (ArchiveFile.Read(pBuf, 31) == false)
	{
		// could not read header
		// -> no more files in archive?
		return nullptr;
	}

	CArchiveEntry *pEntry = new CArchiveEntry(lOffset, pBuf, 31);
	pEntry->m_lDataOffset += 31; // increment for data pos
	pEntry->m_lHeaderDataSize += 31; // add to read size

	// temp for counting crc-checksum of entry-header,
	// verify by counting that crc in file is same
	CRCSum CrcSum; // TODO: member of entry?
	
	// get value and reset for counting crc (set zero where header CRC):
	// count CRC of this portion with zero in CRC-bytes
	pEntry->TakeCrcBytes();
	CrcSum.crc_calc(pEntry->m_Header.archive_header, 31);

	unsigned int uiStringLen = 0; // temp for reading string
	
	// read file name
	uiStringLen = pEntry->m_Header.GetFileNameLength();
	if (ArchiveFile.Read(pBuf, uiStringLen) == false)
	{
		throw IOException("Failed reading string: filename");
	}
	pEntry->m_lDataOffset += uiStringLen; // increment for data pos
	pEntry->m_lHeaderDataSize += uiStringLen; // add to read size

	pBuf[uiStringLen] = 0;                 // null-terminate
	CrcSum.crc_calc(pBuf, uiStringLen);    // update CRC
	pEntry->m_szFileName = (char*)pBuf;      // keep as string

	// read comment
	uiStringLen = pEntry->m_Header.GetCommentLength();
	if (ArchiveFile.Read(pBuf, uiStringLen) == false)
	{
		throw IOException("Failed reading string: comment");
	}
	pEntry->m_lDataOffset += uiStringLen; // increment for data pos
	pEntry->m_lHeaderDataSize += uiStringLen; // add to read size

	pBuf[uiStringLen] = 0;                 // null-terminate
	CrcSum.crc_calc(pBuf, uiStringLen);    // update CRC
	pEntry->m_szComment = (char*)pBuf;       // keep as string

	// verify counted crc against the one read from file
	// (in case of corruption of file),
	// check against CRC in entry-header (instead of data which is separate)
	if (CrcSum.GetSum() != pEntry->m_uiCrc)
	{
		// critical error 
		// -> throw exception (cleanup first)
		if (m_bAllowBadCrc == false)
		{
			delete pEntry;
			throw ArcException("CRC: Entry Header", pEntry->m_szFileName);
		}
		else
		{
			// emit error("CRC: Entry Header");
		}
	}

	// parse some entry-header information for later processing
	pEntry->ParseHeader();
	
	// keep this entry
	m_EntryList.insert(tArchiveEntryList::value_type(pEntry->m_lHeaderOffset, pEntry));

	// entry header read successfully
	return pEntry;
}


// this is actual "decompress+store" method on compressed files?
// (ExtractStore() for unpacked+store?)
//
bool CUnLzx::ExtractNormal(CAnsiFile &ArchiveFile, std::vector<CArchiveEntry*> &vEntryList)
{
	unsigned int last_offset = 1;
	unsigned int global_control = 0; // initial control word 
	int global_shift = -16;

	m_decrunch_method = 0;
	m_decrunch_length = 0;

	m_Decoder.setup_buffers_for_decode(&m_ReadBuffer, &m_DecrunchBuffer);

	// TODO: use merged group information in extraction
	// TODO: check that already extracted isn't handled again?
	auto itEntry = vEntryList.begin();
	while (itEntry != vEntryList.end())
	{
		CArchiveEntry *pEntry = (*itEntry);

		// not actual file ? (EOF, maybe removed later..)
		if (pEntry->m_PackMode == tLzxArchiveHeader::HDR_PACK_EOF)
		{
			++itEntry;
			continue;
		}

		// is extracted already?
		// TODO: think of better way for merged files..
		if (pEntry->m_bIsExtracted == true)
		{
			++itEntry;
			continue;
		}

		CRCSum CrcSum; // TODO: member of entry?

		// verify path exists and create it if necessary,
		// open file for writing
		// (should throw exception if critical..)
		CAnsiFile OutFile;
		PrepareEntryForWriting(*pEntry, OutFile);

		// per-entry unpack
		unsigned int unpack_size = pEntry->m_ulUnpackedSize;
		while (unpack_size > 0)
		{
			/* time to fill the buffer? */
			if (m_Decoder.is_time_to_fill_buffer() == true)
			{
				/* check if we have enough data and read some if not */
				if (m_Decoder.is_readbuffer_exhausted() == true) /* have we exhausted the current read buffer? */
				{
					/* copy the remaining overrun to the start of the buffer */
					unsigned int count = 0;
					unsigned char *pEnd = m_Decoder.move_overrun_on_exhaust(count);

					// check remaining packed-data to read
					if (m_pack_size < count) 
					{
						// make sure we don't read too much: remaining at most
						count = m_pack_size;
					}

					// read to where existing data ends?
					if (ArchiveFile.Read(pEnd, count) == false)
					{
						throw IOException("FRead(Archive)");
					}

					m_pack_size -= count;
					pEnd += count;

					// this only if count is already zero 
					if (m_Decoder.source >= pEnd) 
					{
						break; /* argh! no more data! */
					}
				} /* if(source >= source_end) */

				/* check if we need to read the tables */
				if (m_decrunch_length <= 0)
				{
					if (m_Decoder.read_literal_table(global_control, global_shift, m_decrunch_method, m_decrunch_length))
					{
						/* argh! can't make huffman tables! */
						throw IOException("can't make huffman tables!");
						//break;
					}
				}

				// unpack/decrunch some data,
				// update count: how much was decrunched
				m_decrunch_length -= m_Decoder.decrunch_data(global_control, global_shift, last_offset, m_decrunch_method, m_decrunch_length);

			} /* if(pos == destination) */

			/* calculate amount of data we can use before we need to fill the buffer again */
			unsigned long decrunchedsize = m_Decoder.get_decrunched_size();
			if (decrunchedsize > unpack_size) 
			{
				decrunchedsize = unpack_size; /* take only what we need */
			}

			// update to crc and write to output-file
			CrcSum.crc_calc(m_Decoder.m_pos, decrunchedsize);
			if (OutFile.Write(m_Decoder.m_pos, decrunchedsize) == false)
			{
				throw IOException("FWrite(Out)");
			}

			// count bytes remaining to be unpacked
			unpack_size -= decrunchedsize;
			
			// update Decoder.m_pos to new position after 
			// writing decrunched data to file
			//
			m_Decoder.update_pos(decrunchedsize);

		} // while (unpack_size > 0)

		// just make sure enough written before closing
		if (OutFile.Flush() == false)
		{
			throw ArcException("Failed to flush on output-entry", pEntry->m_szFileName);
		}
		OutFile.Close();

		// note: data has different CRC
		// which we need to use in here,
		// header of file-entry has another in the archive
		//
		if (CrcSum.GetSum() != pEntry->m_uiDataCrc)
		{
			// unless user has enabled "skip crc" we throw exception here
			if (m_bAllowBadCrc == false)
			{
				throw ArcException("CRC: Entry Data", pEntry->m_szFileName);
			}
			else
			{
				// log, don't abort
				// emit error("CRC: Entry Header");
			}
		}

		// extracted
		// TODO: think of better way for merged files..
		pEntry->m_bIsExtracted = true;

		++itEntry;
	} // while (itEntry != m_EntryList.end())

	return true;
}

// this is actually store "as-is" method when no compression applied?
//
bool CUnLzx::ExtractStore(CAnsiFile &ArchiveFile, std::vector<CArchiveEntry*> &vEntryList)
{
	auto itEntry = vEntryList.begin();
	while (itEntry != vEntryList.end())
	{
		CArchiveEntry *pEntry = (*itEntry);

		// not actual file (EOF) ?
		if (pEntry->m_PackMode == tLzxArchiveHeader::HDR_PACK_EOF)
		{
			++itEntry;
			continue;
		}

		// already extracted ?
		if (pEntry->m_bIsExtracted == true)
		{
			++itEntry;
			continue;
		}

		// verify path exists and create it if necessary,
		// open file for writing
		// (should throw exception if critical..)
		CAnsiFile OutFile;
		PrepareEntryForWriting(*pEntry, OutFile);

		CRCSum CrcSum; // TODO: member of entry?
		unsigned int unpack_size = pEntry->m_ulUnpackedSize;

		// why this?
		// in this method of packing there is no compression
		// -> only read&write same amount as actually is in archive..
		// (case for a corrupted file?)
		//
		if (unpack_size > pEntry->m_ulPackedSize) 
		{
			unpack_size = pEntry->m_ulPackedSize;
		}
		
		// prepare buffer to at least given size
		// (keep if it is larger, allocate if not),
		// zero existing memory
		m_ReadBuffer.PrepareBuffer(16384, false);
		size_t nBufSize = m_ReadBuffer.GetSize();
		unsigned char *pReadBuf = m_ReadBuffer.GetBegin();

		while (unpack_size > 0)
		{
			unsigned int count = (unpack_size > nBufSize) ? nBufSize : unpack_size;

			// must succeed reading wanted
			if (ArchiveFile.Read(pReadBuf, count) == false)
			{
				throw IOException("Failed to read entry from archive");
			}
			m_pack_size -= count;

			// count crc
			CrcSum.crc_calc(pReadBuf, count);

			if (OutFile.Write(pReadBuf, count) == false)
			{
				throw ArcException("Failed to write to output-entry", pEntry->m_szFileName);
			}
			unpack_size -= count;
		}

		// just make sure enough written before closing
		if (OutFile.Flush() == false)
		{
			throw ArcException("Failed to flush on output-entry", pEntry->m_szFileName);
		}
		OutFile.Close();

		// use CRC of data when checking data..
		//
		if (CrcSum.GetSum() != pEntry->m_uiDataCrc)
		{
			// unless user has enabled "skip crc" we throw exception here
			if (m_bAllowBadCrc == false)
			{
				throw ArcException("CRC: Entry Data", pEntry->m_szFileName);
			}
			else
			{
				// log, don't abort
				// emit error("CRC: Entry Header");
			}
		}

		++itEntry;
	}
	return true;
}

bool CUnLzx::ExtractArchive(CAnsiFile &ArchiveFile)
{
	auto itEntry = m_EntryList.begin();
	auto itEntryEnd = m_EntryList.end();

	while (itEntry != itEntryEnd)
	{
		CArchiveEntry *pEntry = itEntry->second;
		if (pEntry->m_bIsExtracted == true)
		{
			++itEntry;
			continue;
		}

		std::vector<CArchiveEntry*> vEntryList;
		if (pEntry->m_bIsMerged == true)
		{
			// merged-files group compression..
			CMergeGroup *pGroup = pEntry->m_pGroup;
			vEntryList = pGroup->m_MergedList;
			m_pack_size = pGroup->m_ulGroupPackedSize;
		}
		else
		{
			// single-file compression..
			vEntryList.push_back(pEntry);
			m_pack_size = pEntry->m_ulPackedSize;
		}

		if (pEntry->m_ulPackedSize)
		{
			bool bRet = false;
			long lOffset = pEntry->m_lDataOffset; // should be the first..
			m_pack_size = pEntry->m_ulPackedSize;

			// note: unpacking is very sensitive to file-position,
			// try to improve metadata to locate correctly when necessary..
			if (ArchiveFile.Seek(lOffset, SEEK_SET) == false)
			{
				throw IOException("FSeek(Data): failed to seek merge-group start");
			}

			// TODO: give merge-group for extraction..
			switch (pEntry->m_PackMode)
			{
			case tLzxArchiveHeader::HDR_PACK_STORE: /* store */
				bRet = ExtractStore(ArchiveFile, vEntryList);
				break;

			case tLzxArchiveHeader::HDR_PACK_NORMAL: /* normal */
				bRet = ExtractNormal(ArchiveFile, vEntryList);
				break;

			default: /* unknown */
				if (m_bSkipUnknownPackMode == false)
				{
					throw IOException("Extract: unknown pack mode");
				}
				else
				{
					// -> skip ?
					//ArchiveFile.Seek(pEntry->m_ulPackedSize, SEEK_CUR);
				}
				break;
			}
		}

		++itEntry;
	}

	return true;
}

bool CUnLzx::ViewArchive(CAnsiFile &ArchiveFile)
{
	// reset statistical counters before adding again
	ResetCounters();
	ClearItems();
	
	long lEntryOffset = 0;
	while (lEntryOffset < ArchiveFile.GetSize())
	{
		if (ArchiveFile.Tell(lEntryOffset) == false)
		{
			throw IOException("FTell(): failed to tell position");
		}

		// read and verify checksum of this entry-header,
		// should throw exception on error,
		// null-object on end of file
		CArchiveEntry *pEntry = ReadEntryHeader(ArchiveFile, lEntryOffset);
		if (pEntry == nullptr)
		{
			// entry header could not be read
			// -> no more files in archive
			return true;
		}
		
		// count some statistical information
		AddCounters(*pEntry);

		// track some info in merge-groups..
		if (pEntry->m_bIsMerged == true)
		{
			if (m_ulMergeSize == 0)
			{
				// entry starts merge-group?
				m_GroupList.insert(tMergeGroupList::value_type(lEntryOffset, new CMergeGroup(lEntryOffset)));
				auto itGroup = m_GroupList.find(lEntryOffset);
				CMergeGroup *pGroup = itGroup->second;
				pEntry->SetGroup(pGroup);
			}
			else
			{
				// get latest merge-group for addition
				auto itGroup = m_GroupList.rbegin();
				CMergeGroup *pGroup = itGroup->second;

				// add linkage
				if (pGroup->m_MergedList.size() > 0)
				{
					pEntry->m_pPreviousEntry = pGroup->m_MergedList.back();
					pEntry->m_pPreviousEntry->m_pNextEntry = pEntry;
				}
				pEntry->SetGroup(pGroup);
			}
			m_ulMergeSize += pEntry->m_ulUnpackedSize;

			// flag: merged == true,
			// packed size given -> end of merge-group?
			if (pEntry->m_ulPackedSize)
			{
				// end of merge-group, merged size in m_ulMergeSize
				auto itGroup = m_GroupList.rbegin();
				CMergeGroup *pGroup = itGroup->second;
				pGroup->m_ulGroupUnpackedSize = m_ulMergeSize;
				pGroup->m_ulGroupPackedSize = pEntry->m_ulPackedSize;
				m_ulMergeSize = 0; // reset counter (end of merge-group)
			}
		}

		/* seek past the packed data */
		if (pEntry->m_ulPackedSize)
		{
			// in case of merged files, need to seek data from next entry
			// -> seek past this data
			if (ArchiveFile.Seek(pEntry->m_ulPackedSize, SEEK_CUR) == false)
			{
				throw IOException("FSeek(): failed to seek past packed data");
			}
		}
	}

	return true;
}


//////// public methods

// list all files in archive for viewing
bool CUnLzx::View()
{
	CAnsiFile ArchiveFile;

	OpenArchiveFile(ArchiveFile);
	return ViewArchive(ArchiveFile);
}

// list contents
//
// note: user must NOT destroy the objects which have pointers in the list,
// the objects are destroyed by this class.
//
bool CUnLzx::GetEntryList(tArchiveEntryList &lstArchiveInfo) const
{
	lstArchiveInfo = m_EntryList;
	return true;
}

// extract all files from archive to given path
bool CUnLzx::Extract()
{
	CAnsiFile ArchiveFile;

	OpenArchiveFile(ArchiveFile);
	if (ViewArchive(ArchiveFile) == false)
	{
		// failed listing contents first
		return false;
	}
	return ExtractArchive(ArchiveFile);
}

bool CUnLzx::SetExtractPath(const std::string &szOutPath)
{
	// allow resetting on another extract..
	m_szExtractionPath = szOutPath;
	if (m_szExtractionPath.length() > 0)
	{
		// replace "\\" by "/"..
		//m_szExtractionPath.replace('\\', "/");
		auto it = m_szExtractionPath.begin();
		auto itEnd = m_szExtractionPath.end();
		while (it != itEnd)
		{
			if ((*it) == '\\')
			{
				(*it) = '/';
			}
			++it;
		}

		// check proper ending..
		if (m_szExtractionPath.at(m_szExtractionPath.length() -1) != '/')
		{
			m_szExtractionPath += "/";
		}

		// note: check that output-path exists first,
		// check that it is used by file store also..
		CPathHelper::MakePath(m_szExtractionPath);
	}
	return true;
}


/*
// verify integrity of archive-file
bool CUnLzx::TestArchive()
{
}
*/
