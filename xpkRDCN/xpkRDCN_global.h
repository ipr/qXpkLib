#ifndef XPKRDCN_GLOBAL_H
#define XPKRDCN_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(XPKRDCN_LIBRARY)
#  define XPKRDCNSHARED_EXPORT Q_DECL_EXPORT
#else
#  define XPKRDCNSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // XPKRDCN_GLOBAL_H
