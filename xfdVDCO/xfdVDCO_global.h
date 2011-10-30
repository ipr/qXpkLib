#ifndef XFDVDCO_GLOBAL_H
#define XFDVDCO_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(XFDVDCO_LIBRARY)
#  define XFDVDCOSHARED_EXPORT Q_DECL_EXPORT
#else
#  define XFDVDCOSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // XFDVDCO_GLOBAL_H
