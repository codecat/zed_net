/////////////////////////////////////////////////////////////////////////////////////////
//
// zed_net - v0.22 - public domain networking library
// (inspired by the excellent stb libraries: https://github.com/nothings/stb)
//
// This library is intended primarily for use in games and provides a simple wrapper
// around BSD sockets (Winsock 2.2 on Windows). Sockets can be set to be blocking or
// non-blocking.
//
// VERSION HISTORY
//
//    0.22 (7/8/2021) TCP listen socket and non-blocking socket fixes.
//    0.21 (14/01/2021) Win compilation fixes.
//    0.20 (7/28/2019) OSX compilation fixes.
//    0.19 (3/4/2016) TCP added and malloc/free calls removed.
//                     Not backwards compatible. - Ian T. Jacobsen (itjac.me)
//    0.18 (9/13/2015) minor polishing
//    0.17 (8/8/2015) initial release
//
// LICENSE
//
//    This software is in the public domain. Where that dedication is not recognized, you
//    are granted a perpetual, irrevocable license to copy, distribute, and modify this
//    file as you see fit.
//
// USAGE
//
//    #define the symbol ZED_NET_IMPLEMENTATION in *one* C/C++ file before the #include
//    of this file; the implementation will be generated in that file.
//
//    If you define the symbol ZED_NET_STATIC, then the implementation will be private to
//    that file.
//
//    Immediately after this block comment is the "header file" section. This section
//    includes documentation for each API function.
//

#ifndef INCLUDE_ZED_NET_H
#define INCLUDE_ZED_NET_H

#ifdef ZED_NET_STATIC
#define ZED_NET_DEF static
#else
#define ZED_NET_DEF extern
#endif

#ifdef __cplusplus
extern "C" {
#endif

/////////////////////////////////////////////////////////////////////////////////////////
//
// INITIALIZATION AND SHUTDOWN
//

// Get a brief reason for failure
ZED_NET_DEF const char *zed_net_get_error(void);

// Perform platform-specific socket initialization;
// *must* be called before using any other function
//
// Returns 0 on success, -1 otherwise (call 'zed_net_get_error' for more info)
ZED_NET_DEF int zed_net_init(void);

// Perform platform-specific socket de-initialization;
// *must* be called when finished using the other functions
ZED_NET_DEF void zed_net_shutdown(void);

/////////////////////////////////////////////////////////////////////////////////////////
//
// INTERNET ADDRESS API
//

// Represents an internet address usable by sockets
typedef struct {
    unsigned int host;
    unsigned short port;
} zed_net_address_t;

// Obtain an address from a host name and a port
//
// 'host' may contain a decimal formatted IP (such as "127.0.0.1"), a human readable
// name (such as "localhost"), or NULL for the default address
//
// Returns 0 on success, -1 otherwise (call 'zed_net_get_error' for more info)
ZED_NET_DEF int zed_net_get_address(zed_net_address_t *address, const char *host, unsigned short port);

// Converts an address's host name into a decimal formatted string
//
// Returns NULL on failure (call 'zed_net_get_error' for more info)
ZED_NET_DEF const char *zed_net_host_to_str(unsigned int host);

/////////////////////////////////////////////////////////////////////////////////////////
//
// SOCKET HANDLE API
//

// Wraps the system handle for a UDP/TCP socket
typedef struct {
    unsigned long long handle;
    unsigned long non_blocking;
    int ready;
} zed_net_socket_t;

// Closes a previously opened socket
ZED_NET_DEF void zed_net_socket_close(zed_net_socket_t *socket);

// Returns the amount of bytes available to receive on success.
//  returns -1 otherwise.
ZED_NET_DEF int zed_net_socket_available(zed_net_socket_t *remote_socket);

/////////////////////////////////////////////////////////////////////////////////////////
//
// UDP SOCKETS API
//

// Opens a UDP socket and binds it to a specified host and port
// (use 0 to select a random open port)
//
// Socket will not block if 'non-blocking' is non-zero
// Socket will be for broadcasting if 'for_broadcast' is non-zero
//
// Returns 0 on success
// Returns -1 on failure (call 'zed_net_get_error' for more info)
ZED_NET_DEF int zed_net_udp_socket_open(zed_net_socket_t *socket, zed_net_address_t addr, unsigned long non_blocking, unsigned long for_broadcast);

// Sends a specific amount of data to 'destination'
//
// Returns 0 on success, -1 otherwise (call 'zed_net_get_error' for more info)
ZED_NET_DEF int zed_net_udp_socket_send(zed_net_socket_t *socket, zed_net_address_t destination, const void *data, int size);

// Receives a specific amount of data from 'sender'
//
// Returns the number of bytes received, -1 otherwise (call 'zed_net_get_error' for more info)
ZED_NET_DEF int zed_net_udp_socket_receive(zed_net_socket_t *socket, zed_net_address_t *sender, void *data, int size);

/////////////////////////////////////////////////////////////////////////////////////////
//
// TCP SOCKETS API
//

// Opens a TCP socket and binds it to a specified host and port
// (use 0 to select a random open port)
//
// Socket will not block if 'non-blocking' is non-zero
//
// Returns NULL on failure (call 'zed_net_get_error' for more info)
// Socket will listen for incoming connections if 'listen_socket' is non-zero
// Returns 0 on success
// Returns -1 on failure (call 'zed_net_get_error' for more info)
ZED_NET_DEF int zed_net_tcp_socket_open(zed_net_socket_t *socket, zed_net_address_t addr, unsigned long non_blocking, int listen_socket);

// Connect to a remote endpoint
// Returns 0 on success.
//  returns -1 otherwise. (call 'zed_net_get_error' for more info)
ZED_NET_DEF int zed_net_tcp_connect(zed_net_socket_t *socket, zed_net_address_t remote_addr);

// Accept connection
// New remote_socket inherits non-blocking from listening_socket
// Returns 0 on success.
//  if the socket is non-blocking, then this can return 1 if the socket isn't ready
//  if the socket is non_blocking and there was no connection to accept, returns 2
//  returns -1 otherwise. (call 'zed_net_get_error' for more info)
ZED_NET_DEF int zed_net_tcp_accept(zed_net_socket_t *listening_socket, zed_net_socket_t *remote_socket, zed_net_address_t *remote_addr);

// Returns the number of bytes sent on success.
//  if the socket is non-blocking, then this can return 1 if the socket isn't ready
//  returns -1 otherwise. (call 'zed_net_get_error' for more info)
ZED_NET_DEF int zed_net_tcp_socket_send(zed_net_socket_t *remote_socket, const void *data, int size);

// Returns the number of bytes received on success.
// Socket will not remove data from buffer if 'peek' is non-zero
//  if the socket is non-blocking, then this can return 1 if the socket isn't ready
//  returns -1 otherwise. (call 'zed_net_get_error' for more info)
ZED_NET_DEF int zed_net_tcp_socket_receive(zed_net_socket_t *remote_socket, void *data, int size, int peek = 0);

// Blocks until the TCP socket is ready. Only makes sense for non-blocking socket.
// Set poll to 1 to immediately return instead of block.
// Returns 0 on success.
//  returns -1 otherwise. (call 'zed_net_get_error' for more info)
ZED_NET_DEF int zed_net_tcp_make_socket_ready(zed_net_socket_t *socket, int poll);

#ifdef __cplusplus
}
#endif

