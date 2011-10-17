#ifndef XFDGZIP_GLOBAL_H
#define XFDGZIP_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(XFDGZIP_LIBRARY)
#  define XFDGZIPSHARED_EXPORT Q_DECL_EXPORT
#else
#  define XFDGZIPSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // XFDGZIP_GLOBAL_H
