#ifndef XADADF_GLOBAL_H
#define XADADF_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(XADADF_LIBRARY)
#  define XADADFSHARED_EXPORT Q_DECL_EXPORT
#else
#  define XADADFSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // XADADF_GLOBAL_H
