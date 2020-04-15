// Copyright 2020 author. All rights reserved.

#ifndef SRC_COMMS_COMMS_H_
#define SRC_COMMS_COMMS_H_

/**
 * Create listen socket on localhost:1234
 *
 * @open - File descriptor for listen socket.
 * @return (int) - listen socket fd on success, -1 on failure.
 */
int init_listen_sock(void);

/**
 * Monitor listen sock and user file descriptors.
 *
 * @param listenfd (int) - Listen socket fd.
 * @return (int) - EXIT_SUCCESS on success, EXIT_FAILURE on failure.
 */
int monitor_socks(int listenfd);

#endif  // SRC_COMMS_COMMS_H_
