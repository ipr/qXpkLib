#ifndef XADZAP_GLOBAL_H
#define XADZAP_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(XADZAP_LIBRARY)
#  define XADZAPSHARED_EXPORT Q_DECL_EXPORT
#else
#  define XADZAPSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // XADZAP_GLOBAL_H
