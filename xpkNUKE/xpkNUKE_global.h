#ifndef XPKNUKE_GLOBAL_H
#define XPKNUKE_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(XPKNUKE_LIBRARY)
#  define XPKNUKESHARED_EXPORT Q_DECL_EXPORT
#else
#  define XPKNUKESHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // XPKNUKE_GLOBAL_H
