#include "Utility/PreBase.h"


#define null NULL

#define unused(...)   (void)(__VA_ARGS__)
#define default_(...) reinterpret_cast<__VA_ARGS__>(null)

#define as(obj, ...)  dynamic_cast<__VA_ARGS__>(obj)
#define is(obj, ...)  (as(obj, __VA_ARGS__) != null)

#define internal_
#define static_
#define abstract_
#define sealed
#define interface class

#define abstract virtual
#define override virtual

#define foreach(it, list)         for (auto it = (list).begin(); it != (list).end(); it++)
#define safe_foreach(it, list)    for (auto it = (list).begin(), _##it##Prev = it; _##it##Prev = ((_##it##Prev != (list).end()) ? ++_##it##Prev : _##it##Prev), it != (list).end(); it = _##it##Prev)
#define reverse_foreach(it, list) for (auto it = (list).end(); it != (list).begin() && &(--it) != null; )

#define register_qt_type(...)                                       qRegisterMetaType<__VA_ARGS__>(#__VA_ARGS__)
#define register_qml_type(package, versionMajor, versionMinor, ...) qmlRegisterType<__VA_ARGS__>(package, versionMajor, versionMinor, #__VA_ARGS__)

#if defined(USE_CORE_LOGGING) && !defined(CORE_LOGGING_LOG_H)
#include "Logging/Log.h"
using namespace Djbozkosz::Core::Logging;
#endif