#endif // INCLUDE_ZED_NET_H

#ifdef ZED_NET_IMPLEMENTATION

#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif
#include <WinSock2.h>
#pragma comment(lib, "wsock32.lib")
#define ZED_NET_SOCKET_ERROR SOCKET_ERROR
#define ZED_NET_INVALID_SOCKET INVALID_SOCKET
#else
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#define ZED_NET_SOCKET_ERROR -1
#define ZED_NET_INVALID_SOCKET -1
#endif

static const char *zed_net__g_error;

static int zed_net__error(const char *message) {
    zed_net__g_error = message;

    return -1;
}

ZED_NET_DEF const char *zed_net_get_error(void) {
    return zed_net__g_error;
}

ZED_NET_DEF int zed_net_init(void) {
#ifdef _WIN32
    WSADATA wsa_data;
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0)
    {
        return zed_net__error("Windows Sockets failed to start");
    }

    return 0;
#else
    return 0;
#endif
}

ZED_NET_DEF void zed_net_shutdown(void) {
#ifdef _WIN32
    WSACleanup();
#endif
}

ZED_NET_DEF int zed_net_get_address(zed_net_address_t *address, const char *host, unsigned short port) {
    if (host == NULL) {
        address->host = INADDR_ANY;
    } else {
        address->host = inet_addr(host);
        if (address->host == INADDR_NONE) {
            struct hostent *hostent = gethostbyname(host);
            if (hostent) {
                memcpy(&address->host, hostent->h_addr, hostent->h_length);
            } else {
                return zed_net__error("Invalid host name");
            }
        }
    }

    address->port = port;

    return 0;
}

ZED_NET_DEF const char *zed_net_host_to_str(unsigned int host) {
    struct in_addr in;
    in.s_addr = host;

    return inet_ntoa(in);
}

