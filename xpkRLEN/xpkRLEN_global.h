#ifndef XPKRLEN_GLOBAL_H
#define XPKRLEN_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(XPKRLEN_LIBRARY)
#  define XPKRLENSHARED_EXPORT Q_DECL_EXPORT
#else
#  define XPKRLENSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // XPKRLEN_GLOBAL_H
