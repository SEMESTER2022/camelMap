#ifndef CS_CORE_H
#define CS_CORE_H

// library version in standard format major * 10000 + minor * 100 + patch.
#define CS_VERSION 10101

// clang-format off

#ifndef CS_ASSERT
#	include <cassert>
#	define CS_ASSERT(cond) assert(cond)
#endif

#ifndef CS_ERROR
#	include <cassert>
#	define CS_ERROR(reason) assert(0 && reason)
#endif

#if defined(__GNUC__) && !defined(__clang__) && !defined(__INTEL_COMPILER) && \
	!defined(__NVCOMPILER)
#	define CS_GCC_VERSION (__GNUC__ * 100 + __GNUC_MINOR__)
#else
#	define CS_GCC_VERSION 0
#endif

#ifdef __ICL
#   define CS_ICC_VERSION __ICL
#elif defined(__INTEL_COMPILER)
#   define CS_ICC_VERSION __INTEL_COMPILER
#else
#   define CS_ICC_VERSION 0
#endif

#ifdef __NVCOMPILER
#	define CS_NVCOMPILER_VERSION \
		(__NVCOMPILER_MAJOR__ * 100 + __NVCOMPILER_MINOR__)
#else
#	define CS_NVCOMPILER_VERSION 0
#endif

#ifdef __NVCC__
#	define CS_NVCC __NVCC__
#else
#	define CS_NVCC 0
#endif

#ifdef _MSC_VER
#   define CS_MSC_VER _MSC_VER
#else
#   define CS_MSC_VER 0
#endif

#ifndef CS_HAS_FEATURE
#	ifdef __has_feature
#   	define CS_HAS_FEATURE(x) __has_feature(x)
#	else
#   	define CS_HAS_FEATURE(x) 0
#	endif
#endif

#if defined(__has_include) &&                            \
	(!defined(__INTELLISENSE__) || CS_MSC_VER > 1900) && \
	(!CS_ICC_VERSION || CS_ICC_VERSION >= 1600)
#  	define CS_HAS_INCLUDE(x) __has_include(x)
#else
#	define CS_HAS_INCLUDE(x) __has_include(x)
#endif

#ifndef CS_HAS_CPP_ATTRIBUTE
#	ifdef __has_cpp_attribute
#   	define CS_HAS_CPP_ATTRIBUTE(x) __has_cpp_attribute(x)
#	else
#   	define CS_HAS_CPP_ATTRIBUTE
#	endif
#endif

#ifndef CS_CPLUSPLUS
#	ifdef _MSVC_LANG
#	   define CS_CPLUSPLUS _MSVC_LANG
#	else
#	   define CS_CPLUSPLUS __cplusplus
#	endif
#endif

#ifndef CS_USE_CONSTEXPR
#	define CS_USE_CONSTEXPR										         \
		(CS_HAS_FEATURE(cxx_relaxed_constexpr) || CS_MSC_VER >= 1912 ||  \
			(CS_GCC_VERSION >= 600 && __cplusplus >= 201402L)) &&        \
				!CS_NVCC && !CS_ICC_VERSION
#endif
#if CS_USE_CONSTEXPR
#	define CS_CONSTEXPR constexpr
#else
#	define CS_CONSTEXPR
#endif

#ifndef CS_FALLTHROUGH
#	if CS_HAS_CPP_ATTRIBUTE(fallthrough)
#		define CS_FALLTHROUGH [[fallthrough]]
#	else
#		define CS_FALLTHROUGH
#	endif
#endif

#ifndef CS_NODISCARD
#	if CS_HAS_CPP_ATTRIBUTE(nodiscard)
#		define CS_NODISCARD [[nodiscard]]
#	else
#		define CS_NODISCARD
#	endif
#endif

#if CS_HAS_INCLUDE(<string_view>)
#	define CS_USE_STRING_VIEW
#elif CS_HAS_INCLUDE("experimental/string_view")
#	define CS_USE_EXPERIMENTAL_STRING_VIEW
#else
	CS_ERROR("Missing <string_view> on system")
#endif

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
	bool CS_API AppInit();
}

#endif // CS_CORE_H
