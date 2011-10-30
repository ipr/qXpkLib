#ifndef XADZOOM_GLOBAL_H
#define XADZOOM_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(XADZOOM_LIBRARY)
#  define XADZOOMSHARED_EXPORT Q_DECL_EXPORT
#else
#  define XADZOOMSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // XADZOOM_GLOBAL_H
