//////////////////////////////////////
//
// UnLzx.h
//
// Ilkka Prusi
// ilkka.prusi@gmail.com
//

#ifndef _UNLZX_H_
#define _UNLZX_H_

#include <string>
//#include <list>
#include <map>
//#include <set>
#include <vector>

#include "CrcSum.h"
#include "AnsiFile.h"


// archive info-header entry (archive-file header)
struct tLzxInfoHeader
{
	enum tInfoFlags
	{
		INFO_DAMAGE_PROTECT  = 1,
		INFO_FLAG_LOCKED  = 2
	};
	
	/* STRUCTURE Info_Header
	{
	  UBYTE ID[3]; 0 - "LZX"
	  UBYTE flags; 3 - INFO_FLAG_#?
	  UBYTE[6]; 4
	} */ /* SIZE = 10 */
	unsigned char info_header[10];

	tLzxInfoHeader()
	{
		::memset(info_header, 0, sizeof(unsigned char)*10);
	}

	bool IsLzx()
	{
		if ((info_header[0] == 76) && (info_header[1] == 90) && (info_header[2] == 88)) /* LZX */
		{
			return true;
		}
		return false;
	}
	bool IsDamageProtect()
	{
		if ((info_header[3] & (int)INFO_DAMAGE_PROTECT) != 0)
		{
			return true;
		}
		return false;
	}
	bool IsLocked()
	{
		if ((info_header[3] & (int)INFO_FLAG_LOCKED) != 0)
		{
			return true;
		}
		return false;
	}
};

// header of single entry in LZX-archive
struct tLzxArchiveHeader
{
	enum tHeaderFlag
	{
		HDR_FLAG_MERGED = 1
	};

	enum tHeaderMachineType
	{
		HDR_TYPE_MSDOS   = 0,
		HDR_TYPE_WINDOWS = 1,
		HDR_TYPE_OS2     = 2,
		HDR_TYPE_AMIGA   = 10,
		HDR_TYPE_UNIX    = 20
	};

	// file-entry packing mode
	// (not used as mask, enum-only)
	enum tHeaderPackMode
	{
		HDR_PACK_STORE    = 0,
		HDR_PACK_NORMAL   = 2,
		HDR_PACK_EOF      = 32
	};
	
	/* STRUCTURE Archive_Header
	{
	  UBYTE attributes; 0 - HDR_PROT_#?
	  UBYTE; 1
	  ULONG unpacked_length; 2 - FUCKED UP LITTLE ENDIAN SHIT
	  ULONG packed_length; 6 - FUCKED UP LITTLE ENDIAN SHIT
	  UBYTE machine_type; 10 - HDR_TYPE_#?
	  UBYTE pack_mode; 11 - HDR_PACK_#?
	  UBYTE flags; 12 - HDR_FLAG_#?
	  UBYTE; 13
	  UBYTE len_comment; 14 - comment length [0,79]
	  UBYTE extract_ver; 15 - version needed to extract
	  UBYTE; 16
	  UBYTE; 17
	  ULONG date; 18 - Packed_Date
	  ULONG data_crc; 22 - FUCKED UP LITTLE ENDIAN SHIT
	  ULONG header_crc; 26 - FUCKED UP LITTLE ENDIAN SHIT
	  UBYTE filename_len; 30 - filename length
	} */ /* SIZE = 31 */
	unsigned char archive_header[31];

	tLzxArchiveHeader()
	{
		::memset(archive_header, 0, sizeof(unsigned char)*31);
	}

	// CRC in header (including filename, comment etc.):
	// set to zero before counting CRC of this part
	unsigned int TakeCrcBytes()
	{
		unsigned int crc = (archive_header[29] << 24) + (archive_header[28] << 16) + (archive_header[27] << 8) + archive_header[26];

		/* Must set the field to 0 before calculating the crc */
		archive_header[29] = 0;
		archive_header[28] = 0;
		archive_header[27] = 0;
		archive_header[26] = 0;

		return crc;
	}

	// CRC of actual data (after checking header-CRC)
	unsigned int GetDataCrc()
	{
		return (archive_header[25] << 24) + (archive_header[24] << 16) + (archive_header[23] << 8) + archive_header[22]; /* data crc */
	}

	unsigned char GetAttributes()
	{
		return archive_header[0]; /* file protection modes */
	}

