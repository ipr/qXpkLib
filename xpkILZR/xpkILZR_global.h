#ifndef XPKILZR_GLOBAL_H
#define XPKILZR_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(XPKILZR_LIBRARY)
#  define XPKILZRSHARED_EXPORT Q_DECL_EXPORT
#else
#  define XPKILZRSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // XPKILZR_GLOBAL_H
