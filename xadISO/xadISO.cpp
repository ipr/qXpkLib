////////////////////////////////////////
//
// xadISO - unpacking extension library for qXpkLib
//
// Allows "extracting" files from ISO-image of CD/DVD rom
// without mounting as virtual-device.
//
// Note: currently planning only "base" ISO 9660 support,
// RockRidge/Joliet/HFS/UDF not planned for now..
//
// Note2: will not include CDDA, use other tools for that.
//
// Author: Ilkka Prusi, 2011
// ilkka.prusi@gmail.com
//

#include "xadISO.h"

// use standard typedefs whenever possible
#include <stdint.h>

/*
 ISO 9660 volume structure definitions from freely available documentation in:
 http://users.telenet.be/it3.consultants.bvba/handouts/ISO9960.html
 
 Also: http://alumnus.caltech.edu/~pje/iso9660.html
 
 Also see ECMA-119 standard:
 http://www.ecma-international.org/publications/standards/Ecma-119.htm
 
 733 refers to "binary palindrome" encoding of little&big endian values
 where 32-bit integer is given in both byteorders, 
 e.g. 0x11223344 is stored as sequence: 0x44 0x33 0x22 0x11 0x11 0x22 0x33 0x44
 
 723 is same as 733 except as sequence of two 16-bit integer.

 Only upper-case characters are allowed in strings in the filesystem and metadata.
 "dchars" refers to alphanumeric characters in ASCII-table (0..9, A..Z).
 "achars" refers to part of printable characters in ASCII-table (not all).
 
*/

// Volume descriptor types
enum ISOVolumeDescriptorType : uint8_t
{
	ISO_VD_BOOT				= 0, // Boot Record
	ISO_VD_PRIMARY			= 1, // Primary Volume Descriptor
	ISO_VD_SUPPLEMENTARY	= 2, // Supplementary Volume Descriptor
	ISO_VD_PARTITION		= 3, // Volume Partition Descriptor
	// 4..254 reserved
	ISO_VD_SET_TERMINATOR	= 255 // Volume Descriptor Set Terminator
};

static const size_t nSectorSize = 2048; // sector size 
static const size_t nBootSectorCount = 16; // empty/boot sectors at start (0..15)

// force align, no padding
#pragma pack(push, 1)

// 2*4 bytes sequence for one 4 byte integer
//typedef uint64_t iso733; 
struct iso733_t
{
	uint32_t le;
	uint32_t be;
};

// 2*2 bytes sequence for one 2 byte integer
//typedef uint32_t iso723;
struct iso723_t
{
	uint16_t le;
	uint16_t be;
};

typedef uint8_t iso711; // 1 byte

// only differs in allowed character-set
//typedef unsigned char isoachar;
//typedef unsigned char isodchar;

// 17 bytes, values as "ASCII digits" (string)..
struct iso_date_time
{
	uchar year[4];		// 1..9999
	
	uchar month[2];		// 1..12
	uchar day[2];		// 1..31
	uchar hour[2];		// 0..23
	uchar minute[2];	// 0..59
	uchar second[2];	// 0..59
	uchar fractions[2]; // hundreths of a second
	
	int8_t gmt_offset; // 15min intervals from -48(W) +52(E)
};

// at least one primary descriptor, 0..n additional volume descriptors
// total size: 2048 bytes
struct iso_primary_descriptor
{
	iso711			type;					// 711
	unsigned char	id[5];					// "CD001" or others
	iso711			version;				// 711
	uint8_t			unused1;				//
	unsigned char	system_id[32];			// "achars", narrow chars?
	unsigned char	volume_id[32];			// "dchars", wide chars?
	uint8_t			unused2[8];				//
	iso733_t		volume_space_size;		// 733
	uint8_t			unused3[32];			//
	iso723_t		volume_set_size;		// 723
	iso723_t		volume_sequence_number;	// 723
	iso723_t		logical_block_size;		// 723, sector size (2048)
	iso733_t		path_table_size;		// 733
	uint32_t		type_l_path_table;		// 731, little-endian logical sector for first path table entry
	uint32_t		opt_type_l_path_table;	// 731, optional type L
	uint32_t		type_m_path_table;		// 732, big-endian logical sector for first path table entry
	uint32_t		opt_type_m_path_table;	// 732, optional type M
	uint8_t			root_directory_record[34];	// 9.1
	unsigned char	volume_set_id[128];		// "dunsigned chars"
	unsigned char	publisher_id[128];		// "achars"
	unsigned char	preparer_id[128];		// "achars"
	unsigned char	application_id[128];	// "achars"
	unsigned char	copyright_file_id[37];	// 7.5 "dchars"
	unsigned char	abstract_file_id[37];	// 7.5 "dchars"
	unsigned char	bibliographic_file_id[37];	// 7.5 "dchars"
	iso_date_time	creation_date;			// 8.4.26.1
	iso_date_time	modification_date;		// 8.4.26.1
	iso_date_time	expiration_date;		// 8.4.26.1
	iso_date_time	effective_date;			// 8.4.26.1
	iso711			file_structure_version;	// 711
	uint8_t			unused4;				//
	uint8_t			application_data[512];	//
	uint8_t			unused5[653];			//
};

