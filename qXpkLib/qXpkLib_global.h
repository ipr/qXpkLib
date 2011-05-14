#ifndef QXPKLIB_GLOBAL_H
#define QXPKLIB_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(QXPKLIB_LIBRARY)
#  define QXPKLIBSHARED_EXPORT Q_DECL_EXPORT
#else
#  define QXPKLIBSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // QXPKLIB_GLOBAL_H
