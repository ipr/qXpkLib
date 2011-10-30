#ifndef XFDVICE_GLOBAL_H
#define XFDVICE_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(XFDVICE_LIBRARY)
#  define XFDVICESHARED_EXPORT Q_DECL_EXPORT
#else
#  define XFDVICESHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // XFDVICE_GLOBAL_H
