#ifndef XADSTUFFIT_GLOBAL_H
#define XADSTUFFIT_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(XADSTUFFIT_LIBRARY)
#  define XADSTUFFITSHARED_EXPORT Q_DECL_EXPORT
#else
#  define XADSTUFFITSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // XADSTUFFIT_GLOBAL_H
