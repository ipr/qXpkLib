#include "xadZAP.h"

#include "UnZAP.h"

xadZAP::xadZAP()
 : xadLibraryBase()
{
	// so far only to/from file(s) supported here,
	// may add more (in progress)
	m_XpkCaps.input.file = true;
	//m_XpkCaps.input.buffer = true;
	m_XpkCaps.output.file = true;
	//m_XpkCaps.output.buffer = true;
	m_XpkCaps.m_LibIdentifier = "ZAP";
	m_XpkCaps.m_LibDescription = "ZAP disk-image unpacking";
}

xadZAP::~xadZAP()
{
}

// unpack/decompress
bool xadZAP::Decrunch(XpkProgress *pProgress)
{
	CUnZAP unZap(pProgress->pInputBuffer, pProgress->pOutputBuffer);
	try
	{
		// TODO: set final output size to progress
		// for writing correct amount..
		return unZap.unpack();
	}
	catch (std::exception &exp) // catch by base-type
	{
		//emit fatal_error(exp.what());
	}
	return false;
}

