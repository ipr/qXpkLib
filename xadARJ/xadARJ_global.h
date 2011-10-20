#ifndef XADARJ_GLOBAL_H
#define XADARJ_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(XADARJ_LIBRARY)
#  define XADARJSHARED_EXPORT Q_DECL_EXPORT
#else
#  define XADARJSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // XADARJ_GLOBAL_H
