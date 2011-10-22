///////////////////////////////////////////
//
// CLhExtract : extract&decode file(s) from LhA-archive file to disk,
// handle file read/write and buffering while extracting,
// uses CLhDecoder for actual decoding interface.
//
// Ilkka Prusi 2011
//


#include "LhExtract.h"

/////// private methods

void CLhExtract::CreateDecoders()
{
	//CLhDecoder *pDecoder = nullptr;
	
	// (note: -lh0-, -lhd- and -lz4- are "store only", no compression)
	
	// -lh1-
	m_mapDecoders.insert(tDecorders::value_type(LZHUFF1_METHOD_NUM, new CLhDecodeLh1()));
	
	// -lh2-
	m_mapDecoders.insert(tDecorders::value_type(LZHUFF2_METHOD_NUM, new CLhDecodeLh2()));
	
	// -lh3-
	m_mapDecoders.insert(tDecorders::value_type(LZHUFF3_METHOD_NUM, new CLhDecodeLh3()));

	// -lh4- .. -lh7- -> same decoding (different instance)
	m_mapDecoders.insert(tDecorders::value_type(LZHUFF4_METHOD_NUM, new CLhDecodeLh7()));
	m_mapDecoders.insert(tDecorders::value_type(LZHUFF5_METHOD_NUM, new CLhDecodeLh7()));
	m_mapDecoders.insert(tDecorders::value_type(LZHUFF6_METHOD_NUM, new CLhDecodeLh7()));
	m_mapDecoders.insert(tDecorders::value_type(LZHUFF7_METHOD_NUM, new CLhDecodeLh7()));
	
	// -lzs-
	m_mapDecoders.insert(tDecorders::value_type(LARC_METHOD_NUM, new CLhDecodeLzs()));
	
	// -lz5-
	m_mapDecoders.insert(tDecorders::value_type(LARC5_METHOD_NUM, new CLhDecodeLz5()));
}

CLhDecoder *CLhExtract::GetDecoder(const tCompressionMethod enMethod)
{
	// TODO: just create new instance every time decoding is needed?
	// (would simplify resetting stuff..)
	
	auto it = m_mapDecoders.find(enMethod);
	if (it != m_mapDecoders.end())
	{
		return it->second;
	}
	return nullptr;
}

/////// protected methods

tHuffBits CLhExtract::GetDictionaryBits(const tCompressionMethod enMethod)
{
    switch (enMethod) 
	{
    case LZHUFF0_METHOD_NUM:    /* -lh0- */
        return LZHUFF0_DICBIT; // -> "store only", not compressed
		
    case LZHUFF1_METHOD_NUM:    /* -lh1- */
        return LZHUFF1_DICBIT;
    case LZHUFF2_METHOD_NUM:    /* -lh2- */
        return LZHUFF2_DICBIT;
    case LZHUFF3_METHOD_NUM:    /* -lh3- */
        return LZHUFF3_DICBIT;
		
    case LZHUFF4_METHOD_NUM:    /* -lh4- */
        return LZHUFF4_DICBIT;
    case LZHUFF5_METHOD_NUM:    /* -lh5- */
        return LZHUFF5_DICBIT;
    case LZHUFF6_METHOD_NUM:    /* -lh6- */
        return LZHUFF6_DICBIT;
    case LZHUFF7_METHOD_NUM:    /* -lh7- */
        return LZHUFF7_DICBIT;
		
    case LARC_METHOD_NUM:       /* -lzs- */
        return LARC_DICBIT;
    case LARC5_METHOD_NUM:      /* -lz5- */
        return LARC5_DICBIT;
		
    case LARC4_METHOD_NUM:      /* -lz4- */
        return LARC4_DICBIT; // -> "store only", not compressed
		
	case LZHDIRS_METHOD_NUM:    /* -lhd- */
        return LARC4_DICBIT; // -> "store only", not compressed
		
    default: // just to silence some compilers..
		break;
    }

	emit warning(QString("unknown method %1").arg((int)enMethod));
	return LZHUFF5_DICBIT; /* for backward compatibility */
}

