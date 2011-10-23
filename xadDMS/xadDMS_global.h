#ifndef XADDMS_GLOBAL_H
#define XADDMS_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(XADDMS_LIBRARY)
#  define XADDMSSHARED_EXPORT Q_DECL_EXPORT
#else
#  define XADDMSSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // XADDMS_GLOBAL_H
