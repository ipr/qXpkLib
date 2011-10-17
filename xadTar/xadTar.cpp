#include "xadTar.h"

// use from master-library
// to reuse file-handling code
#include "AnsiFile.h"

// use ISO-standard typedefs (platform-independency)
#include <stdint.h>

// force align, no padding
#pragma pack(push, 1)

// Tar-file header,
// offsets of fields in comments.
//
// This header is from XadMaster-Dev
// (author: Dirk Stöcker <stoecker@amigaworld.com>),
// check format with latest open versions (if changed).
//
struct TarHeader
{
  uint8_t th_Name[100];		//   0 
  uint8_t th_Mode[8];		// 100 
  uint8_t th_UserID[8];		// 108 
  uint8_t th_GroupID[8];	// 116 
  uint8_t th_Size[12];		// 124 
  uint8_t th_MTime[12];		// 136 
  uint8_t th_Checksum[8]; 	// 148 
  uint8_t th_Typeflag;		// 156 
  uint8_t th_LinkName[100];	// 157 
  uint8_t th_Magic[6];		// 257 
  uint8_t th_Version[2];	// 263 
  uint8_t th_UserName[32];	// 265 
  uint8_t th_GroupName[32];	// 297 
  uint8_t th_DevMajor[8]; 	// 329 
  uint8_t th_DevMinor[8]; 	// 337 
  uint8_t th_Prefix[155]; 	// 345 
  uint8_t th_Pad[12];		// 500 
};

#pragma pack(pop)


// (see header)
xadTar g_Instance;
xadLibraryBase *GetXpkInstance(void)
{
  // TODO: switch to: new xpkRLEN() when there are members..
	return &g_Instance;
}

xadTar::xadTar()
 : xadLibraryBase()
{
}

xadTar::~xadTar()
{
}

// set path to uncompress files to
bool xadTar::setExtractPath(QString &szPath)
{
	// test, debug
	CAnsiFile archive(szPath.toStdString());
	if (archive.IsOk() == false)
	{
		return false;
	}
	CReadBuffer buf(archive.GetSize());
	if (archive.Read(buf.GetBegin(), buf.GetSize()) == false)
	{
		return false;
	}
	
	TarHeader *phead = (TarHeader*)buf.GetBegin();



	//m_pArchive->SetExtractPath(szPath.toStdString());
	return true;
}

// test archive integrity
bool xadTar::testArchive()
{
	// not implemented yet
	return false;
}

// unpack/decompress
bool xadTar::Decrunch(XpkProgress *pProgress)
{
	try
	{
		//CTar tar;
		//return tar->Extract();
	}
	catch (std::exception &exp) // catch by base-type
	{
		//emit fatal_error(exp.what());
	}
	return false;
}


