#ifndef XPKFAST_GLOBAL_H
#define XPKFAST_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(XPKFAST_LIBRARY)
#  define XPKFASTSHARED_EXPORT Q_DECL_EXPORT
#else
#  define XPKFASTSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // XPKFAST_GLOBAL_H
