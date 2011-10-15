#ifndef XPKHUFF_GLOBAL_H
#define XPKHUFF_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(XPKHUFF_LIBRARY)
#  define XPKHUFFSHARED_EXPORT Q_DECL_EXPORT
#else
#  define XPKHUFFSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // XPKHUFF_GLOBAL_H
