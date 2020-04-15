// Copyright 2020 author. All rights reserved.

#ifndef _DEFAULT_SOURCE
#define _DEFAULT_SOURCE
#endif

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "src/comms/comms.h"
#include "src/user/user.h"

static volatile int run;

static void sighandler(int signum) {
    run = 0;
    printf("INFO: received signal %d\n", signum);
}

int main() {
    int listenfd;
    int ret;
    int ret_final = EXIT_FAILURE;
    struct sigaction action;

    listenfd = init_listen_sock();
    if (-1 == listenfd) {
        printf("ERRO: failed to setup listen sock\n");
        goto error_out_listen;
    }

    init_user();

    action.sa_flags = 0;
    action.sa_handler = &sighandler;
    sigemptyset(&action.sa_mask);
    if (-1 == sigaction(SIGINT, &action, NULL)) {
        printf("ERRO: failed to handle SIGINT\n");
        goto error_out_sigaction;
    }

    run = 1;
    while (run) {
        ret = monitor_socks(listenfd);
        if (EXIT_FAILURE == ret) {
            printf("ERRO: failed to monitor sockets\n");
            goto error_out_monitor_socks;
        }
    }

    ret_final = EXIT_SUCCESS;
error_out_monitor_socks:
error_out_sigaction:
error_out_listen:
    if (-1 != listenfd) {
        close(listenfd);
    }
    return ret_final;
}
