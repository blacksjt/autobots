#ifndef SINA_ENCRYPT_GLOBAL_H
#define SINA_ENCRYPT_GLOBAL_H

#include <QtCore/qglobal.h>

#ifdef SINA_ENCRYPT_LIB
# define SINA_ENCRYPT_EXPORT Q_DECL_EXPORT
#else
# define SINA_ENCRYPT_EXPORT Q_DECL_IMPORT
#endif

#endif // SINA_ENCRYPT_GLOBAL_H
