#ifndef UTIL_COMPAT_H
#define UTIL_COMPAT_H

#include <chrono>
#include <memory>
#include <string>

#ifdef WIN32
#	ifndef NOMINMAX
#		define NOMINMAX
#	endif
#include <winsock2.h>
#include <afunix.h>
#else
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#endif

// clang-format off
#ifndef WIN32
    typedef unsigned int SOCKET;
#   include <errno.h>
#   define WSAGetLastError() errno
#   define WSAEINVAL        EINVAL
#   define WSAEALREADY      EALREADY
#   define WSAEWOULDBLOCK   EWOULDBLOCK
#   define WSAEAGAIN        EAGAIN
#   define WSAEMSGSIZE      EMSGSIZE
#   define WSAEINTR         EINTR
#   define WSAEINPROGRESS   EINPROGRESS
#   define WSAEADDRINUSE    EADDRINUSE
#   define WSAENOTSOCK      EBADF
#   define INVALID_SOCKET   static_cast<SOCKET>(~0)
#   define SOCKET_ERROR     -1
#else
#   ifndef WSAEAGAIN
#       ifdef EAGAIN
#           define WSAEAGAIN EAGAIN
#       else
#           define WSAEAGAIN WSAEWOULDBLOCK
#       endif
#   endif
#endif

#ifdef WIN32
#   ifndef S_IRUSR
#       define S_IRUSR      0400
#       define S_IWUSR      0200
#   endif
#else
#   define MAX_PATH         1024
#endif

#ifdef _MSC_VER
#   if !defined(ssize_t)
#       ifdef __WIN64__
            typedef int64_t ssize_t;
#		else
			typedef int32_t ssize_t;
#		endif
#	endif
#endif

#if !defined(MSG_NOSIGNAL)
#	define MSG_NOSIGNAL 0
#endif

// clang-format on

#endif // UTIL_COMPAT_H