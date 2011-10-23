#ifndef XADCPIO_GLOBAL_H
#define XADCPIO_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(XADCPIO_LIBRARY)
#  define XADCPIOSHARED_EXPORT Q_DECL_EXPORT
#else
#  define XADCPIOSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // XADCPIO_GLOBAL_H
