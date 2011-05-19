#ifndef XPKNONE_GLOBAL_H
#define XPKNONE_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(XPKNONE_LIBRARY)
#  define XPKNONESHARED_EXPORT Q_DECL_EXPORT
#else
#  define XPKNONESHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // XPKNONE_GLOBAL_H

