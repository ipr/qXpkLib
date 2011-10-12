#ifndef XFDSZDD_GLOBAL_H
#define XFDSZDD_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(XFDSZDD_LIBRARY)
#  define XFDSZDDSHARED_EXPORT Q_DECL_EXPORT
#else
#  define XFDSZDDSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // XFDSZDD_GLOBAL_H
