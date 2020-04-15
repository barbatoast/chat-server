// Copyright 2020 author. All rights reserved.

#include "src/user/user_internal.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/list.h"
#include "lib/osdep/osdep.h"
#include "src/message/message.h"

char broadcast_banner[10] = {
    0x42, 0x52, 0x4f, 0x41, 0x44, 0x43, 0x41, 0x53,
    0x54, 0x00
};

char user_join_msg[24] = {
    0x20, 0x68, 0x61, 0x73, 0x20, 0x6a, 0x6f, 0x69,
    0x6e, 0x65, 0x64, 0x20, 0x74, 0x68, 0x65, 0x20,
    0x63, 0x68, 0x61, 0x6e, 0x6e, 0x65, 0x6c, 0x00
};

char user_left_msg[22] = {
    0x20, 0x68, 0x61, 0x73, 0x20, 0x6c, 0x65, 0x66,
    0x74, 0x20, 0x74, 0x68, 0x65, 0x20, 0x63, 0x68,
    0x61, 0x6e, 0x6e, 0x65, 0x6c, 0x00
};

struct user_info *add_new_user(int fd,
                          struct msg_new_user *msg,
                          struct list_head *users) {
    struct user_info *user;

    user = os_malloc(sizeof(struct user_info));
    if (NULL == user) {
        printf("ERRO: failed to allocate memory for new user\n");
        goto error_out_os_malloc_user;
    }

    user->fd = fd;
    user->remove_user = false;
    user->name = msg->name;
    list_add(&user->list, users);

error_out_os_malloc_user:
    return user;
}

void copy_user_fds(int *user_fds, struct list_head *users) {
    struct user_info *cur_user = NULL;
    struct user_info *n = NULL;
    struct list_head tmp;
    int i = 0;

    INIT_LIST_HEAD(&tmp);
    list_splice_init(users, &tmp);

    list_for_each_entry_safe(cur_user, n, &tmp, list) {
        user_fds[i] = cur_user->fd;
        i++;
    }

    list_splice(&tmp, users);
}

int count_num_users(struct list_head *users) {
    struct user_info *cur_user = NULL;
    struct user_info *n = NULL;
    struct list_head tmp;
    int count = 0;

    INIT_LIST_HEAD(&tmp);
    list_splice_init(users, &tmp);

    list_for_each_entry_safe(cur_user, n, &tmp, list) {
        count++;
    }

    list_splice(&tmp, users);

    return count;
}

int create_broadcast_msg_user_joined(struct user_info *user,
                                     struct msg_broadcast *msg) {
    int ret_final = EXIT_FAILURE;

    msg->msg = os_malloc(strlen(user->name)
                         + sizeof(user_join_msg));
    if (NULL == msg->msg) {
        printf("WARN: failed to allocate memory for broadcast message\n");
        goto error_out_os_malloc_msg;
    }

    os_memcpy(msg->msg, user->name, strlen(user->name));
    os_memcpy(&msg->msg[strlen(user->name)],
              user_join_msg,
              sizeof(user_join_msg));

    msg->name = broadcast_banner;
    msg->timestamp = os_timestamp();

    ret_final = EXIT_SUCCESS;
error_out_os_malloc_msg:
    return ret_final;
}

int create_broadcast_msg_user_left(struct user_info *user,
                                   struct msg_broadcast *msg) {
    int ret_final = EXIT_FAILURE;

    msg->msg = os_malloc(strlen(user->name)
                                + sizeof(user_left_msg));
    if (NULL == msg->msg) {
        printf("ERRO: failed to allocate memory for broadcast message\n");
        goto error_out_os_malloc;
    }

    os_memcpy(msg->msg, user->name, strlen(user->name));
    os_memcpy(&msg->msg[strlen(user->name)],
              user_left_msg,
              sizeof(user_left_msg));

    msg->name = broadcast_banner;
    msg->timestamp = os_timestamp();

    ret_final = EXIT_SUCCESS;
error_out_os_malloc:
    return ret_final;
}

struct user_info *get_user_from_fd(int fd, struct list_head *users) {
    struct user_info *cur_user = NULL;
    struct user_info *n = NULL;
    struct list_head tmp;
    struct user_info *matched_user = NULL;

    INIT_LIST_HEAD(&tmp);
    list_splice_init(users, &tmp);

    list_for_each_entry_safe(cur_user, n, &tmp, list) {
        if (cur_user->fd == fd) {
            matched_user = cur_user;
            break;
        }
    }

    list_splice(&tmp, users);

    return matched_user;
}

void init_user_list(struct list_head *users) {
    INIT_LIST_HEAD(users);
}

void remove_flagged_users(struct list_head *users) {
    struct user_info *cur_user = NULL;
    struct user_info *n = NULL;
    struct list_head tmp;

    INIT_LIST_HEAD(&tmp);
    list_splice_init(users, &tmp);

    list_for_each_entry_safe(cur_user, n, &tmp, list) {
        if (true == cur_user->remove_user) {
            os_close(cur_user->fd);
            os_free(cur_user->name);
            list_del(&cur_user->list);
            os_free(cur_user);
        }
    }

    list_splice(&tmp, users);
}

void send_broadcast_msg_to_users(struct msg_broadcast *msg,
                                 struct list_head *users) {
    struct user_info *cur_user = NULL;
    struct user_info *n = NULL;
    struct list_head tmp;

    INIT_LIST_HEAD(&tmp);
    list_splice_init(users, &tmp);

    list_for_each_entry_safe(cur_user, n, &tmp, list) {
        if (false == cur_user->remove_user) {
            int ret = send_broadcast_msg(cur_user->fd, msg);
            if (EXIT_FAILURE == ret) {
                printf("WARN: failed to send message to fd %d\n", cur_user->fd);
                cur_user->remove_user = true;
            }
        }
    }

    list_splice(&tmp, users);
}
