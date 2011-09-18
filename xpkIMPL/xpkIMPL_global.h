#ifndef XPKIMPL_GLOBAL_H
#define XPKIMPL_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(XPKIMPL_LIBRARY)
#  define XPKIMPLSHARED_EXPORT Q_DECL_EXPORT
#else
#  define XPKIMPLSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // XPKIMPL_GLOBAL_H
