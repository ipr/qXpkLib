#ifndef XADACE_GLOBAL_H
#define XADACE_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(XADACE_LIBRARY)
#  define XADACESHARED_EXPORT Q_DECL_EXPORT
#else
#  define XADACESHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // XADACE_GLOBAL_H
