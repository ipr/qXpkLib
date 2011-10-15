///////////////////////////////////////////
//
// CLhExtract : extract&decode file(s) from LhA-archive file to disk,
// handle file read/write and buffering while extracting,
// uses CLhDecoder for actual decoding interface.
//
// Ilkka Prusi 2011
//

#ifndef LHEXTRACT_H
#define LHEXTRACT_H

#include <QObject>
#include <QString>
#include <QByteArray>

#include <map>

#include "AnsiFile.h"
#include "LhaTypeDefs.h"
#include "LhHeader.h"
#include "crcio.h"

#include "LhDecoder.h"


class CLhExtract : public QObject
{
	Q_OBJECT
	
private:
	typedef std::map<tCompressionMethod, CLhDecoder*> tDecorders;
	tDecorders m_mapDecoders;
	void CreateDecoders();
	
	inline CLhDecoder *GetDecoder(const tCompressionMethod enMethod);
	
protected:
	
	QString m_szExtractPath;
	
	CCrcIo m_crcio;
	
	CReadBuffer m_ReadBuf; // packed data from archive
	CReadBuffer m_WriteBuf; // unpacked data for writing
	
	// kept and updated when extracting file
	//unsigned int m_uiCrc; // TODO: cumulative of all files? anywhere to check?
	tCompressionMethod m_Compression;
	tHuffBits m_HuffBits;
	
	tHuffBits GetDictionaryBits(const tCompressionMethod enMethod);

	unsigned int ExtractDecode(CAnsiFile &ArchiveFile, LzHeader *pHeader);
	unsigned int ExtractNoCompression(CAnsiFile &ArchiveFile, LzHeader *pHeader);

	bool ExtractFileFromArchive(CAnsiFile &ArchiveFile, LzHeader *pHeader);
	
public:
    CLhExtract(QObject *parent = 0)
		: QObject(parent)
	    , m_szExtractPath()
		, m_mapDecoders()
		, m_ReadBuf(4096) // emulated old buffering style
		, m_WriteBuf(2* 4096)
		, m_crcio()
		//, m_uiCrc(0)
	{
		CreateDecoders();
	}
    ~CLhExtract()
	{
		auto it = m_mapDecoders.begin();
		auto itEnd = m_mapDecoders.end();
		while (it != itEnd)
		{
			CLhDecoder *pDec = it->second;
			delete pDec;
			++it;
		}
		m_mapDecoders.clear();
	}

	void ToFile(CAnsiFile &ArchiveFile, LzHeader *pHeader);
	void ToUserBuffer(CAnsiFile &ArchiveFile, LzHeader *pHeader, QByteArray &outArray);
	void Test(CAnsiFile &ArchiveFile, LzHeader *pHeader);

	QString GetExtractPath();
	QString GetExtractPathToFile(QString &szFilename);
	void SetExtractPath(QString &szExtractPath);
	
signals:
	// progress-status by signals, errors by exceptions
	void message(QString);
	void warning(QString);
};

#endif // LHEXTRACT_H