	unsigned char GetMachineType()
	{
		return archive_header[10]; /* machine type */
	}
	
	unsigned char GetPackMode()
	{
		return archive_header[11]; /* pack mode */
	}
	
	unsigned char GetFlags()
	{
		return archive_header[12]; /* flags */
	}

	unsigned char GetExtractVersion()
	{
		// get version needed to extract (details?)
		return archive_header[15]; /* extract_ver */
	}
	
	unsigned int GetFileNameLength()
	{
		return archive_header[30]; /* filename length */
	}

	unsigned int GetCommentLength()
	{
		return archive_header[14]; /* comment length */
	}

	unsigned int GetPackSize()
	{
		// packed size of file (may be zero on merged)
		return (archive_header[9] << 24) + (archive_header[8] << 16) + (archive_header[7] << 8) + archive_header[6]; /* packed size */
	}

	unsigned int GetUnpackSize()
	{
		// original/unpacked size of file
		return (archive_header[5] << 24) + (archive_header[4] << 16) + (archive_header[3] << 8) + archive_header[2]; /* unpack size */
	}


	/* STRUCTURE DATE_Unpacked
	{
	  UBYTE year; 80 - Year 0=1970 1=1971 63=2033
	  UBYTE month; 81 - 0=january 1=february .. 11=december
	  UBYTE day; 82
	  UBYTE hour; 83
	  UBYTE minute; 84
	  UBYTE second; 85
	} */ /* SIZE = 6 */
	
	/* STRUCTURE DATE_Packed
	{
	  UBYTE packed[4]; bit 0 is MSB, 31 is LSB
	; bit # 0-4=Day 5-8=Month 9-14=Year 15-19=Hour 20-25=Minute 26-31=Second
	} */ /* SIZE = 4 */
	
	unsigned int GetPackedTimestamp()
	{
		return (archive_header[18] << 24) + (archive_header[19] << 16) + (archive_header[20] << 8) + archive_header[21]; /* date */
	}
};

typedef struct PackedTimestamp
{
	/*
	enum tDateShift
	{
		DATE_SHIFT_YEAR   = 17,
		DATE_SHIFT_MONTH  = 23,
		DATE_SHIFT_DAY    = 27,
		DATE_SHIFT_HOUR   = 12,
		DATE_SHIFT_MINUTE = 6,
		DATE_SHIFT_SECOND = 0
	};
	enum tDateMask
	{
		DATE_MASK_YEAR    = 0x007E0000,
		DATE_MASK_MONTH   = 0x07800000,
		DATE_MASK_DAY     = 0xF8000000,
		DATE_MASK_HOUR    = 0x0001F000,
		DATE_MASK_MINUTE  = 0x00000FC0,
		DATE_MASK_SECOND  = 0x0000003F
	};
	*/
	
	// "as-is"
	unsigned int m_uiTimeStamp;
	
	// unpacked
	unsigned int year;
	unsigned int month;
	unsigned int day;
	unsigned int hour;
	unsigned int minute;
	unsigned int second;
	
	PackedTimestamp(void)
	{
		m_uiTimeStamp = 0;
		year = 0;
		month = 0;
		day = 0;
		hour = 0;
		minute = 0;
		second = 0;
	}
	
	/* STRUCTURE DATE_Packed
	{
	  UBYTE packed[4]; bit 0 is MSB, 31 is LSB
	; bit # 0-4=Day 5-8=Month 9-14=Year 15-19=Hour 20-25=Minute 26-31=Second
	} */ /* SIZE = 4 */
	void SetParseStamp(const unsigned int uiPackedStamp)
	{
		m_uiTimeStamp = uiPackedStamp;
		
		// split value into timestamp-parts
		year = ((uiPackedStamp >> 17) & 63) + 1970;
		month = (uiPackedStamp >> 23) & 15;
		day = (uiPackedStamp >> 27) & 31;
		hour = (uiPackedStamp >> 12) & 31;
		minute = (uiPackedStamp >> 6) & 63;
		second = uiPackedStamp & 63;
	}
	
} PackedTimestamp;

