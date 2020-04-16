// Copyright 2020 author. All rights reserved.

#include "src/message/message.h"

#include <endian.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "src/message/message_internal.h"

int get_msg_new_user(int fd, void *data) {
    return get_msg(fd, data, &parse_msg_new_user);
}

int get_msg_registered_user(int fd, void *data) {
    return get_msg(fd, data, &parse_msg_registered_user);
}

int send_broadcast_msg(int fd, struct msg_broadcast *data) {
    struct msg_hdr hdr;
    struct msg_field field;
    uint32_t timestamp;
    int ret;
    int ret_final = EXIT_FAILURE;

    hdr.type = htobe16(MSG_TYPE_BROADCAST_MSG);
    hdr.len = htobe16(sizeof(struct msg_field)      // timestamp
                      + sizeof(data->timestamp)
                      + sizeof(struct msg_field)    // name
                      + strlen(data->name) + 1
                      + sizeof(struct msg_field)    // msg
                      + strlen(data->msg) + 1);

    ret = send_data(fd, &hdr, sizeof(struct msg_hdr));
    if (EXIT_FAILURE == ret) {
        printf("ERRO: failed to send message header to fd %d\n", fd);
        goto error_out_send_msg_hdr;
    }

    field.type = htobe16(FIELD_TYPE_UINT32);
    field.id = htobe16(FIELD_ID_TIMESTAMP);
    field.len = htobe16(sizeof(data->timestamp));
    ret = send_data(fd, &field, sizeof(struct msg_field));
    if (EXIT_FAILURE == ret) {
        printf(
            "ERRO: failed to send field header for timestamp to fd %d\n",
            fd);
        goto error_out_send_field_hdr_timestamp;
    }

    timestamp = htobe32(data->timestamp);
    ret = send_data(fd, &timestamp, sizeof(timestamp));
    if (EXIT_FAILURE == ret) {
        printf("ERRO: failed to send timestamp to fd %d\n", fd);
        goto error_out_send_timestamp;
    }

    field.type = htobe16(FIELD_TYPE_STRING);
    field.id = htobe16(FIELD_ID_NAME);
    field.len = htobe16(strlen(data->name) + 1);
    ret = send_data(fd, &field, sizeof(struct msg_field));
    if (EXIT_FAILURE == ret) {
        printf("ERRO: failed to send field header for name to fd %d\n", fd);
        goto error_out_send_field_hdr_name;
    }

    ret = send_data(fd, data->name, strlen(data->name) + 1);
    if (EXIT_FAILURE == ret) {
        printf("ERRO: failed to send name to fd %d\n", fd);
        goto error_out_send_name;
    }

    field.type = htobe16(FIELD_TYPE_STRING);
    field.id = htobe16(FIELD_ID_MSG);
    field.len = htobe16(strlen(data->msg) + 1);
    ret = send_data(fd, &field, sizeof(struct msg_field));
    if (EXIT_FAILURE == ret) {
        printf("ERRO: failed to send field header for message to fd %d\n", fd);
        goto error_out_send_field_hdr_msg;
    }

    ret = send_data(fd, data->msg, strlen(data->msg) + 1);
    if (EXIT_FAILURE == ret) {
        printf("ERRO: failed to send message to fd %d\n", fd);
        goto error_out_send_msg;
    }

    ret_final = EXIT_SUCCESS;
error_out_send_msg:
error_out_send_field_hdr_msg:
error_out_send_name:
error_out_send_field_hdr_name:
error_out_send_timestamp:
error_out_send_field_hdr_timestamp:
error_out_send_msg_hdr:
    return ret_final;
}
