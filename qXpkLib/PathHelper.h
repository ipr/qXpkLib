//////////////////////////////////////
//
// CPathHelper : separated from AnsiFile for easier reuse
//
// Ilkka Prusi
// ilkka.prusi@gmail.com
//

#ifndef PATHHELPER_H
#define PATHHELPER_H

#include <string>

// don't make static, allow using this helper in sub-libraries
// -> must build into those libraries without reference to parent-library 
class CPathHelper
{
public:
	// no members, just code
    CPathHelper() {}
    virtual ~CPathHelper() {}
    
	bool MakePath(const std::string &szPath);
	bool MakePathToFile(const std::string &szOutFile);
};

#endif // PATHHELPER_H
