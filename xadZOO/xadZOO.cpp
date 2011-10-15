#include "xadZOO.h"

#include "UnZoo.h"

xadZOO::xadZOO()
{
}

// placeholder,
// needs common interface
// and need to set archive, path and some options 
// for extracting files..
bool xadZOO::extract()
{
	CUnZoo unZoo;
	try
	{
	}
	catch (std::exception &exp) // catch by base-type
	{
		// debug..
		//std::cerr << "Exception: " << exp.what() << std::end;
	}
	return false;
}
