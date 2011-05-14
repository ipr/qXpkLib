//////////////////////////////////////
//
// AnsiFile.cpp
//
// Ilkka Prusi
// ilkka.prusi@gmail.com
//

//#include "stdafx.h"
#include "AnsiFile.h"

//#include <dir.h>
//#include <dirent.h>
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

