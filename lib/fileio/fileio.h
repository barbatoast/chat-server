// Copyright 2020 author. All rights reserved.

#ifndef LIB_FILEIO_FILEIO_H_
#define LIB_FILEIO_FILEIO_H_

#include <unistd.h>

int reliable_read(int fd, void *buf, size_t len);

int reliable_write(int fd, void *buf, size_t len);

/**
 * Set File descriptor to nonblocking.
 *
 * @param fd (int) - File descriptor.
 * @return (int) - EXIT_SUCCESS on success, EXIT_FAILURE on failure.
 */
int set_fd_nonblocking(int fd);

#endif  // LIB_FILEIO_FILEIO_H_
