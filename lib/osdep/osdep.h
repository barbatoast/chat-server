// Copyright 2020 author. All rights reserved.

#ifndef LIB_OSDEP_OSDEP_H_
#define LIB_OSDEP_OSDEP_H_

#include <stdint.h>
#include <sys/select.h>
#include <sys/socket.h>

/**
 * Wrapper for accept().
 *
 * @param sockfd (int) - The listen socket.
 * @param addr (struct sockaddr *) - Pointer to a sockaddr structure.
 * @param addrlen (socklen_t *) - The size, in bytes, of the structure pointed to by addr.
 * @return (int) - File descriptor on success, -1 on failure.
 */
int os_accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);

/**
 * Wrapper for bind().
 *
 * @param sockfd (int) - File descriptor of socket.
 * @param addr (const struct sockaddr *) - Address to bind to socket.
 * @param addrlen (socklen_t) - The size of the address structure pointed to by addr.
 * @return (int) - 0 on success, -1 on failure.
 */
int os_bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

/**
 * Wrapper for close().
 *
 * @param fd (int) - File descriptor to close.
 * @error - On error, errno is set appropriately.
 * @return (int) - Returns 0 on success, -1 on failure.
 */
int os_close(int fd);

/**
 * Wrapper for FD_ISSET().
 *
 * @param fd (int) - File descriptor.
 * @param set (fd_set *) - File descriptor set.
 * @return (int) - 1 if fd is in set, 0 otherwise.
 */
int os_fd_isset(int fd, fd_set *set);

/**
 * Wrapper for FD_SET().
 *
 * @param fd (int) - File descriptor.
 * @param set (fd_set *) - File descriptor set.
 */
void os_fd_set(int fd, fd_set *set);

/**
 * Wrapper for FD_ZERO().
 *
 * @param set (fd_set *) - File descriptor set.
 */
void os_fd_zero(fd_set *set);

/**
 * Wrapper for free().
 *
 * @param ptr (void *) - Pointer to memory allocated by malloc().
 */
void os_free(void * ptr);

/**
 * Get error number.
 *
 * @return (int) - errno
 */
int os_get_error(void);

/**
 * Wrapper for listen().
 *
 * @param sockfd (int) - File descriptor of socket.
 * @param backlog (int) - Length of queue for pending connections.
 * @return 0 on success, -1 on failure.
 */
int os_listen(int sockfd, int backlog);

/**
 * Wrapper for malloc() and memset().
 *
 * @param size (size_t) - Number of bytes to allocate.
 * @return (void *) - Pointer on success, NULL on failure.
 */
void *os_malloc(size_t size);

/**
 * Wrapper for memcpy().
 *
 * @param dest (void *) - Destination for copied bytes.
 * @param src (void *) - Source of copied bytes.
 * @param n (size_t) - Number of bytes to copy.
 * @return (void *) - A pointer to dest.
 */
void *os_memcpy(void *dest, const void *src, size_t n);

/**
 * Wrapper for memset().
 *
 * @param s (void *) - Pointer to memory.
 * @param c (int) - Byte to fill memory with.
 * @param n (size_t) - Number of bytes to fill.
 * @return (void *) - Pointer to memory area s.
 */
void *os_memset(void *s, int c, size_t n);

/**
 * Wrapper for select().
 *
 * @param nfds (int) - The highest-numbered file descriptor, plus 1.
 * @param readfds (fd_set *) - Read fd set.
 * @param writefds (fd_set *) - Write fd set.
 * @param exceptfds (fd_set *) - Except fd set.
 * @param timeout (struct timeval *) - Timeout for select blocking.
 * @errno - On failure, errno is set.
 * @return (int) - The number of file descriptors ready on success, -1 on failure.
 */
int os_select(int nfds,
              fd_set *readfds,
              fd_set *writefds,
              fd_set *exceptfds,
              struct timeval *timeout);

/**
 * Wrapper for setsockopt().
 *
 * @param sockfd (int) - File descriptor of socket to manipulate.
 * @param level (int) - The level at which the option resides.
 * @param optname (int) - Option specified by name.
 * @param optval (void *) - Buffer to access option values.
 * @param optlen (socklen_t ) - Size of buffer pointed to by optval.
 * @return (int) - 0 on success, -1 on failure.
 */
int os_setsockopt(int sockfd,
                  int level,
                  int optname,
                  void *optval,
                  socklen_t optlen);

/**
 * Wrapper for socket().
 *
 * @param domain (int) - The socket domain.
 * @param type (int) - The socket type.
 * @param protocol (int) - The socket protocol.
 * @return (int) - A file descriptor on success, -1 on failure.
 */
int os_socket(int domain, int type, int protocol);

/**
 * Get current timestamp.
 *
 * @return (uint32_t) - Current timestamp.
 */
uint32_t os_timestamp(void);

#endif  // LIB_OSDEP_OSDEP_H_
