#ifndef XADISO_GLOBAL_H
#define XADISO_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(XADISO_LIBRARY)
#  define XADISOSHARED_EXPORT Q_DECL_EXPORT
#else
#  define XADISOSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // XADISO_GLOBAL_H
