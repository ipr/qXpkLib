#ifndef XADLHA_GLOBAL_H
#define XADLHA_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(XADLHA_LIBRARY)
#  define XADLHASHARED_EXPORT Q_DECL_EXPORT
#else
#  define XADLHASHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // XADLHA_GLOBAL_H
