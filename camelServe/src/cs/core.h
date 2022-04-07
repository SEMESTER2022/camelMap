#ifndef CS_CORE_H
#define CS_CORE_H

// clang-format off
#if defined(_WIN32)
#	ifdef CS_EXPORT
#		define CS_API __declspec(dllexport)
#	elif defined(CS_SHARED)
#		define CS_API __declspec(dllimport)
#	endif
#else
#	if defined(CS_EXPORT) || defined(CS_SHARED)
#		if defined(__GNUC__) || defined(__clang__)
#			define CS_API __attribute__((visibility("default")))
#		endif
#	endif
#endif
#ifndef CS_API
#	define CS_API
#endif
// clang-format on

namespace cs {

bool CS_API init();

void CS_API mainloop();

void CS_API shutdown();

} // namespace cs

#endif // CS_CORE_H
