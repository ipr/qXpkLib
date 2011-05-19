#ifndef XPKGZIP_GLOBAL_H
#define XPKGZIP_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(XPKGZIP_LIBRARY)
#  define XPKGZIPSHARED_EXPORT Q_DECL_EXPORT
#else
#  define XPKGZIPSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // XPKGZIP_GLOBAL_H