// these match Amiga-style file-attributes
// (protection mode flags):
// HSPA RWED
typedef struct FileAttributes
{
	// protection flags
	enum tHeaderProt
	{
		HDR_PROT_READ    = 1,
		HDR_PROT_WRITE   = 2,
		HDR_PROT_DELETE  = 4,
		HDR_PROT_EXECUTE = 8,
		HDR_PROT_ARCHIVE = 16,
		HDR_PROT_HOLD    = 32,
		HDR_PROT_SCRIPT  = 64,
		HDR_PROT_PURE    = 128
	};
	
	// protection flags "as-is"
	unsigned char m_ucAttribs;

	// parsed file-protection flags
	bool h; // 'hidden'
	bool s; // 'script'
	bool p; // 'pure' (can be made resident in-memory)
	bool a; // 'archived'
	bool r; // 'readable'
	bool w; // 'writable'
	bool e; // 'executable'
	bool d; // 'delete'
	
	// constructor
	FileAttributes(void)
	{
		h = false;
		s = false;
		p = false;
		a = false;

		r = false;
		w = false;
		e = false;
		d = false;
		m_ucAttribs = 0;
	}

	// constructor
	FileAttributes(const unsigned char ucAttribs)
	{
		ParseAttributes(ucAttribs);
	}
	
	/* parse file protection modes */
	void ParseAttributes(const unsigned char ucAttribs)
	{
		m_ucAttribs = ucAttribs;
		h = ((m_ucAttribs & (int)HDR_PROT_HOLD)    ? true : false);
		s = ((m_ucAttribs & (int)HDR_PROT_SCRIPT)  ? true : false);
		p = ((m_ucAttribs & (int)HDR_PROT_PURE)    ? true : false);
		a = ((m_ucAttribs & (int)HDR_PROT_ARCHIVE) ? true : false);
		r = ((m_ucAttribs & (int)HDR_PROT_READ)    ? true : false);
		w = ((m_ucAttribs & (int)HDR_PROT_WRITE)   ? true : false);
		e = ((m_ucAttribs & (int)HDR_PROT_EXECUTE) ? true : false);
		d = ((m_ucAttribs & (int)HDR_PROT_DELETE)  ? true : false);
	}
	
} FileAttributes;


// fwd. decl.
class CArchiveEntry;

class CMergeGroup
{
public:
	CMergeGroup(void) 
		: m_MergedList()
		, m_pHead(nullptr)
		, m_lGroupOffset(0)
		, m_ulGroupUnpackedSize(0)
		, m_ulGroupPackedSize(0)
		//, m_ulMergeSize(0)
	{};
	CMergeGroup(const long lOffset) 
		: m_MergedList()
		, m_pHead(nullptr)
		, m_lGroupOffset(lOffset)
		, m_ulGroupUnpackedSize(0)
		, m_ulGroupPackedSize(0)
		//, m_ulMergeSize(0)
	{};
	~CMergeGroup(void) 
	{
		// don't delete objects, 
		// destroyed elsewhere.
		m_MergedList.clear();
	};
	
	typedef std::vector<CArchiveEntry*> tMergedList;
	tMergedList m_MergedList;

	// head-node
	CArchiveEntry *m_pHead;

	// offset in archive file
	// to beginning of this group of merged files.
	long m_lGroupOffset;
	
	// total size of group when unpacked (sum of file sizes)
	unsigned long m_ulGroupUnpackedSize;

	// total group packed size in archive
	unsigned long m_ulGroupPackedSize;

	// merged data size in archive
	//unsigned long m_ulMergeSize;


	// add entry as member of this group
	// note: merged files need to extracted in-order
	// (by offset in file) for correctness
	// -> caller should get ordered list (as found)
	void SetEntry(CArchiveEntry *pEntry)
	{
		if (m_MergedList.size() == 0)
		{
			m_pHead = pEntry;
		}

		auto it = m_MergedList.begin();
		auto itEnd = m_MergedList.end();
		while (it != itEnd)
		{
			if ((*it) == pEntry)
			{
				// bug: same instance -> abort
				return;
			}
			++it;
		}
		m_MergedList.push_back(pEntry);
	}
};

