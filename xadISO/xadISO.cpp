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

#include <QFile>

/*
 ISO 9660 volume structure definitions from freely available documentation in:
 http://users.telenet.be/it3.consultants.bvba/handouts/ISO9960.html
 
 733 refers to "binary palindrome" encoding of little&big endian values
 where 32-bit integer is given in both byteorders, 
 e.g. 0x11223344 is stored as sequence: 0x44 0x33 0x22 0x11 0x11 0x22 0x33 0x44
*/

// only use either 4-byte sequence in suitable byteorder
typedef uint64_t iso733; // 2*4 bytes sequence for one 4 byte integer

typedef uint32_t iso723; // 2*2 bytes sequence for on 2 byte integer?

typedef uint8_t iso711; // 1 byte

// Volume descriptor types
enum ISOVolumeDescriptorType : iso711
{
	ISO_VD_BOOT				= 0, // Boot Record
	ISO_VD_PRIMARY			= 1, // Primary Volume Descriptor
	ISO_VD_SUPPLEMENTARY	= 2, // Supplementary Volume Descriptor
	ISO_VD_PARTITION		= 3, // Volume Partition Descriptor
	// 4..254 reserved
	ISO_VD_SET_TERMINATOR	= 255 // Volume Descriptor Set Terminator
};

// force align, no padding
#pragma pack(push, 1)

// total size: 2048 bytes
struct iso_primary_descriptor
{
	iso711			type;					// 711
	unsigned char	id[5];					// "CD001" or others
	iso711			version;				// 711
	uint8_t			unused1;				//
	unsigned char	system_id[32];			// "aunsigned chars", narrow chars?
	unsigned char	volume_id[32];			// "dunsigned chars", wide chars?
	uint8_t			unused2[8];				//
	iso733			volume_space_size;		// 733
	uint8_t			unused3[32];			//
	iso723			volume_set_size;		// 723
	iso723			volume_sequence_number;		// 723
	iso723			logical_block_size;			// 723
	iso733			path_table_size;			// 733
	uint8_t			type_l_path_table[4];		// 731
	uint8_t			opt_type_l_path_table[4];	// 731
	uint8_t			type_m_path_table[4];		// 732
	uint8_t			opt_type_m_path_table[4];	// 732
	uint8_t			root_directory_record[34];	// 9.1
	unsigned char	volume_set_id[128];		// "dunsigned chars"
	unsigned char	publisher_id[128];		// "achars"
	unsigned char	preparer_id[128];		// "achars"
	unsigned char	application_id[128];	// "achars"
	unsigned char	copyright_file_id[37];	// 7.5 "dchars"
	unsigned char	abstract_file_id[37];	// 7.5 "dchars"
	unsigned char	bibliographic_file_id[37];	// 7.5 "dchars"
	uint8_t			creation_date[17];		// 8.4.26.1
	uint8_t			modification_date[17];	// 8.4.26.1
	uint8_t			expiration_date[17];	// 8.4.26.1
	uint8_t			effective_date[17];		// 8.4.26.1
	iso711			file_structure_version;	// 711
	uint8_t			unused4;				//
	uint8_t			application_data[512];	//
	uint8_t			unused5[653];			//
};

// size = 33 bytes + (name_len)
struct iso_directory_record 
{
	iso711		length;				// 711
	iso711		ext_attr_length;	// 711
	iso733		extent;				// 733
	iso733		size;				// 733
	iso711		date[7];			// 7 * 711
	iso711		flags;				// 711
	iso711		file_unit_size;		// 711
	iso711		interleave;			// 711
	iso723		volume_sequence_number;	// 723
	iso711		name_len;			// 711
	// followed by name, (name_len)
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
}

xadISO::~xadISO()
{
	if (m_pFile != nullptr)
	{
		delete m_pFile; // should close itself in destructor
		m_pFile = nullptr;
	}
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

// unpack/decompress
bool xadISO::Decrunch(XpkProgress *pProgress)
{
	m_pFile = new QFile(m_filename);
	
	// memory-map the file, easy&efficient for large files
	qint64 size = m_pFile->size();
	uchar *pView = m_pFile->map(0, size);
	
	// seek first primary volume descriptor in file
	// (skip boot record if any)
	uchar *pos = findDescriptorIdentifier((pView + 32768), (pView + size), "CD001");
	if (pos == nullptr)
	{
		// did not find supported identifier
		// (TODO: try other possibilities..)
		// -> abort
		return false;
	}
	
	// TODO: handle filesystem in image,
	// locate & extract each file to dest path
	//
	iso_primary_descriptor *pd = (iso_primary_descriptor*)(pos-1);
	

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
