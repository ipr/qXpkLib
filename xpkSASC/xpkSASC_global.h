#ifndef XPKSASC_GLOBAL_H
#define XPKSASC_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(XPKSASC_LIBRARY)
#  define XPKSASCSHARED_EXPORT Q_DECL_EXPORT
#else
#  define XPKSASCSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // XPKSASC_GLOBAL_H
