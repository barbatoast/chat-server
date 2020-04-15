// Copyright 2020 author. All rights reserved.

#ifndef SRC_USER_USER_INTERNAL_H_
#define SRC_USER_USER_INTERNAL_H_

#include <stdbool.h>

#include "include/list.h"
#include "src/message/message.h"

struct user_info {
    int fd;
    bool remove_user;
    char *name;
    struct list_head list;
};

/**
 * Add user to registered user list.
 *
 * @param fd (int) - File descriptor of new user.
 * @param msg (struct msg_new_user *) - Message from new user.
 * @param users (struct list_head *) - List of registered users.
 * @malloc (struct user_info *) - Buffer to store info of new user.
 * @return (struct user_info *) - New user on success, NULL on failure.
 */
struct user_info *add_new_user(int fd,
                               struct msg_new_user *msg,
                               struct list_head *users);

/**
 * Copy user file descriptors into buffer.
 *
 * @param user_fds (int *) - Buffer to hold fds.
 * @param users (struct list_head *) - List of registered users.
 */
void copy_user_fds(int *user_fds, struct list_head *users);

/**
 * Count the number of users.
 *
 * @param users (struct list_head *) - List of registered users.
 * @return (int) - Number of users.
 */
int count_num_users(struct list_head *users);

/**
 * Create broadcast message for user joining.
 *
 * @param user (struct user_info *) - User that joined.
 * @param msg (struct msg_broadcast *) - Broadcast message.
 * @malloc msg->msg (char *) - Buffer for broadcast message.
 * @return (int) - EXIT_SUCCESS on success, EXT_FAILURE on failure.
 */
int create_broadcast_msg_user_joined(struct user_info *user,
                                     struct msg_broadcast *msg);

/**
 * Create broadcast message for user leaving.
 *
 * @param user (struct user_info *) - User that left.
 * @param msg (struct msg_broadcast *) - Broadcast message.
 * @malloc msg->msg (char *) - Buffer for broadcast message.
 * @return (int) - EXIT_SUCCESS on success, EXT_FAILURE on failure.
 */
int create_broadcast_msg_user_left(struct user_info *user,
                                   struct msg_broadcast *msg);

/**
 * Get user name from fd
 *
 * @param fd (int) - File descriptor of registerd user.
 * @param users (struct list_head *) - List of registered users.
 * @return (struct user_info *) - User on success, NULL on failure.
 */
struct user_info *get_user_from_fd(int fd, struct list_head *users);

/**
 * Initialize user list
 */
void init_user_list(struct list_head *user);

/**
 * Remove flagged users.
 *
 * @param users (struct list_head *) - List of registered users
 * @free (char *) - Buffer to store user name.
 * @close (int) - User's file descriptor.
 * @free (struct user_info *) - Buffer to store user info.
 */
void remove_flagged_users(struct list_head *users);

/**
 * Send broadcast message to registered users.
 *
 * Flags users for removal that fail to receive broadcast message.
 */
void send_broadcast_msg_to_users(struct msg_broadcast *msg,
                                 struct list_head *users);

#endif  // SRC_USER_USER_INTERNAL_H_
