#include "xadTar.h"

// use from master-library
// to reuse file-handling code
#include "AnsiFile.h"

// use ISO-standard typedefs (platform-independency)
#include <stdint.h>


/* Values used in Typeflag field.  */
//#define TF_FILE 	'0'  /* Regular file */
//#define TF_AFILE	'\0' /* Regular file */
//#define TF_LINK 	'1'  /* Link */
//#define TF_SYM		'2'  /* Reserved - but GNU tar uses this for links... */
//#define TF_CHAR 	'3'  /* Character special */
//#define TF_BLOCK	'4'  /* Block special */
//#define TF_DIR		'5'  /* Drawer */
//#define TF_FIFO 	'6'  /* FIFO special */
//#define TF_CONT 	'7'  /* Reserved */
//#define TF_LONGNAME     'L'  /* longname block, preceedes the full block */
//#define TF_LONGLINK     'K'  /* longlink block, preceedes the full block */

enum TypeFlag // in C++0x : uint8_t
{
	TF_FILE  = '0',   // Regular file 
	TF_AFILE = '\0',  // Regular file 
	TF_LINK  = '1',   // Link 
	TF_SYM	 = '2',   // Reserved - but GNU tar uses this for links... 
	TF_CHAR  = '3',   // Character special 
	TF_BLOCK = '4',   // Block special 
	TF_DIR	 = '5',   // Drawer (directory = folder)
	TF_FIFO  = '6',   // FIFO special 
	TF_CONT  = '7',   // Reserved 
	TF_LONGNAME  = 'L',  // longname block, preceedes the full block 
	TF_LONGLINK  = 'K'  // longlink block, preceedes the full block 
}


const int32_t MINPRINTSIZE	= 51200; // 50KB 
const int32_t NAMEBUFSIZE	= 512;
const int32_t PATBUFSIZE	= (NAMEBUFSIZE*2+10);


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

struct TarBlockInfo 
{
  struct Args *		Args;
  struct xadArchiveInfo *ArchiveInfo;
  uint8_t			Block[512];
  uint8_t                 LongName[512*2];
  struct TarHeader	Header;
  uint32_t			CurSize;
  uint32_t			NumFile;
  uint32_t			NumDir;
  uint32_t                 NumSpecial;
  int32_t			NeedToSkip; /* bytes */
  int32_t			NeedToSave; /* bytes */
  int32_t                  EndBytes;
  //struct Hook		Hook;
  //struct xHookArgs	HookArgs;
  uint8_t                 Filename[NAMEBUFSIZE];
  uint8_t                 EndMode;
  uint8_t                 LongLinkMode;
  uint8_t                 LongNameMode;
};


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
	// only to/from file(s) supported here
	m_XpkCaps.input.file = true;
	m_XpkCaps.output.file = true;
	m_XpkCaps.m_LibIdentifier = "Tar";
	m_XpkCaps.m_LibDescription = "Tar extracting implementation";
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


