#ifndef ECHOXEDITOR_API_H
#define ECHOXEDITOR_API_H

#include <QtCore/qglobal.h>

#if defined(ECHOXEDITOR_LIBRARY)
#  define ECHOXEDITOR_API Q_DECL_EXPORT
#else
#  define ECHOXEDITOR_API Q_DECL_IMPORT
#endif

#endif // ECHOXEDITOR_API_H