ZED_NET_DEF int zed_net_udp_socket_open(zed_net_socket_t *sock, zed_net_address_t addr, unsigned long non_blocking, unsigned long for_broadcast) {
    if (!sock)
        return zed_net__error("Socket is NULL");

    // Create the socket
    sock->handle = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock->handle <= 0) {
        zed_net_socket_close(sock);
        return zed_net__error("Failed to create socket");
    }

    if (for_broadcast) {
        // Enable broadcasting on the socket
        int broadcast = 1;
        setsockopt(sock->handle, SOL_SOCKET, SO_BROADCAST, (const char*)&broadcast, sizeof(broadcast));
    }

    // Bind the socket to the port
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = addr.host;
    address.sin_port = htons(addr.port);

    if (bind(sock->handle, (const struct sockaddr *) &address, sizeof(struct sockaddr_in)) != 0) {
        zed_net_socket_close(sock);
        return zed_net__error("Failed to bind socket");
    }

    // Set the socket to non-blocking if neccessary
    if (non_blocking) {
#ifdef _WIN32
        if (ioctlsocket(sock->handle, FIONBIO, &non_blocking) != 0) {
            zed_net_socket_close(sock);
            return zed_net__error("Failed to set socket to non-blocking");
        }
#else
        if (fcntl(sock->handle, F_SETFL, O_NONBLOCK, non_blocking) != 0) {
            zed_net_socket_close(sock);
            return zed_net__error("Failed to set socket to non-blocking");
        }
#endif
    }

    sock->non_blocking = non_blocking;

    return 0;
}

ZED_NET_DEF int zed_net_tcp_socket_open(zed_net_socket_t *sock, zed_net_address_t addr, unsigned long non_blocking, int listen_socket) {
    if (!sock)
        return zed_net__error("Socket is NULL");

    // Create the socket
    sock->handle = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock->handle <= 0) {
        zed_net_socket_close(sock);
        return zed_net__error("Failed to create socket");
    }

    // Bind the socket to the port
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = addr.host;
    address.sin_port = htons(addr.port);

    if (listen_socket) {
        int yes = 1;
        setsockopt(sock->handle, SOL_SOCKET, SO_REUSEADDR, (const char*)&yes, sizeof(int));
    }

    if (bind(sock->handle, (const struct sockaddr *) &address, sizeof(struct sockaddr_in)) != 0) {
        zed_net_socket_close(sock);
        return zed_net__error("Failed to bind socket");
    }

    // Set the socket to non-blocking if neccessary
    if (non_blocking) {
#ifdef _WIN32
        if (ioctlsocket(sock->handle, FIONBIO, &non_blocking) != 0) {
            zed_net_socket_close(sock);
            return zed_net__error("Failed to set socket to non-blocking");
        }
#else
        if (fcntl(sock->handle, F_SETFL, O_NONBLOCK, non_blocking) != 0) {
            zed_net_socket_close(sock);
            return zed_net__error("Failed to set socket to non-blocking");
        }
#endif
	sock->ready = 0;
    }

    if (listen_socket) {
#ifndef SOMAXCONN
#define SOMAXCONN 10
#endif
		if (listen(sock->handle, SOMAXCONN) != 0) {
            zed_net_socket_close(sock);
            return zed_net__error("Failed make socket listen");
        }

        sock->ready = 1;
    }
    sock->non_blocking = non_blocking;

    return 0;
}

// Returns 1 if it would block, <0 if there's an error.
ZED_NET_DEF int zed_net_check_would_block(zed_net_socket_t *socket) {
    struct timeval timer;
    fd_set writefd;
    int retval;

    if (socket->non_blocking && !socket->ready) {
        FD_ZERO(&writefd);
        FD_SET(socket->handle, &writefd);
        timer.tv_sec = 0;
        timer.tv_usec = 0;
		retval = select(0, NULL, &writefd, NULL, &timer);
        if (retval == 0)
			return 1;
		else if (retval == ZED_NET_SOCKET_ERROR) {
			zed_net_socket_close(socket);
			return zed_net__error("Got socket error from select()");
		}
		socket->ready = 1;
    }

	return 0;
}

ZED_NET_DEF int zed_net_tcp_make_socket_ready(zed_net_socket_t *socket, int poll) {
	if (!socket->non_blocking)
		return 0;
	if (socket->ready)
		return 0;

    fd_set writefd;
    int retval;

    timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;

    FD_ZERO(&writefd);
    FD_SET(socket->handle, &writefd);
	retval = select((int)socket->handle + 1, NULL, &writefd, NULL, poll ? &timeout : NULL);
	if (retval != 1)
		return zed_net__error("Failed to make non-blocking socket ready");

	socket->ready = 1;

	return 0;
}

