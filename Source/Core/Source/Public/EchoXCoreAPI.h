#ifndef ECHOXCORE_API_H
#define ECHOXCORE_API_H

#include <QtCore/qglobal.h>

#if defined(ECHOXCORE_LIBRARY)
#  define ECHOXCORE_API Q_DECL_EXPORT
#else
#  define ECHOXCORE_API Q_DECL_IMPORT
#endif

#endif // ECHOXCORE_API_H