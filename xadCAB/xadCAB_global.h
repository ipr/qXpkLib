#ifndef XADCAB_GLOBAL_H
#define XADCAB_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(XADCAB_LIBRARY)
#  define XADCABSHARED_EXPORT Q_DECL_EXPORT
#else
#  define XADCABSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // XADCAB_GLOBAL_H