// size = 33 bytes + (name_len) + 1 padding
struct iso_directory_record 
{
	iso711			length;				// 711, LEN_DR
	iso711			ext_attr_length;	// 711
	iso733_t		extent_pos;			// 733
	iso733_t		data_length;		// 733
	iso711			date[7];			// 7 * 711
	iso711			file_flags;			// 711
	iso711			file_unit_size;		// 711
	iso711			interleave_gap_size;	// 711
	iso723_t		volume_sequence_number;	// 723
	iso711			name_len;			// 711, LEN_FI
	// followed by name (LEN_FI bytes)
	uint8_t			padding;
	// system use ? (LEN_SU bytes)
};

struct iso_path_table_record
{
	iso711			length_dir_identifier;	// LEN_DI
	iso711			extended_attrib_len;	// 
	uint32_t		extent_pos;				// location
	uint16_t		parent_dir_number;		// 
	// directory identifier: LEN_DI bytes
};

struct iso_ext_attrib_record 
{
	uint32_t		owner_id;
	uint32_t		group_id;
	uint16_t		permissions;
	iso_date_time	creation_date;			// 
	iso_date_time	modification_date;		// 
	iso_date_time	expiration_date;		// 
	iso_date_time	effective_date;			// 
	uint8_t			record_format;
	uint8_t			record_attributes;
	uint32_t		record_length;
	unsigned char	system_id[32];			// a-chars, a1-chars
	uint8_t			system_use[64];			// not specified
	uint8_t			extended_attribute_record_version;
	uint8_t			length_escape_sequences; // LEN_ESC
	uint8_t			reserved[64];			// not specified
	uint32_t		length_application_use; // LEN_AU
	// application use:  LEN_AU bytes (content?? skip??)
	// escape sequences: LEN_ESC bytes (content?? skip??)
};


// restore normal align
#pragma pack(pop)


// (see header)
xadISO g_Instance;
xadLibraryBase *GetXpkInstance(void)
{
  // TODO: switch to: new xpkRLEN() when there are members..
	return &g_Instance;
}


xadISO::xadISO()
	: xadLibraryBase()
	, m_destPath()
	, m_filename()
	, m_pFile(nullptr)
{
	// only from file supported here
	m_XpkCaps.input.file = true;
	
	// output (optionally) to buffer
	//m_XpkCaps.output.file = true;
	//m_XpkCaps.output.buffer = true;
	
	m_XpkCaps.m_LibIdentifier = "ISO";
	m_XpkCaps.m_LibDescription = "ISO 9660 extracting implementation";
}

xadISO::~xadISO()
{
	if (m_pFile != nullptr)
	{
		delete m_pFile; // should close itself in destructor
		m_pFile = nullptr;
	}
}

// TODO: implement giving info to caller..
bool xadISO::archiveInfo(QXpkLib::CArchiveInfo &info)
{
	// (see Decrunch() for testing..)
	return false;
}


// set path to uncompress files to
bool xadISO::setExtractPath(QString &szPath)
{
	m_destPath = szPath;
	return true;
}

// test archive integrity
bool xadISO::testArchive()
{
	// not implemented yet
	return false;
}

// unpack/decompress files
bool xadISO::Decrunch(XpkProgress *pProgress)
{
	// we likely have memory-mapped file in master
	// -> use it
	CIoContext *pIn = pProgress->pInputIo;
	CReadBuffer *pData = pIn->getBuffer(); // access data via wrapper
	uint8_t *pView = pData->GetBegin();
	size_t fileSize = pData->GetSize();
	
	// seek first primary volume descriptor in file
	// (skip boot record if any)
	const size_t nStart = (nBootSectorCount * nSectorSize);
	uchar *pos = findDescriptorIdentifier((pView + nStart), (pView + fileSize), "CD001");
	if (pos == nullptr)
	{
		// did not find supported identifier
		// (TODO: try other possibilities..)
		// -> abort
		return false;
	}
	
	iso_primary_descriptor *pd = (iso_primary_descriptor*)(pos-1);
	if (pd->volume_space_size.le > fileSize)
	{
		// error: invalid volume size / bug in code?
		return false;
	}
	if (pd->logical_block_size.le != nSectorSize)
	{
		// unsupported sector size
		return false;
	}

	// skip volume records to first path table entry..
	//
	std::string pathName;
	size_t nPathOffset = pd->type_l_path_table * nSectorSize;
	iso_path_table_record *ptbl = (iso_path_table_record*)(pView + nPathOffset);
	
	nPathOffset += sizeof(iso_path_table_record);
	if (ptbl->length_dir_identifier == 1) // root dir
	{
		// empty in this case?
	}
	else
	{
		pathName.assign((char*)(pView + nPathOffset), ptbl->length_dir_identifier);
	}
	
	if ((ptbl->length_dir_identifier %2) != 0)
	{
		// even: no padding
		// odd: 1 byte for padding
		nPathOffset += 1;
	}

	// should be next entry here?
	ptbl = (iso_path_table_record*)(pView + nPathOffset);
	nPathOffset += sizeof(iso_path_table_record);
	pathName.assign((char*)(pView + nPathOffset), ptbl->length_dir_identifier);

	return true;
}

///////// private methods

uchar *xadISO::findDescriptorIdentifier(uchar *pos, const uchar *end, const char *identifier)
{
	while ((pos + 5) < end)
	{
		// locate "CD001" or other supported identifier..
		if (::memcmp(pos, identifier, 5) == 0)
		{
			return pos;
		}
		pos += 1;
	}
	
	// did not find this identifier in file
	return nullptr;
}
