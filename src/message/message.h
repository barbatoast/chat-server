// Copyright 2020 author. All rights reserved.

#ifndef SRC_MESSAGE_MESSAGE_H_
#define SRC_MESSAGE_MESSAGE_H_

#include <stdint.h>

struct msg_new_user {
    char *name;
};

struct msg_registered_user {
    char *msg;
};

struct msg_broadcast {
    uint32_t timestamp;
    char *name;
    char *msg;
};

/**
 * Get message from new user.
 *
 * Get the name of a new user.
 *
 * @param fd (int) - File descriptor of new user.
 * @param data (void *) - Struct to store info of new user.
 * @return (int) - EXIT_SUCCESS on success, EXIT_FAILURE on failure.
 */
int get_msg_new_user(int fd, void *data);

/**
 * Get message from registered user.
 *
 * Get a message from a registered user.
 *
 * @param fd (int) - File descriptor of registered user.
 * @param data (void *) - Struct to store message from registered user.
 * @return (int) - EXIT_SUCCESS on success, EXIT_FAILURE on failure.
 */
int get_msg_registered_user(int fd, void *data);

/**
 * Send a boradcast message to a registered user.
 *
 * @param fd (int) - File descriptor of registered user.
 * @param data (struct msg_broadcast *) - Struct containing broadcast message.
 * @fail - Fail to send message header.
 * @fail - Fail to send field header.
 * @fail - Fail to send timestamp, name or message.
 * @return (int) - EXIT_SUCCESS on success, EXIT_FAILURE on failure.
 */
int send_broadcast_msg(int fd, struct msg_broadcast *data);

#endif  // SRC_MESSAGE_MESSAGE_H_
