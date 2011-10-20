#ifndef XADARC_GLOBAL_H
#define XADARC_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(XADARC_LIBRARY)
#  define XADARCSHARED_EXPORT Q_DECL_EXPORT
#else
#  define XADARCSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // XADARC_GLOBAL_H
