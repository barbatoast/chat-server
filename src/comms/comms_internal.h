// Copyright 2020 author. All rights reserved.

#ifndef SRC_COMMS_COMMS_INTERNAL_H_
#define SRC_COMMS_COMMS_INTERNAL_H_

#include <sys/select.h>

/**
 * Process file descriptors.
 *
 * @param listenfd (int) - File descriptor for listen socket.
 * @param max_fd (int) - The max file descriptor.
 * @param rfds (fd_set *) - File descriptor set.
 * @open - Open file descriptor for new user connection.
 * @return (int) - EXIT_SUCCESS on success, EXIT_FAILURE on failure.
 */
int process_fds(int listenfd, int max_fd, fd_set *rfds);

/**
 * Add File descritors to fd set.
 *
 * @param listenfd (int) - File descriptor for listen socket.
 * @param rfds (fd_set *) - File descriptor set.
 * @return (int) - Max file descriptor.
 */
int set_fds(int listenfd, fd_set *rfds);

#endif  // SRC_COMMS_COMMS_INTERNAL_H_