// extract with decoding (compressed):
// -lh1- .. -lh7-, -lzs-, -lz5-
// -> different decoders and variations needed
//
unsigned int CLhExtract::ExtractDecode(CAnsiFile &ArchiveFile, LzHeader *pHeader)
{
	CLhDecoder *pDecoder = GetDecoder(m_Compression);
	if (pDecoder == nullptr)
	{
		throw ArcException("Unknown/unsupported compression", m_Compression);
	}
	
	if (ArchiveFile.Read(m_ReadBuf.GetBegin(), pHeader->packed_size) == false)
	{
		throw IOException("Failed reading input");
	}

	pDecoder->InitClear();
	pDecoder->InitDictionary(m_Compression, m_HuffBits);
	pDecoder->InitBitIo(pHeader->packed_size, pHeader->original_size, &m_ReadBuf, &m_WriteBuf);
	
	pDecoder->DecodeStart();
	
    while (pDecoder->GetDecodeCount() < pHeader->original_size) 
	{
		pDecoder->Decode();
	}
	
	pDecoder->DecodeFinish();

	return pDecoder->GetCrc();
}

// extract "store only":
// -lh0-, -lhd- and -lz4- 
// -> no compression -> "as-is"
//
unsigned int CLhExtract::ExtractNoCompression(CAnsiFile &ArchiveFile, LzHeader *pHeader)
{
	// no compression, just copy to output
	
	unsigned int uiFileCrc = 0;
	size_t nToWrite = pHeader->original_size;
	
	// TODO: just read entirely at once and remove this looping..
	while (nToWrite > 0)
	{
		size_t read_size = nToWrite;
		if (read_size > 4096)
		{
			read_size = 4096;
		}
		
		if (ArchiveFile.Read(m_ReadBuf.GetBegin(), read_size) == false)
		{
			throw IOException("Failed reading data");
		}
		
		// update crc
		uiFileCrc = m_crcio.calccrc(uiFileCrc, m_ReadBuf.GetBegin(), read_size);
		
		// copy to output-buffer as-is for writing to file when ready
		m_WriteBuf.Append(m_ReadBuf.GetBegin(), read_size);
		
		nToWrite -= read_size;
	}
	
	// file done
	return uiFileCrc;
}

bool CLhExtract::ExtractFileFromArchive(CAnsiFile &ArchiveFile, LzHeader *pHeader)
{
	emit message(QString("decoding.. %1 method: ")
					.arg(pHeader->filename).append(pHeader->pack_method));
	
	// check decoding method (if supported/directory only)
	m_Compression = pHeader->m_enCompression;
	if (m_Compression == LZ_UNKNOWN)
	{
		// unknown/unsupported method
		throw ArcException("Unknown/unsupported compression", pHeader->pack_method.toStdString());
	}
	
	if (m_Compression == LZHDIRS_METHOD_NUM)
	{
		// just make directories and no actual files:
		// -lhd- is just directory-entry in file-list
		// and not actual file (make path only)
		return false;
	}

	// seek in archive where data for this entry begins..
	//
	if (ArchiveFile.Seek(pHeader->data_pos, SEEK_SET) == false)
	{
		throw IOException("Failure seeking entry data");
	}

	// prepare enough in buffers, zero them also
	m_ReadBuf.PrepareBuffer(pHeader->packed_size, false);
	m_WriteBuf.PrepareBuffer(pHeader->original_size, false);
	
	unsigned int uiFileCrc = 0;
	
	// huffman dictionary bits
	m_HuffBits = GetDictionaryBits(m_Compression);
	if ((int)m_HuffBits == 0)
	{
		// no compression, just copy to output
		// (-lh0-, -lhd- and -lz4-)
		
		uiFileCrc = ExtractNoCompression(ArchiveFile, pHeader);
		
		// file done
	}
	else
	{
		// LZ decoding: need decoder for the method
		//
		uiFileCrc = ExtractDecode(ArchiveFile, pHeader);

		// file done
	}
	
	// verify CRC: exception if not match (keep decoded anyway..)
	if (pHeader->isFileCrc(uiFileCrc) == false)
	{
		throw ArcException("CRC error on extract", pHeader->filename.toStdString());
	}
	return true;
}


