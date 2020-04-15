// Copyright 2020 author. All rights reserved.

#include "src/comms/comms.h"

#include <endian.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/socket.h>

#include "lib/osdep/osdep.h"
#include "src/comms/comms_internal.h"
#include "src/user/user.h"

int init_listen_sock() {
    int listenfd;
    int reuse = 1;
    struct sockaddr_in servaddr;
    int ret;

    // listen on localhost:1234 for TCP connections
    listenfd = os_socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == listenfd) {
        printf("ERRO: failed to crate socket\n");
        goto error_out_socket;
    }

    ret = os_setsockopt(listenfd,
                        SOL_SOCKET,
                        SO_REUSEADDR,
                        &reuse,
                        sizeof(reuse));
    if (-1 == ret) {
        printf("ERRO: failed to set socket option\n");
        goto error_out_setsockopt;
    }

    os_memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htobe32(INADDR_LOOPBACK);
    servaddr.sin_port = htobe16(1234);

    ret = os_bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    if (-1 == ret) {
        printf("ERRO: failed to bind to socket\n");
        goto error_out_bind;
    }

    if (-1 == os_listen(listenfd, 5)) {
        printf("ERRO: failed to listen on socket\n");
        goto error_out_listen;
    }

    return listenfd;
error_out_listen:
error_out_bind:
error_out_setsockopt:
error_out_socket:
    return -1;
}

int monitor_socks(int listenfd) {
    fd_set rfds;
    int max_fd;
    int ret;
    int ret_final = EXIT_FAILURE;

    max_fd = set_fds(listenfd, &rfds);

    ret = os_select(max_fd + 1, &rfds, NULL, NULL, NULL);

    if (-1 == ret) {
        if (EINTR == os_get_error()) {
            goto error_out_select_eintr;
        }
        goto error_out_select;
    }

    ret = process_fds(listenfd, max_fd, &rfds);
    if (EXIT_FAILURE == ret) {
        printf("Failed to process file descriptors\n");
        goto error_out_process_fds;
    }

    update_user_list();

error_out_select_eintr:
    ret_final = EXIT_SUCCESS;
error_out_process_fds:
error_out_select:
    return ret_final;
}
