// Copyright 2020 author. All rights reserved.

#include "src/message/message_internal.h"

#include <endian.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib/fileio/fileio.h"
#include "lib/osdep/osdep.h"
#include "src/message/message.h"

char *get_field_string(struct msg_field *field) {
    char *field_string = NULL;
    size_t field_string_len;

    if (FIELD_TYPE_STRING != field->type) {
        printf("WARN: wrong field type, expected %d, got %d\n",
               FIELD_TYPE_STRING,
               field->type);
        goto error_out_field_type;
    }

    field_string = (char *)(field + 1);

    field_string_len = strnlen(field_string, field->len);

    if (field_string_len != field->len - 1) {
        printf("WARN: wrong field length, expected %d, got %zu\n",
               field->len - 1,
               field_string_len);
        field_string = NULL;
        goto error_out_field_len;
    }

error_out_field_len:
error_out_field_type:
    return field_string;
}

int get_msg(int fd, void *data, msg_parser parser) {
    struct msg_hdr hdr;
    uint8_t *buf;
    int ret;
    int ret_final = EXIT_FAILURE;

    ret = reliable_read(fd, &hdr, sizeof(struct msg_hdr));
    if (-1 == ret) {
        printf("ERRO: failed to read message header from fd %d\n", fd);
        goto error_out_get_msg_hdr;
    } else if (0 == ret) {
        printf("ERRO: failed to read message header, " \
               "fd %d closed on other end\n", fd);
        goto error_out_get_msg_hdr;
    }

    hdr.type = be16toh(hdr.type);
    hdr.len = be16toh(hdr.len);

    buf = os_malloc(hdr.len);
    if (NULL == buf) {
        printf("ERRO: failed to allocate memory for message buffer\n");
        goto error_out_os_malloc;
    }

    ret = reliable_read(fd, buf, hdr.len);
    if (-1 == ret) {
        printf("ERRO: failed to read message from fd %d\n", fd);
        goto error_out_reliable_read;
    } else if (0 == ret) {
        printf("ERRO: failed to read message, " \
               "fd %d closed on other end\n", fd);
        goto error_out_reliable_read;
    }

    ret = parser(buf, &hdr, data);
    if (EXIT_FAILURE == ret) {
        printf("ERRO: failed to parse message buffer\n");
        goto error_out_parser;
    }

    ret_final = EXIT_SUCCESS;
error_out_parser:
error_out_reliable_read:
    os_free(buf);
error_out_os_malloc:
error_out_get_msg_hdr:
    return ret_final;
}

int parse_msg_new_user(uint8_t *buf, struct msg_hdr *hdr, void *data) {
    struct msg_field *field = (struct msg_field *)buf;
    struct msg_new_user *msg_data = (struct msg_new_user *)data;
    char *name;
    int ret_final = EXIT_FAILURE;

    if (MSG_TYPE_NEW_USER != hdr->type) {
        printf("ERRO: wrong message type: expected %d, got %d\n",
               MSG_TYPE_NEW_USER,
               hdr->type);
        goto error_out_msg_type;
    }

    field->type = be16toh(field->type);
    field->id = be16toh(field->id);
    field->len = be16toh(field->len);
    if (FIELD_ID_NAME != field->id) {
        printf("ERRO: wrong field ID: expected %d, got %d\n",
               FIELD_ID_NAME,
               field->id);
        goto error_out_field_id;
    }

    name = get_field_string(field);
    if (NULL == name) {
        printf("ERRO: failed to get name from new user\n");
        goto error_out_get_field_string;
    }

    msg_data->name = os_malloc(field->len);
    if (NULL == msg_data->name) {
        printf("ERRO: failed to allocate memory for msg_data:name\n");
        goto error_out_malloc;
    }

    strncpy(msg_data->name, get_field_string(field), field->len);

    ret_final = EXIT_SUCCESS;
error_out_malloc:
error_out_get_field_string:
error_out_field_id:
error_out_msg_type:
    return ret_final;
}

int parse_msg_registered_user(uint8_t *buf, struct msg_hdr *hdr, void *data) {
    struct msg_field *field = (struct msg_field *)buf;
    struct msg_registered_user *msg_data = (struct msg_registered_user *)data;
    char *msg;
    int ret_final = EXIT_FAILURE;

    if (MSG_TYPE_REGISTERED_USER != hdr->type) {
        printf("ERRO: wrong message type: expected %d, got %d\n",
               MSG_TYPE_REGISTERED_USER,
               hdr->type);
        goto error_out_msg_type;
    }

    field->type = be16toh(field->type);
    field->id = be16toh(field->id);
    field->len = be16toh(field->len);
    if (FIELD_ID_MSG != field->id) {
        printf("ERRO: wrong field ID: expected %d, got %d\n",
               FIELD_ID_MSG,
               field->id);
        goto error_out_field_id;
    }

    msg = get_field_string(field);
    if (NULL == msg) {
        printf("ERRO: failed to get msg from registered user\n");
        goto error_out_get_field_string;
    }

    msg_data->msg = os_malloc(field->len);
    if (NULL == msg_data->msg) {
        printf("ERRO: failed to allocate memory for msg_data:msg\n");
        goto error_out_malloc;
    }

    strncpy(msg_data->msg, msg, field->len);

    ret_final = EXIT_SUCCESS;
error_out_malloc:
error_out_get_field_string:
error_out_field_id:
error_out_msg_type:
    return ret_final;
}

int send_data(int fd, void *buf, int len) {
    int ret;
    int ret_final = EXIT_FAILURE;

    ret = reliable_write(fd, buf, len);
    if (-1 == ret) {
        printf("ERRO: failed to send data to fd %d\n", fd);
        goto error_out_reliable_write;
    }

    ret_final = EXIT_SUCCESS;
error_out_reliable_write:
    return ret_final;
}
