#ifndef XPKSHSC_GLOBAL_H
#define XPKSHSC_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(XPKSHSC_LIBRARY)
#  define XPKSHSCSHARED_EXPORT Q_DECL_EXPORT
#else
#  define XPKSHSCSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // XPKSHSC_GLOBAL_H
