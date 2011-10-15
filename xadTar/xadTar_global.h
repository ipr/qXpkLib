#ifndef XADTAR_GLOBAL_H
#define XADTAR_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(XADTAR_LIBRARY)
#  define XADTARSHARED_EXPORT Q_DECL_EXPORT
#else
#  define XADTARSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // XADTAR_GLOBAL_H
