#ifndef XPKPP20_GLOBAL_H
#define XPKPP20_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(XPKPP20_LIBRARY)
#  define XPKPP20SHARED_EXPORT Q_DECL_EXPORT
#else
#  define XPKPP20SHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // XPKPP20_GLOBAL_H
