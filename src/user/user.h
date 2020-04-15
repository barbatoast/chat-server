// Copyright 2020 author. All rights reserved.

#ifndef SRC_USER_USER_H_
#define SRC_USER_USER_H_

/**
 * Get the number of users.
 *
 * @return (int) - Number of users.
 */
int get_num_users(void);

/**
 * Get file descriptor for each user.
 *
 * @param user_fds (int *) - Buffer to copy user fds.
 */
void get_user_fds(int *user_fds);

/**
 * Handle new user.
 *
 * @param connfd (int) - File descriptor of new user.
 * @free (char *) - Buffer used to store broadcast message.
 * @fail - Fail to get message from fd.
 * @return (int) - EXIT_SUCCESS on success, EXIT_FAILURE on failure.
 */
int handle_new_user(int connfd);

/**
 * Handle registered user.
 *
 * @param fd (int) - File descriptor of registered user.
 * @free (char *) - Buffer used to store message from user.
 * @fail - Fail to find user for fd.
 * @fail - Fail to get message from fd.
 * @return (int) - EXIT_SUCCESS on success, EXIT_FAILURE on failure.
 */
int handle_registered_user(int fd);

/**
 * Initialize user module.
 */
void init_user(void);

/**
 * Update the user list.
 */
void update_user_list(void);

#endif  // SRC_USER_USER_H_
