//////////////////////////////////////
//
// CPathHelper : separated from AnsiFile for easier reuse
//
// Ilkka Prusi
// ilkka.prusi@gmail.com
//
// Class does not care if 
// path has drive or not (C:, DF0: or any other convention),
// it's upto caller to concatenate path before calling MakePath:
// methods only care about path separator but for reliability 
// you should use full path (e.g "/usr/stuff/foo" or "DH0:foobar/stuff" or "C:/foobar/stuff").
//
// Also we usually don't care if path is made/exists of fails entirely:
// you see error in creating file anyway or if filename is invalid 
// -> simpler error detection.
//

#include "PathHelper.h"

#include <direct.h>
#include <stdlib.h>
#include <stdio.h>


// expects path only without file name
bool CPathHelper::MakePath(const std::string &szPath)
{
	if (szPath.length() == 0)
	{
		// ignore empty path (use current only?)
		return true;
	}

	std::string::size_type nPos = szPath.find('/');
	if (nPos == std::string::npos)
	{
		// no multiple dirs (just one) 
		// -> create what we have

		// don't care if it succeeds of not..
		_mkdir(szPath.c_str());
		return true;
	}

	std::string::size_type nPrevPos = nPos;
	while (nPos != std::string::npos)
	{
		std::string szTmpPath = szPath.substr(0, nPos +1);

		// don't care if it succeeds of not..
		_mkdir(szTmpPath.c_str());

		nPrevPos = nPos +1;
		// locate next separator (after current)
		nPos = szPath.find('/', nPos +2);
	}

	if (nPrevPos < szPath.length())
	{
		// use remaining also (if it doesn't end with path separator)
		std::string szTmpPath = szPath.substr(nPrevPos);
		// don't care if it succeeds of not..
		_mkdir(szTmpPath.c_str());
	}
	return true;
}

// expects ending with file name,
// which is not made to dir..
bool CPathHelper::MakePathToFile(const std::string &szOutFile)
{
	if (szOutFile.length() == 0)
	{
		// ignore empty path (use current only?)
		return true;
	}

	std::string::size_type nPos = szOutFile.find('/');
	if (nPos == std::string::npos)
	{
		// no path (just filename) -> ok
		// -> no need to continue
		return true;
	}

	while (nPos != std::string::npos)
	{
		std::string szPath = szOutFile.substr(0, nPos +1);

		// don't care if it succeeds of not..
		_mkdir(szPath.c_str());

		// locate next separator (after current)
		nPos = szOutFile.find('/', nPos +2);
	}
	return true;
}