// describe single entry within LZX-archive
class CArchiveEntry
{
public:
	CArchiveEntry(const long lEntryOffset, const unsigned char *pArcHeader, const size_t nArchSize)
		: m_lHeaderOffset(lEntryOffset)
		, m_lHeaderDataSize(0)
		, m_lDataOffset(lEntryOffset)
		, m_Header()
		, m_Attributes()
		, m_Timestamp()
		, m_MachineType(tLzxArchiveHeader::HDR_TYPE_AMIGA)
		, m_PackMode(tLzxArchiveHeader::HDR_PACK_EOF)
		, m_bIsMerged(false)
		, m_uiCrc(0)
		, m_uiDataCrc(0)
		, m_ulUnpackedSize(0)
		, m_bPackedSizeAvailable(true)
		, m_ulPackedSize(0)
		, m_pPreviousEntry(nullptr)
		, m_pNextEntry(nullptr)
		, m_pGroup(nullptr)
		, m_bIsExtracted(false)
		, m_szFileName()
		, m_szComment()
	{
		// always same size
		::memcpy(m_Header.archive_header, pArcHeader, nArchSize);
	}
	~CArchiveEntry(void)
	{}

	// copy header CRC-bytes from read header
	// and zeroize them before counting CRC on that part
	void TakeCrcBytes()
	{
		m_uiCrc = m_Header.TakeCrcBytes();
	}
	
	void ParseHeader()
	{
		// keep data-CRC for easy handling
		m_uiDataCrc = m_Header.GetDataCrc();
		
		// file protection modes
		m_Attributes.ParseAttributes(m_Header.GetAttributes());
		
		// packing information
		m_ulUnpackedSize = m_Header.GetUnpackSize();
		m_ulPackedSize = m_Header.GetPackSize();
		
		m_Timestamp.SetParseStamp(m_Header.GetPackedTimestamp());
		
		// test, always at least "1.0" ? (10)
		//unsigned char ucExtractVer = m_Header.GetExtractVersion();

		// use as enum instead of flags
		unsigned char ucMachine = m_Header.GetMachineType();
		if (ucMachine == tLzxArchiveHeader::HDR_TYPE_AMIGA)
		{
			m_MachineType = tLzxArchiveHeader::HDR_TYPE_AMIGA;
		}
		else if (ucMachine == tLzxArchiveHeader::HDR_TYPE_UNIX)
		{
			m_MachineType = tLzxArchiveHeader::HDR_TYPE_UNIX;
		}
		else if (ucMachine == tLzxArchiveHeader::HDR_TYPE_OS2)
		{
			m_MachineType = tLzxArchiveHeader::HDR_TYPE_OS2;
		}
		else if (ucMachine == tLzxArchiveHeader::HDR_TYPE_WINDOWS)
		{
			m_MachineType = tLzxArchiveHeader::HDR_TYPE_WINDOWS;
		}
		else if (ucMachine == tLzxArchiveHeader::HDR_TYPE_MSDOS)
		{
			m_MachineType = tLzxArchiveHeader::HDR_TYPE_MSDOS;
		}
		
		// pack-mode: not used as flags, enum-only
		unsigned char ucPackMode = m_Header.GetPackMode();
		if (ucPackMode == tLzxArchiveHeader::HDR_PACK_NORMAL)
		{
			m_PackMode = tLzxArchiveHeader::HDR_PACK_NORMAL;
		}
		else if (ucPackMode == tLzxArchiveHeader::HDR_PACK_STORE)
		{
			m_PackMode = tLzxArchiveHeader::HDR_PACK_STORE;
		}
		else if (ucPackMode == tLzxArchiveHeader::HDR_PACK_EOF)
		{
			m_PackMode = tLzxArchiveHeader::HDR_PACK_EOF;
		}
		
		unsigned char ucFlags = m_Header.GetFlags();
		if (ucFlags & tLzxArchiveHeader::HDR_FLAG_MERGED)
		{
			m_bPackedSizeAvailable = false;
			m_bIsMerged = true;
		}
	}
	
	void SetGroup(CMergeGroup *pGroup)
	{
		// keep group and set as member of it
		m_pGroup = pGroup;
		pGroup->SetEntry(this);
	}

	// offsets in archive file
	// for this entry
	long m_lHeaderOffset; // start of header
	long m_lHeaderDataSize; // size of header data
	long m_lDataOffset; // offset of data

	// entry header from archive
	tLzxArchiveHeader m_Header;
	
	// file attributes of entry
	FileAttributes m_Attributes;
	
	// timestamp of entry
	PackedTimestamp m_Timestamp;

	// machine type where archive is created
	tLzxArchiveHeader::tHeaderMachineType m_MachineType;
	
	// packing mode
	tLzxArchiveHeader::tHeaderPackMode m_PackMode;

