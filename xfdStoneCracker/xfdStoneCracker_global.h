#ifndef XFDSTONECRACKER_GLOBAL_H
#define XFDSTONECRACKER_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(XFDSTONECRACKER_LIBRARY)
#  define XFDSTONECRACKERSHARED_EXPORT Q_DECL_EXPORT
#else
#  define XFDSTONECRACKERSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // XFDSTONECRACKER_GLOBAL_H