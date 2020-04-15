// Copyright 2020 author. All rights reserved.

#include "src/user/user.h"

#include <stdio.h>
#include <stdlib.h>

#include "include/list.h"
#include "lib/osdep/osdep.h"
#include "src/message/message.h"
#include "src/user/user_internal.h"

struct list_head users;

int get_num_users(void) {
    return count_num_users(&users);
}

void get_user_fds(int *user_fds) {
    copy_user_fds(user_fds, &users);
}

int handle_new_user(int connfd) {
    struct user_info *user;
    struct msg_new_user user_msg;
    struct msg_broadcast broadcast_msg;
    int ret;
    int ret_final = EXIT_FAILURE;

    ret = get_msg_new_user(connfd, &user_msg);
    if (EXIT_FAILURE == ret) {
        printf("ERRO: Failed to get message from new user\n");
        goto error_out_get_msg_new_user;
    }

    user = add_new_user(connfd, &user_msg, &users);
    if (NULL == user) {
        printf("ERRO: failed to add new user to list\n");
        goto error_out_add_new_user;
    }

    ret = create_broadcast_msg_user_joined(user, &broadcast_msg);
    if (EXIT_FAILURE == ret) {
        printf("ERRO: failed to create broadcast message\n");
        goto error_out_create_broadcast_msg;
    }

    send_broadcast_msg_to_users(&broadcast_msg, &users);

    os_free(broadcast_msg.msg);
error_out_create_broadcast_msg:
    printf("INFO: registered new user %s\n", user->name);
    ret_final = EXIT_SUCCESS;
    return ret_final;   // don't free user or name
error_out_add_new_user:
    os_free(user_msg.name);
error_out_get_msg_new_user:
    return ret_final;
}

int handle_registered_user(int fd) {
    int ret;
    struct msg_registered_user user_msg;
    struct msg_broadcast broadcast_msg;
    struct user_info* user;
    int ret_final = EXIT_FAILURE;

    user = get_user_from_fd(fd, &users);
    if (NULL == user) {
        printf("ERRO: Cannot find user for fd %d\n", fd);
        goto error_out_get_user_from_fd;
    }

    ret = get_msg_registered_user(user->fd, &user_msg);
    if (EXIT_FAILURE == ret) {
        printf("WARN: failed to get message from fd %d\n", fd);
        printf("INFO: removing user %s\n", user->name);
        user->remove_user = true;
        ret = create_broadcast_msg_user_left(user, &broadcast_msg);
        if (EXIT_FAILURE == ret) {
            printf("ERRO: failed to create broadcast message\n");
            goto error_out_create_broadcast_msg;
        }
    } else {
        broadcast_msg.name = user->name;
        broadcast_msg.msg = user_msg.msg;
        broadcast_msg.timestamp = os_timestamp();
    }

    send_broadcast_msg_to_users(&broadcast_msg, &users);

    // broadcast message and user message are the same
    os_free(broadcast_msg.msg);

    ret_final = EXIT_SUCCESS;
error_out_create_broadcast_msg:
error_out_get_user_from_fd:
    return ret_final;
}

void init_user(void) {
    init_user_list(&users);
}

void update_user_list(void) {
    remove_flagged_users(&users);
}