	// if file is merged with another
	// (packing size may be zero/not available
	// for individual files, just merged size)
	bool m_bIsMerged;
	

	// CRC from file header
	unsigned int m_uiCrc;

	// CRC of data
	unsigned int m_uiDataCrc;

	// unpacked size from file
	unsigned long m_ulUnpackedSize;

	// for some files, packed size might not be given in archive
	// (merged files only?)
	bool m_bPackedSizeAvailable;

	// compressed size from file
	unsigned long m_ulPackedSize;

	// some stuff for diagnostics&debugging
	// of merge-group handling, may be useful later..
	//
	CArchiveEntry *m_pPreviousEntry; // previous file-entry (when merged)
	CArchiveEntry *m_pNextEntry; // next file-entry (when merged)
	CMergeGroup *m_pGroup; // merge group this belongs to (if any)

	// is file already extracted
	// TODO: this is quick hack, 
	// think of better way to handle merged files..
	bool m_bIsExtracted;

	// name of entry from archive
	std::string m_szFileName;

	// comment of entry from archive
	std::string m_szComment;
};

// list of each merged-file group in single archive
// key: offset in archive-file to group (internal use)
// value: description of group
typedef std::map<long, CMergeGroup*> tMergeGroupList;

// list of each entry in single archive
// key: offset in archive-file to entry (internal use)
// value: description of entry
typedef std::map<long, CArchiveEntry*> tArchiveEntryList;


// moving actual decoding to own class (per compression type)
// -> one compression/decompressio type (sliding window changes)
class CLzxDecoder
{
protected:
	static const unsigned char table_one[32];
	static const unsigned int table_two[32];
	static const unsigned int table_three[16];
	static const unsigned char table_four[34];

	// runtime-filled decoding tables
	unsigned char offset_len[8];
	unsigned short offset_table[128];
	unsigned char huffman20_len[20];
	unsigned short huffman20_table[96];
	unsigned char literal_len[768];
	unsigned short literal_table[5120];

protected:

	inline unsigned int reverse_position(const unsigned int fill_size, const unsigned int reverse_pos)
	{
		unsigned int fill = fill_size;
		unsigned int reverse = reverse_pos;
		unsigned int leaf = 0; /* always starts at zero */
		
		/* reverse the position */
		do
		{
			leaf = (leaf << 1) + (reverse & 1);
			reverse >>= 1;
		} while(--fill);
		
		/* used after reversing*/
		return leaf;
	}
	inline void fix_shift_control_word(int &shift, unsigned int &control)
	{
		shift += 16;
		control += ((*source) << (8 + shift));
		source++;
		control += ((*source) << shift);
		source++;
	}
	
	inline void fix_shift_control_long(int &shift, unsigned int &control)
	{
		shift += 16;
		control += ((*source) << 24);
		source++;
		control += ((*source) << 16);
		source++;
	}

	// two cases only but shortens read_literal_table() slightly..
	void symbol_longer_than_6_bits(int &shift, unsigned int &control, unsigned int &symbol)
	{
		/* when symbol is longer than 6 bits */
		do
		{
			symbol = huffman20_table[((control >> 6) & 1) + (symbol << 1)];
			if(!shift--)
			{
				fix_shift_control_long(shift, control);
			}
			control >>= 1;
		} while(symbol >= 20);
	}

	// just shortens read_literal_table() slightly..
	void read_decrunch_method(int &shift, unsigned int &control, unsigned int &decrunch_method)
	{
		decrunch_method = control & 7;
		control >>= 3;
		if((shift -= 3) < 0)
		{
			fix_shift_control_word(shift, control);
		}
	}

	// just shortens read_literal_table() slightly..
	void read_decrunch_length(int &shift, unsigned int &control, unsigned int &decrunch_length)
	{
		decrunch_length = (control & 255) << 16;
		control >>= 8;
		if((shift -= 8) < 0)
		{
			fix_shift_control_word(shift, control);
		}
		
		decrunch_length += (control & 255) << 8;
		control >>= 8;
		if((shift -= 8) < 0)
		{
			fix_shift_control_word(shift, control);
		}
		
		decrunch_length += (control & 255);
		control >>= 8;
		if((shift -= 8) < 0)
		{
			fix_shift_control_word(shift, control);
		}
	}

