#ifndef QQ_ENCRYPT_GLOBAL_H
#define QQ_ENCRYPT_GLOBAL_H

#include <QtCore/qglobal.h>

#ifdef QQ_ENCRYPT_LIB
# define QQ_ENCRYPT_EXPORT Q_DECL_EXPORT
#else
# define QQ_ENCRYPT_EXPORT Q_DECL_IMPORT
#endif

#endif // SINA_ENCRYPT_GLOBAL_H
