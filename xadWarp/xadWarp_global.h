#ifndef XADWARP_GLOBAL_H
#define XADWARP_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(XADWARP_LIBRARY)
#  define XADWARPSHARED_EXPORT Q_DECL_EXPORT
#else
#  define XADWARPSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // XADWARP_GLOBAL_H