/////// public methods


// decode data from archive and write to prepared output file,
// use given metadata as help..
//
void CLhExtract::ToFile(CAnsiFile &ArchiveFile, LzHeader *pHeader)
{
	// note: "directory only" entry should be handled by caller already..
	//
	// if compressed size is zero, how much could we read?
	// if original size is zero, what could we write?
	// -> broken archive-file?
	if (pHeader->packed_size == 0 || pHeader->original_size == 0)
	{
		// broken archive?
		emit warning(QString("zero size in file: %1 method: ")
						.arg(pHeader->filename).append(pHeader->pack_method));
		return;
	}

	if (ExtractFileFromArchive(ArchiveFile, pHeader) == false)
	{
		// nothing to write:
		// link/directory with no data (handled separately)
		return;
	}
	
	// TODO: replace non-path characters in case name has them?
	// (different platforms, different restrictions..)

	// check line-ending&combine
	QString szTempPath = GetExtractPathToFile(pHeader->filename);
	if (szTempPath.length() == 0)
	{
		// empty filename -> can't create file
		emit warning(QString("empty name of file, method: ")
						.append(pHeader->pack_method));
		return;
	}
	
	// open file for writing	
	CAnsiFile OutFile;
	if (OutFile.Open(szTempPath.toStdString(), true) == false)
	{
		throw ArcException("Failed creating file for writing", szTempPath.toStdString());
	}
	
	// write to output upto what is collected in write-buffer
	if (OutFile.Write(m_WriteBuf.GetBegin(), m_WriteBuf.GetCurrentPos()) == false)
	{
		throw IOException("Failed writing output");
	}
	
	// flush to disk (whatever we may have..)
	if (OutFile.Flush() == false)
	{
		throw IOException("Failed flushing output");
	}
}

// decode&extract single file from archive to user-buffer:
// extracted file may be any binary blob (sound, image, text.. whatever),
// user is upto deciding what to do with it..
//
void CLhExtract::ToUserBuffer(CAnsiFile &ArchiveFile, LzHeader *pHeader, QByteArray &outArray)
{
	// extract into buffer, don't write to file
	if (ExtractFileFromArchive(ArchiveFile, pHeader) == false)
	{
		// directory/link entry, no data
		// -> nothing to output
		return;
	}
	
	// whatever user might have there, clear it first
	//
	outArray.clear();
	
	// reserve space according to where we are in "write-buffer" (last position decoded)
	// -> force enough to be allocated
	//
	outArray.resize(m_WriteBuf.GetCurrentPos());
	
	// force copy to user: after this our buffer 
	// may be destroyed/library closed/next file extracted.. -> invalidation of our buffer
	//
	::memcpy(outArray.data(), m_WriteBuf.GetBegin(), m_WriteBuf.GetCurrentPos());

	// TODO:	
	// set new used-size to user-buffer?
	// is it implicitly same as resize() ?
	//
	//outArray.set ??
	
}

// only test extraction, no file-output, just internal buffers
//
void CLhExtract::Test(CAnsiFile &ArchiveFile, LzHeader *pHeader)
{
	// try extract into buffer, don't write to file
	ExtractFileFromArchive(ArchiveFile, pHeader);
}

QString CLhExtract::GetExtractPath()
{
	return m_szExtractPath;
}

QString CLhExtract::GetExtractPathToFile(QString &szFilename)
{
	// this should have proper path-ending already..
	QString szTempPath = m_szExtractPath;
	szTempPath += szFilename;
	return szTempPath;
}

void CLhExtract::SetExtractPath(QString &szExtractPath)
{
	// fix path name if MSDOS-style..
	// check ending too
	//
	szExtractPath.replace('\\', "/");
	if (szExtractPath.at(szExtractPath.length() -1) != '/')
	{
		szExtractPath += "/";
	}
	
	m_szExtractPath = szExtractPath;
}
