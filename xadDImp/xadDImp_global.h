#ifndef XADDIMP_GLOBAL_H
#define XADDIMP_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(XADDIMP_LIBRARY)
#  define XADDIMPSHARED_EXPORT Q_DECL_EXPORT
#else
#  define XADDIMPSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // XADDIMP_GLOBAL_H
