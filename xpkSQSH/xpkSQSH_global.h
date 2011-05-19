#ifndef XPKSQSH_GLOBAL_H
#define XPKSQSH_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(XPKSQSH_LIBRARY)
#  define XPKSQSHSHARED_EXPORT Q_DECL_EXPORT
#else
#  define XPKSQSHSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // XPKSQSH_GLOBAL_H
