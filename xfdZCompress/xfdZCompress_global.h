#ifndef XFDZCOMPRESS_GLOBAL_H
#define XFDZCOMPRESS_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(XFDZCOMPRESS_LIBRARY)
#  define XFDZCOMPRESSSHARED_EXPORT Q_DECL_EXPORT
#else
#  define XFDZCOMPRESSSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // XFDZCOMPRESS_GLOBAL_H