	int read_build_offset_table(int &shift, unsigned int &control)
	{
		unsigned int temp = 0;
		for(temp = 0; temp < 8; temp++)
		{
			offset_len[temp] = control & 7;
			control >>= 3;
			if((shift -= 3) < 0)
			{
				fix_shift_control_word(shift, control);
			}
		}
		return make_decode_table(8, 7, offset_len, offset_table);
	}

	void build_huffman_len_table(int &shift, unsigned int &control)
	{
		unsigned int temp = 0;
		for(temp = 0; temp < 20; temp++)
		{
			huffman20_len[temp] = control & 15;
			control >>= 4;
			if((shift -= 4) < 0)
			{
				fix_shift_control_word(shift, control);
			}
		}
	}

	// this fucker changes pointer for position -> must access in larger scope, use member-pointer..
	void copy_decrunch_string(unsigned int &last_offset, unsigned int &count)
	{
		unsigned char *pDecrunchBegin = m_pDecrunchBuffer->GetBegin();
		
		unsigned char *string = (pDecrunchBegin + last_offset < destination) ?
				destination - last_offset : 
				destination + 65536 - last_offset;

		// note: in theory could replace loop with
		// ::memcpy(destination, string, count);
		// but may overlap memory areas so can't use memcpy()..
		do
		{
			*destination++ = *string++;
		} while(--count);
	}

	CReadBuffer *m_pReadBuffer;
	CReadBuffer *m_pDecrunchBuffer;

public:
	// these need to be public for now..
	// TODO: cleanup

	// positions in read-buffer
	unsigned char *source;
	unsigned char *source_end;
	// positions in decrunch-buffer
	unsigned char *destination;
	unsigned char *destination_end;

	// this is accessed so that decrunched data 
	// can be written to file from buffer
	unsigned char *m_pos;
	

public:
	CLzxDecoder() {}
	
	void setup_buffers_for_decode(CReadBuffer *pReadBuffer, CReadBuffer *pDecrunchBuffer)
	{
		m_pReadBuffer = pReadBuffer;
		m_pDecrunchBuffer = pDecrunchBuffer;

		::memset(offset_len, 0, sizeof(unsigned char)*8);
		::memset(literal_len, 0, sizeof(unsigned char)*768);
		
		// setup some globals
		source = pReadBuffer->GetAt(16384);
		source_end = pReadBuffer->GetAt(16384 - 1024);
		destination = pDecrunchBuffer->GetAt(258 + 65536);
		
		destination_end = destination;
		m_pos = destination;
	}

	bool is_readbuffer_exhausted()
	{
		if (source >= source_end)
		{
			return true;
		}
		return false;
	}

	/* copy the remaining overrun to the start of the buffer */
	unsigned char *move_overrun_on_exhaust(unsigned int &count)
	{
		/* copy the remaining overrun to the start of the buffer */
		
		unsigned char *pBegin = m_pReadBuffer->GetBegin();
		count = (pBegin - source) + 16384; // size before moving
		if (count > 0)
		{
			::memmove(pBegin, source, count);
		}

		unsigned char *pEnd = m_pReadBuffer->GetAt(count);
		count = (pBegin - pEnd) + 16384; // size after moving
		source = pBegin;
		return pEnd;
	}
	
	int make_decode_table(int number_symbols, int table_size, unsigned char *length, unsigned short *table);
	int read_literal_table(unsigned int &control, int &shift, unsigned int &decrunch_method, unsigned int &decrunch_length);

	void decrunch(unsigned int &control, int &shift, unsigned int &last_offset, const unsigned int &decrunch_method);

	unsigned int decrunch_data(unsigned int &control, int &shift, unsigned int &last_offset, unsigned int &decrunch_method, unsigned int &decrunch_length)
	{
		/* unpack some data */
		unsigned int count = unpack(decrunch_length);

		// take before decrunch
		unsigned char *ptempdest = destination;

		decrunch(control, shift, last_offset, decrunch_method);

		// return count: how much was decrunched 
		// (where we are after decrunching)
		return (destination - ptempdest);
	}

	bool is_time_to_fill_buffer()
	{
		if (m_pos == destination)
		{
			return true;
		}
		return false;
	}

