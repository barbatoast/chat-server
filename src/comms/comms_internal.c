// Copyright 2020 author. All rights reserved.

#include "src/comms/comms_internal.h"

#include <stdio.h>
#include <stdlib.h>

#include "lib/fileio/fileio.h"
#include "lib/osdep/osdep.h"
#include "src/user/user.h"

int process_fds(int listenfd, int max_fd, fd_set *rfds) {
    int ret_final = EXIT_FAILURE;
    int ret;

    for (int i = 0; i < max_fd + 1; i++) {
        if (os_fd_isset(i, rfds)) {
            if (i == listenfd) {
                int connfd = os_accept(listenfd, NULL, NULL);
                if (-1 == connfd) {
                    printf("WARN: " \
                            "failed to accept connection from new user\n");
                    goto error_out_accept;
                }
                ret = set_fd_nonblocking(connfd);
                if (EXIT_FAILURE == ret) {
                    printf("ERRO: failed to set user fd to nonblocking\n");
                    os_close(connfd);
                    goto error_out_set_fd_nonblocking;
                }
                ret = handle_new_user(connfd);
                if (EXIT_FAILURE == ret) {
                    printf("WARN: failed to handle new user\n");
                    os_close(connfd);
                } else {
                    printf("INFO: new user connected\n");
                }
            } else {
                ret = handle_registered_user(i);
                if (EXIT_FAILURE == ret) {
                    printf("WARN: failed to handle registered user\n");
                }
            }
        }
    }

    ret_final = EXIT_SUCCESS;
error_out_set_fd_nonblocking:
error_out_accept:
    return ret_final;
}

int set_fds(int listenfd, fd_set *rfds) {
    int num_users;
    int *user_fds;
    int max_fd = listenfd;

    os_fd_zero(rfds);
    os_fd_set(listenfd, rfds);

    num_users = get_num_users();
    if (num_users > 0) {
        user_fds = os_malloc(sizeof(int) * num_users);
        if (NULL == user_fds) {
            printf("ERRO: failed to allocate buffer for user fds\n");
            goto error_out_malloc;
        }

        get_user_fds(user_fds);
        for (int i = 0; i < num_users; i++) {
            os_fd_set(user_fds[i], rfds);
            if (user_fds[i] > max_fd) {
                max_fd = user_fds[i];
            }
        }

        os_free(user_fds);
    }

error_out_malloc:
    return max_fd;
}
