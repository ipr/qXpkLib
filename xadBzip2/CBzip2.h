///////////////////////////////
//
// CBzip2 : wrapper for Bzip2 functions
// to be used in C++ library "glue" code.
//
// Author: Ilkka Prusi
// ilkka.prusi@gmail.com
//

#ifndef CBZIP2_H
#define CBZIP2_H

#include <QObject>
#include <QString>

// use standard typedefs when possible
#include <stdint.h>

// use wrapper from parent-library
#include "AnsiFile.h"

#include "BzDecompress.h"

class CBzip2 : public QObject //, public CBzDecompress
{
	Q_OBJECT

public:
    CBzip2(QString &szArchive, QObject *parent = 0);
	virtual ~CBzip2(void);

protected:
	QString m_szArchive; // name&path
	size_t m_nFileSize; // size for easy access
	
	uint32_t m_nCompressionMode; // from file header

	bool isValidHeader(CReadBuffer &buf);

public:
	bool Test();
	
	bool Extract();
	
	// information about archive file itself
	QString GetFileName()
	{
		return m_szArchive;
	}
	size_t GetFileSize()
	{
		return m_nFileSize;
	}
	
};

#endif // CBZIP2_H
