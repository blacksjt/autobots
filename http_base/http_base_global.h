#ifndef HTTP_BASE_GLOBAL_H
#define HTTP_BASE_GLOBAL_H

#include <QtCore/qglobal.h>

#ifdef HTTP_BASE_LIB
# define HTTP_BASE_EXPORT Q_DECL_EXPORT
#else
# define HTTP_BASE_EXPORT Q_DECL_IMPORT
#endif

#endif // HTTP_BASE_GLOBAL_H