	unsigned int unpack(unsigned int &decrunch_length)
	{
		unsigned int count = 0;
		unsigned char *pDecrunchBegin = m_pDecrunchBuffer->GetBegin();
		
		/* unpack some data */
		if (destination >= (pDecrunchBegin + 258 + 65536))
		{
			count = (destination - pDecrunchBegin) - 65536;
			if (count)
			{
				// copy the overrun to the start of the buffer 
				//
				// simplified, memmove() should support overlap anyway
				// (should not be issue here..)
				//
				::memmove(pDecrunchBegin, 
						  m_pDecrunchBuffer->GetAt(65536), 
						  count);

				// reget pointer to position
				destination = m_pDecrunchBuffer->GetAt(count);
			}
			m_pos = destination;
		}
		
		// note: can we be sure destination is always 
		// beginning of decrunch-buffer in this case?
		// could simplify and reduce some pointers..
		// 
		//destination_end = m_pDecrunchBuffer->GetAt(decrunch_length);
		//
		destination_end = destination + decrunch_length;
		if (destination_end > m_pDecrunchBuffer->GetAt(258 + 65536))
		{
			destination_end = m_pDecrunchBuffer->GetAt(258 + 65536);
		}
		
		return count;
	}
	
	// get count of unpacked data in decrunch-buffer for writing
	unsigned long get_decrunched_size()
	{
		return (destination - m_pos);
	}

	// update current position by decrunched size
	void update_pos(unsigned long ulDecrunchedSize)
	{
		m_pos += ulDecrunchedSize;
	}
};

class CUnLzx
{
private:

	std::string m_szArchive; // path and name of archive-file
	size_t m_nFileSize; // filesize of archive in bytes

	// internal buffer for read information
	CReadBuffer m_ReadBuffer;
	CReadBuffer m_DecrunchBuffer;
	
	// archive info-header (file-type etc.)
	tLzxInfoHeader m_InfoHeader;

	// list of merged-file groups in archive
	tMergeGroupList m_GroupList;
	
	// list of items in archive (files)
	tArchiveEntryList m_EntryList;

	// user-given path where file(s) are 
	// extracted to from current archive
	// (may change on each extract() call..)
	std::string m_szExtractionPath;

	// actual decoding/decrunching routines
	CLzxDecoder m_Decoder;
	
	// current sizes for extraction
	// (need to share for merged groups)
	unsigned int m_pack_size;
	//unsigned int m_unpack_size;

	// for corrupted archives, just try to extract what we can:
	// user must enable this option, otherwise aborted with exception
	bool m_bAllowBadCrc;

	// if archive has unknown (unsupported) packing mode,
	// try to skip those: user must enable or aborted with exception
	bool m_bSkipUnknownPackMode;

	// decrunch&decode related status values
	unsigned int m_decrunch_method;
	unsigned int m_decrunch_length;
	
	// some counters for statistics of archive
	unsigned long m_ulTotalUnpacked;
	unsigned long m_ulTotalPacked;
	unsigned long m_ulTotalFiles;
	unsigned long m_ulMergeSize;

	// update counters with those in entry
	void AddCounters(CArchiveEntry &Entry)
	{
		// add some statistical information
		m_ulTotalPacked += Entry.m_ulPackedSize;
		m_ulTotalUnpacked += Entry.m_ulUnpackedSize;
		m_ulTotalFiles++;
	}
	void ResetCounters()
	{
		m_ulTotalUnpacked = 0;
		m_ulTotalPacked = 0;
		m_ulTotalFiles = 0;
		m_ulMergeSize = 0;
	}
	void ClearItems()
	{
		auto itG = m_GroupList.begin();
		auto itGEnd = m_GroupList.end();
		while (itG != itGEnd)
		{
			CMergeGroup *pGroup = itG->second;
			delete pGroup;
			++itG;
		}
		m_GroupList.clear();

		auto itE = m_EntryList.begin();
		auto itEEnd = m_EntryList.end();
		while (itE != itEEnd)
		{
			CArchiveEntry *pEntry = itE->second;
			delete pEntry;
			++itE;
		}
		m_EntryList.clear();
	}

protected:
	// create path and open file for writing
	void PrepareEntryForWriting(CArchiveEntry &Entry, CAnsiFile &OutFile);

	void OpenArchiveFile(CAnsiFile &ArchiveFile);

	//bool ReadEntryHeader(CAnsiFile &ArchiveFile, const long lOffset, CArchiveEntry &Entry);
	CArchiveEntry *ReadEntryHeader(CAnsiFile &ArchiveFile, const long lOffset);

