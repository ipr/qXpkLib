#ifndef XADZOO_GLOBAL_H
#define XADZOO_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(XADZOO_LIBRARY)
#  define XADZOOSHARED_EXPORT Q_DECL_EXPORT
#else
#  define XADZOOSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // XADZOO_GLOBAL_H
