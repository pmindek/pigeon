#ifndef NN_GLOBAL_H
#define NN_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(NN_LIBRARY)
#  define NN_API Q_DECL_EXPORT
#else
#  define NN_API Q_DECL_IMPORT
#endif

#endif // NN_GLOBAL_H