	bool ExtractNormal(CAnsiFile &ArchiveFile, std::vector<CArchiveEntry*> &vEntryList);
	bool ExtractStore(CAnsiFile &ArchiveFile, std::vector<CArchiveEntry*> &vEntryList);

	bool ExtractArchive(CAnsiFile &ArchiveFile);

	bool ViewArchive(CAnsiFile &ArchiveFile);

public:
	// options for extraction (some for viewing?)
	/*
	class CUnLzxOptions
	{
		bool m_bKeepAttributes;		// keep file-attribs (set on output-file)
		bool m_bKeepTime;			// keep timestamp (set on output-file)
		bool m_bClearArcBit;		// clear "archive" flag on extract
		bool m_bLowercaseFilenames; // make filenames lower-case on extract

		// by default, we handle paths "as-is",
		// with this no paths are created on extraction (all to same place)
		// which can cause trouble with duplicate filenames..
		//bool m_bIgnorePaths;		// don't preserve path names on extract

		//std::string m_szPattern; // pattern-match to files (extract matching)
		//std::string m_szWorkPath; // set working path
	};
	*/

	CUnLzx(const std::string &szArchive)
		: m_szArchive(szArchive)
		, m_nFileSize(0)
		, m_ReadBuffer(16384) /* have a reasonable sized read buffer */
		, m_DecrunchBuffer(258+65536+258) /* allow overrun for speed */
		, m_InfoHeader()
		, m_GroupList()
		, m_EntryList()
		, m_szExtractionPath()
		, m_Decoder()
		, m_pack_size(0)
		//, m_unpack_size(0)
		, m_decrunch_method(0)
		, m_decrunch_length(0)
		, m_ulTotalUnpacked(0)
		, m_ulTotalPacked(0)
		, m_ulTotalFiles(0)
		, m_ulMergeSize(0)
		, m_bAllowBadCrc(false)
		, m_bSkipUnknownPackMode(false)
	{
	}

	~CUnLzx(void)
	{
		ClearItems();
	}

	// For corrupted archives, just try to extract what we can:
	// user must enable this option, otherwise aborted with exception.
	// 
	// Note: other exceptions may be thrown if decrunch-tables can't be made
	// (only choice is to skip entire merge-group in those cases).
	//
	// Usual cause may be broken setpatch on some cases (wrong version?)
	// and only fix would be to repack all again..
	//
	void SetAllowBadCrc(const bool bValue)
	{
		m_bAllowBadCrc = bValue;
	}

	// if archive has unknown (unsupported) packing mode,
	// try to skip those: user must enable or aborted with exception
	void SetSkipUnknownPackMode(const bool bValue)
	{
		m_bSkipUnknownPackMode = bValue;
	}

	// view a single archive:
	// get archive metadata
	// and list of each entry in the archive
	//
	bool View();

	// list contents
	//
	// note: user must NOT destroy the objects which have pointers in the list,
	// the objects are destroyed by this class.
	//
	bool GetEntryList(tArchiveEntryList &lstArchiveInfo) const;

	// extract a single archive:
	// give path where files are extracted to,
	// additional directories are created under that (if necessary)
	//
	bool Extract();

	// TODO: extraction variations for caller simplicity..
	//bool ExtractSelected(QStringList &lstFiles);
	//bool ExtractToBuffer(QString &szFileEntry, QByteArray &outArray);
	
	bool SetExtractPath(const std::string &szOutPath);

	// TODO:
	// verify archive integrity
	//
	bool TestArchive();

	// information about archive file itself
	std::string GetArchiveFileName()
	{
		return m_szArchive;
	}
	size_t GetArchiveFileSize()
	{
		return m_nFileSize;
	}

	// statistical information access to caller
	unsigned long GetTotalSizeUnpacked() 
	{ 
		return m_ulTotalUnpacked; 
	}
	unsigned long GetTotalSizePacked() 
	{ 
		return m_ulTotalPacked; 
	}
	unsigned long GetTotalFileCount() 
	{ 
		return m_ulTotalFiles; 
	}

	/* // internal use only.. per-group
	unsigned long GetMergeSize() 
	{ 
		return m_ulMergeSize; 
	}
	*/
};

#endif // ifndef _UNLZX_H_