ZED_NET_DEF int zed_net_tcp_connect(zed_net_socket_t *socket, zed_net_address_t remote_addr) {
    struct sockaddr_in address;
    int retval;

    if (!socket)
        return zed_net__error("Socket is NULL");

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = remote_addr.host;
    address.sin_port = htons(remote_addr.port);

    retval = connect(socket->handle, (const struct sockaddr *) &address, sizeof(address));
#ifdef _WIN32
    if (retval == ZED_NET_SOCKET_ERROR && WSAGetLastError() != WSAEWOULDBLOCK) {
#else
    if (retval == ZED_NET_SOCKET_ERROR) {
#endif
        zed_net_socket_close(socket);
        return zed_net__error("Failed to connect socket");
    }

    return 0;
}

ZED_NET_DEF int zed_net_tcp_accept(zed_net_socket_t *listening_socket, zed_net_socket_t *remote_socket, zed_net_address_t *remote_addr) {
    struct sockaddr_in address;
	int retval;
    unsigned long long handle;

    if (!listening_socket)
        return zed_net__error("Listening socket is NULL");
    if (!remote_socket)
        return zed_net__error("Remote socket is NULL");
    if (!remote_addr)
        return zed_net__error("Address pointer is NULL");

	retval = zed_net_check_would_block(listening_socket);
	if (retval == 1)
		return 1;
	else if (retval)
		return -1;
#ifdef _WIN32
    typedef int socklen_t;
#endif
	socklen_t addrlen = sizeof(address);
	handle = accept(listening_socket->handle, (struct sockaddr *)&address, &addrlen);

	if (handle == ZED_NET_INVALID_SOCKET)
		return 2;

    remote_addr->host = address.sin_addr.s_addr;
    remote_addr->port = ntohs(address.sin_port);
	remote_socket->non_blocking = listening_socket->non_blocking;
	remote_socket->ready = 0;
	remote_socket->handle = handle;

	return 0;
}

ZED_NET_DEF void zed_net_socket_close(zed_net_socket_t *socket) {
    if (!socket) {
        return;
    }

    if (socket->handle) {
#ifdef _WIN32
        closesocket(socket->handle);
#else
        close(socket->handle);
#endif

        socket->handle = 0;
        socket->ready = 0;
        socket->non_blocking = 0;
    }
}

ZED_NET_DEF int zed_net_socket_available(zed_net_socket_t *remote_socket) {
    unsigned long retval;
#ifdef _WIN32
    if (ioctlsocket(remote_socket->handle, FIONREAD, &retval) != 0) {
        //TODO: Use WSAGetLastError() to get more information about the error
        return -1;
    }
#else
    if (ioctl(remote_socket->handle, FIONREAD, &retval) != 0) {
        return -1;
    }
#endif
    return (int)retval;
}

ZED_NET_DEF int zed_net_udp_socket_send(zed_net_socket_t *socket, zed_net_address_t destination, const void *data, int size) {
    if (!socket) {
        return zed_net__error("Socket is NULL");
    }

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = destination.host;
    address.sin_port = htons(destination.port);

    int sent_bytes = sendto(socket->handle, (const char *) data, size, 0, (const struct sockaddr *) &address, sizeof(struct sockaddr_in));
    if (sent_bytes != size) {
        return zed_net__error("Failed to send data");
    }

    return 0;
}

ZED_NET_DEF int zed_net_udp_socket_receive(zed_net_socket_t *socket, zed_net_address_t *sender, void *data, int size) {
    if (!socket) {
        return zed_net__error("Socket is NULL");
    }

#ifdef _WIN32
    typedef int socklen_t;
#endif

    struct sockaddr_in from;
    socklen_t from_length = sizeof(from);

    int received_bytes = recvfrom(socket->handle, (char *) data, size, 0, (struct sockaddr *) &from, &from_length);
    if (received_bytes <= 0) {
        return 0;
    }

    sender->host = from.sin_addr.s_addr;
    sender->port = ntohs(from.sin_port);

    return received_bytes;
}

ZED_NET_DEF int zed_net_tcp_socket_send(zed_net_socket_t *remote_socket, const void *data, int size) {
	int retval;

    if (!remote_socket) {
        return zed_net__error("Socket is NULL");
    }

	retval = zed_net_check_would_block(remote_socket);
	if (retval == 1)
		return 1;
	else if (retval)
		return -1;

    int sent_bytes = send(remote_socket->handle, (const char *) data, size, 0);
    if (sent_bytes < 0) {
        return zed_net__error("Failed to send data");
    }
    return sent_bytes;
}

ZED_NET_DEF int zed_net_tcp_socket_receive(zed_net_socket_t *remote_socket, void *data, int size, int peek) {
	int retval;

    if (!remote_socket) {
        return zed_net__error("Socket is NULL");
    }

	retval = zed_net_check_would_block(remote_socket);
	if (retval == 1)
		return 1;
	else if (retval)
		return -1;

#ifdef _WIN32
    typedef int socklen_t;
#endif

    int received_bytes = recv(remote_socket->handle, (char *) data, size, peek ? MSG_PEEK : 0);
    if (received_bytes < 0) {
      return zed_net__error("Failed to receive data");
    }
    return received_bytes;
}

#endif // ZED_NET_IMPLEMENTATION

// vim: tabstop=4 shiftwidth=4 expandtab
