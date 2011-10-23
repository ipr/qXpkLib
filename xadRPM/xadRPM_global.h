#ifndef XADRPM_GLOBAL_H
#define XADRPM_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(XADRPM_LIBRARY)
#  define XADRPMSHARED_EXPORT Q_DECL_EXPORT
#else
#  define XADRPMSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // XADRPM_GLOBAL_H
