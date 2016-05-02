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

#ifndef PATHHELPER_H
#define PATHHELPER_H

#include <string>

// don't make static, allow using this helper in sub-libraries
// -> must build into those libraries without reference to parent-library 
class CPathHelper
{
protected:
    bool MakeDir(const std::string &szPath);

public:
	// no members, just code
    CPathHelper() {}
    virtual ~CPathHelper() {}
    
	bool MakePath(const std::string &szPath);
	bool MakePathToFile(const std::string &szOutFile);
};

#endif // PATHHELPER_H
