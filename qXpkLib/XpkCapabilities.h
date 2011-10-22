///////////////////////////////////
//
// XpkCapabilities is re-invented from original XPKMaster way,
// this supposed to be more C++ style and cross-platform.
//
// Struct can be static per sub-library,
// master-library may access this information
// to decide how IO is done. 
//

#ifndef XPKCAPABILITIES_H
#define XPKCAPABILITIES_H

struct XpkCapabilities
{
	// constructor
	XpkCapabilities()
	 : input()
	 , output()
	{}

	struct IOtypes
	{
		// constructor
		IOtypes()
		{
			file = false;
			buffer = false;
			library = false;
		}

		// by default, either file or buffer IO
		// is absolute minimum for a given sub-library.
		// library-IO is entirely optional.
		//
		bool file;	// file IO
		bool buffer; // buffer IO
		bool library;	// between libraries
	};

	IOtypes input;
	IOtypes output;

};

#endif // XPKCAPABILITIES_H
