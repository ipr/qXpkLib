#ifndef XADRAR_GLOBAL_H
#define XADRAR_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(XADRAR_LIBRARY)
#  define XADRARSHARED_EXPORT Q_DECL_EXPORT
#else
#  define XADRARSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // XADRAR_GLOBAL_H
