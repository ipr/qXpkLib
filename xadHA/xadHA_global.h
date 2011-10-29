#ifndef XADHA_GLOBAL_H
#define XADHA_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(XADHA_LIBRARY)
#  define XADHASHARED_EXPORT Q_DECL_EXPORT
#else
#  define XADHASHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // XADHA_GLOBAL_H
