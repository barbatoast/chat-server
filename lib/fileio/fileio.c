// Copyright 2020 author. All rights reserved.

#include "lib/fileio/fileio.h"

#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "include/common.h"

#define CHUNK_SIZE 4096
#define BLOCK_LIMIT 5
#define SLEEP_USECONDS 100

int reliable_read(int fd, void *buf, size_t len) {
    uint8_t *buf_iter = (uint8_t *)buf;
    size_t bytes_read = 0;
    size_t chunk;
    ssize_t ret;
    int block_counter = 0;
    int ret_final = -1;

    while (bytes_read < len) {
        chunk = MIN((len - bytes_read), CHUNK_SIZE);
        ret = read(fd, &buf_iter[bytes_read], chunk);

        if (block_counter > BLOCK_LIMIT) {
            printf("ERRO: fd %d blocked too many times on read\n", fd);
            goto error_out_block;
        }

        if (ret < 0) {
            if (EAGAIN == errno) {
                block_counter++;
                usleep(SLEEP_USECONDS);
                continue;
            }
            printf("ERRO: failed to read from fd %d\n", fd);
            goto error_out_read_error;
        } else if (0 == ret) {
            printf("WARN: read zero bytes from %d\n", fd);
            goto error_out_fd_closed;
        }
        bytes_read += ret;
    }

    ret_final = bytes_read;
    return ret_final;
error_out_fd_closed:
    // closed during middle of read?
    // notify calling function?
    ret_final = 0;
error_out_read_error:
error_out_block:
    return ret_final;
}

int reliable_write(int fd, void *buf, size_t len) {
    uint8_t *buf_iter = (uint8_t *)buf;
    size_t bytes_sent = 0;
    size_t chunk;
    ssize_t ret;
    int block_counter = 0;
    int ret_final = -1;

    while (bytes_sent < len) {
        chunk = MIN((len - bytes_sent), CHUNK_SIZE);
        ret = write(fd, &buf_iter[bytes_sent], chunk);

        if (block_counter > BLOCK_LIMIT) {
            printf("ERRO: fd %d blocked too many times on write\n", fd);
            goto error_out_block;
        }

        if (ret < 0) {
            if (EAGAIN == errno) {
                block_counter++;
                usleep(SLEEP_USECONDS);
                continue;
            }

            printf("ERRO: failed to write to fd %d\n", fd);
            goto error_out_write_error;
        }

        bytes_sent += ret;
    }

    ret_final = bytes_sent;

error_out_write_error:
error_out_block:
    return ret_final;
}

int set_fd_nonblocking(int fd) {
    int flags;
    int ret;
    int ret_final = EXIT_FAILURE;

    flags = fcntl(fd, F_GETFL, 0);
    if (-1 == flags) {
        printf("ERRO: failed to get fd flags\n");
        goto error_out_fcntl_get_flags;
    }

    flags |= O_NONBLOCK;

    ret = fcntl(fd, F_SETFL, flags);
    if (-1 == ret) {
        printf("ERRO: failed to set fd flags");
        goto error_out_fcntl_set_flags;
    }

    ret_final = EXIT_SUCCESS;
error_out_fcntl_set_flags:
error_out_fcntl_get_flags:
    return ret_final;
}
