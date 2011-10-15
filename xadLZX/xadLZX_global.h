#ifndef XADLZX_GLOBAL_H
#define XADLZX_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(XADLZX_LIBRARY)
#  define XADLZXSHARED_EXPORT Q_DECL_EXPORT
#else
#  define XADLZXSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // XADLZX_GLOBAL_H
