#ifndef XPKDUMMY_GLOBAL_H
#define XPKDUMMY_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(XPKDUMMY_LIBRARY)
#  define XPKDUMMYSHARED_EXPORT Q_DECL_EXPORT
#else
#  define XPKDUMMYSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // XPKDUMMY_GLOBAL_H
