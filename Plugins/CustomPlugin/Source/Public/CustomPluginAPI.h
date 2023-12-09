#ifndef CUSTOMPLUGIN_API_H
#define CUSTOMPLUGIN_API_H

#include <QtCore/qglobal.h>

#if defined(CUSTOMPLUGIN_LIBRARY)
#  define CUSTOMPLUGIN_API Q_DECL_EXPORT
#else
#  define CUSTOMPLUGIN_API Q_DECL_IMPORT
#endif

#endif // CUSTOMPLUGIN_API_H