#ifndef XPKDUKE_GLOBAL_H
#define XPKDUKE_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(XPKDUKE_LIBRARY)
#  define XPKDUKESHARED_EXPORT Q_DECL_EXPORT
#else
#  define XPKDUKESHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // XPKDUKE_GLOBAL_H
