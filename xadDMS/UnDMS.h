////////////////////////
//
// CUnDMS : DMS disk image unpacking/extraction
//
// This should have possibility to read/write to/from file/buffer,
// at first only to/from file maybe..
// 
//

#ifndef UNDMS_H
#define UNDMS_H

// use standard typedefs when possible
#include <stdint.h>

#include <string>
#include <vector>

// use wrapper(s) from parent-library
#include "AnsiFile.h"

// these were just hard-coded before..
// defines set but never used :(
//
enum PackMode
{
	PACK_STORE = 0,
	PACK_RLE = 1,
	PACK_QUICK = 2,
	PACK_MEDIUM = 3,
	PACK_DEEP = 4,
	PACK_HEAVY1 = 5, // TESTPACK ?
	PACK_HEAVY2 = 6,
	CRUNCH_DLE  = 0x90 // is this used in some case?
};

struct Archive_Header
{
	// constr
	Archive_Header()
	{}

   int32_t extra;          // 0  : for future expansion
   int32_t general;        // 4  : general purpose flags
   int32_t timestamp;      // 8  : creation date of archive (seconds since 00:00:00 01-Jan-78)
   int16_t lowtrack;      // 12 : first track (0-85)
   int16_t hightrack;     // 14 : last track (0-85)
   int32_t pack_size;      // 16 : total size of data after compression
   int32_t unpack_size;    // 20 : total size of data before compression
   int32_t serialno;       // 24 : serial number of creator
   int16_t cpu;           // 28 : CPU type of creator
   int16_t copro;         // 30 : CPU coprocessor of creator
   int16_t machine;       // 32 : machine of creator
   int16_t mextra;        // 34 : extra ID information (machine specific)
   int16_t speed;         // 36 : CPU speed (approx)
   int32_t timecreate;    // 38 : time to create archive
   int16_t create_ver;    // 42 : version of creator
   int16_t extract_ver;   // 44 : version needed to extract
   int16_t disktype;      // 46 : disk type of archive
   int16_t crunchmode;    // 48 : compression mode (generally) used
   int16_t header_sum;    // 50 : header CRC
   
	void parseBuf(const uint8_t *buf)
	{
		lowtrack = (buf[12] << 8) + buf[13];
		hightrack = (buf[14] << 8) + buf[15];
		pack_size = (buf[16] << 8) + buf[17];
		unpack_size = (buf[18] << 8) + buf[19];
		extract_ver = (buf[44] << 8) + buf[45];
		header_sum = (buf[50] << 8) + buf[51];
	}
   
}; /* SIZE = 52 */

struct Track_Header
{
	// constr
	Track_Header()
	{}

   int16_t ID;            // 0  : 'TR'
   int16_t track;         // 2  : track number -1=text
   int16_t flags;         // 4  : general flags
   int16_t pack_size;     // 6  : packed length
   int16_t rle_size;      // 8  : rle-packed size
   int16_t unpack_size;   // 10 : unpacked size
   uint8_t pack_flag;      // 12 : general purpose compression flag
   uint8_t pack_mode;      // 13 : compression mode
   int16_t unpack_sum;    // 14 : unpacked CRC
   int16_t data_sum;      // 16 : packed CRC
   int16_t header_sum;    // 18 : header CRC
   
	bool isTR(const uint8_t *buf) const
	{
		if ((buf[0] == 84) && (buf[1] == 82)) /* TR */
		{
			return true;
		}
		return false;
	}
   
	void parseBuf(const uint8_t *buf)
	{
		ID = (buf[0] << 8) + buf[1]; /* TR */
		
		track = (buf[2] << 8) + buf[3];
		pack_size = (buf[6] << 8) + buf[7];
		rle_size = (buf[8] << 8) + buf[9];
		unpack_size = (buf[10] << 8) + buf[11];
		pack_flag = buf[12];
		pack_mode = buf[13];
		unpack_sum = (buf[14] << 8) + buf[15];
		data_sum = (buf[16] << 8) + buf[17];
		header_sum = (buf[18] << 8) + buf[19];
	}
	
	// no details what the flag means but checked in multiple places..
	bool isPackFlagHeavyRle() const
	{
		if (pack_flag & 4)
		{
			return true;
		}
		return false;
	}
   
}; /* SIZE = 20 */

const int16_t ExtractVersion = 111; // minimum version to extract

const size_t BUFFERSIZE = 16000; // medium&deep packing modes

class CUnDMS
{
private:

	std::string m_sourceFile; // temp, see changes planned to parent-library
	std::string m_destFile; // temp, see changes planned to parent-library
	size_t m_nFileSize; // filesize of archive in bytes

	Archive_Header m_header;
	Track_Header *m_tracks;
	
	// see m_header.hightrack

	// to replace old arrays..?
	CReadBuffer m_DecrunchBuffer;

	// TODO: as ptr for parent-instances instead?
	CReadBuffer m_ReadBuffer;
	//CReadBuffer m_WriteBuffer;
	
protected:

	uint8_t *decrunch(const Track_Header *track, uint8_t *pack_buffer);

public:
    CUnDMS()
		: m_sourceFile()
		, m_destFile()
		, m_nFileSize(0)
		, m_header()
		, m_tracks(nullptr)
		, m_DecrunchBuffer(BUFFERSIZE+384) 
		, m_ReadBuffer() 
		//, m_WriteBuffer()
	{}
    ~CUnDMS()
	{
		if (m_tracks != nullptr)
		{
			delete[] m_tracks;
			m_tracks = nullptr;
		}
	}
	
    bool isSupported(const uint8_t *buffer) const
    {
		if((buffer[0] == 68) && (buffer[1] == 77) && /* DMS! */
			(buffer[2] == 83) && (buffer[3] == 33))
		{
			return true;
		}
		return false;
    }

	// TODO: allow to/from buffer/file unpacking..
	// implement necessary details..
	// better way to pass IOcontext from master to here
	// as planned, just need to determine interface..

	// file as source
	void setSourceFile(const std::string &szDms)
	{
		m_sourceFile = szDms;
	}
	// file as dest
	void setDestFile(const std::string &szDms)
	{
		m_destFile = szDms;
	}
	
	/*
	// TODO:
	// client-buffer as source..
	void setSourceBuffer(CReadBuffer *pIn)
	{}
	// client-buffer as dest..
	void setDestBuffer(CReadBuffer *pOut)
	{}
	*/

	bool unpack();
	
};

#endif // UNDMS_H
