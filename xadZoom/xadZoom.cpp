#include "xadZoom.h"

#include "UnZoom.h"

// (see header)
xadZoom g_Instance;
xadLibraryBase *GetXpkInstance(void)
{
  // TODO: switch to: new lib() when there are members..
	return &g_Instance;
}

xadZoom::xadZoom()
 : xadLibraryBase()
{
	// so far only to/from file(s) supported here,
	// may add more (in progress)
	m_XpkCaps.input.file = true;
	//m_XpkCaps.input.buffer = true;
	m_XpkCaps.output.file = true;
	//m_XpkCaps.output.buffer = true;
	m_XpkCaps.m_LibIdentifier = "Zoom";
	m_XpkCaps.m_LibDescription = "Zoom disk-image unpacking";
}

xadZoom::~xadZoom()
{
}

// unpack/decompress
bool xadZoom::Decrunch(XpkProgress *pProgress)
{

	try
	{
	}
	catch (std::exception &exp) // catch by base-type
	{
		//emit fatal_error(exp.what());
	}
	return false;
}

