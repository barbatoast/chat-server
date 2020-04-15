// Copyright 2020 author. All rights reserved.

#include "lib/osdep/osdep.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

int os_accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
    return accept(sockfd, addr, addrlen);
}

int os_bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    return bind(sockfd, addr, addrlen);
}

int os_close(int fd) {
    return close(fd);
}

int os_fd_isset(int fd, fd_set *set) {
    return FD_ISSET(fd, set);
}

void os_fd_set(int fd, fd_set *set) {
    FD_SET(fd, set);
}

void os_fd_zero(fd_set *set) {
    FD_ZERO(set);
}

void os_free(void *ptr) {
    free(ptr);
}

int os_get_error(void) {
    return errno;
}

int os_listen(int sockfd, int backlog) {
    return listen(sockfd, backlog);
}

void *os_malloc(size_t size) {
    void *ptr = malloc(size);

    if (NULL != ptr) {
        memset(ptr, 0, size);
    }

    return ptr;
}

void *os_memcpy(void *dest, const void *src, size_t n) {
    return memcpy(dest, src, n);
}

void *os_memset(void *s, int c, size_t n) {
    return memset(s, c, n);
}

int os_select(int nfds,
              fd_set *readfds,
              fd_set *writefds,
              fd_set *exceptfds,
              struct timeval *timeout) {
    return select(nfds, readfds, writefds, exceptfds, timeout);
}

int os_setsockopt(int sockfd,
                  int level,
                  int optname,
                  void *optval,
                  socklen_t optlen) {
    return setsockopt(sockfd, level, optname, optval, optlen);
}

int os_socket(int domain, int type, int protocol) {
    return socket(domain, type, protocol);
}

uint32_t os_timestamp(void) {
    return (uint32_t)time(NULL);
}
