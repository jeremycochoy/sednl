// SEDNL - Copyright (c) 2013 Jeremy S. Cochoy
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from
// the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
//     1. The origin of this software must not be misrepresented; you must not
//        claim that you wrote the original software. If you use this software
//        in a product, an acknowledgment in the product documentation would
//        be appreciated but is not required.
//
//     2. Altered source versions must be plainly marked as such, and must not
//        be misrepresented as being the original software.
//
//     3. This notice may not be removed or altered from any source
//        distribution.

#ifndef NETWORK_HEADER_HPP_
#define NETWORK_HEADER_HPP_

#ifndef MAX_CONNECTIONS
# define MAX_CONNECTIONS 10000
#endif /* !MAX_CONNECTIONS */

#include "SEDNL/Exception.hpp"
#include "SEDNL/Packet.hpp"

#ifdef SEDNL_WINDOWS

#ifdef SEDNL_BACKEND_WSAPOLL
#if _WIN32_WINNT <= 0x0600
#undef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif /* _WIN32_WINNT <= 0x0600 */
#endif /* SEDNL_BACKEND_WSAPOLL */

#include <winsock2.h>
#include <Ws2tcpip.h>
#include <stdio.h>

typedef unsigned long long n_64;
typedef u_long   n_32;
typedef u_short  n_16;

#define close(s) closesocket(s)

#ifdef SEDNL_BACKEND_WSAPOLL

#ifdef __MINGW64__
#define SEDNL_DEF_WSAPOLL
#endif /* __MINGW64__ */
#ifdef __MINGW32__
#define SEDNL_DEF_WSAPOLL
#endif /* __MINGW32__ */

#endif /* SEDNL_BACKEND_WSAPOLL */

#else /* !SEDNL_WINDOWS */

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>

#include <arpa/inet.h>
#include <netinet/in.h>

typedef uint64_t n_64;
typedef uint32_t n_32;
typedef uint16_t n_16;

#endif /* !SEDNL_WINDOWS */

#endif /* !NETWORK_HEADER_HPP */
