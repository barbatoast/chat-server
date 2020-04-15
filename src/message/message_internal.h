// Copyright 2020 author. All rights reserved.

#ifndef SRC_MESSAGE_MESSAGE_INTERNAL_H_
#define SRC_MESSAGE_MESSAGE_INTERNAL_H_

#include <stdint.h>

#define MSG_TYPE_NEW_USER 1
#define MSG_TYPE_REGISTERED_USER 2
#define MSG_TYPE_BROADCAST_MSG 3
#define FIELD_TYPE_STRING 1
#define FIELD_TYPE_UINT32 2
#define FIELD_ID_TIMESTAMP 1
#define FIELD_ID_NAME 2
#define FIELD_ID_MSG 3

struct msg_hdr {
    uint16_t type;
    uint16_t len;
    uint8_t flags;
    uint8_t ret;
    uint8_t data[0];
}__attribute__((packed));

struct msg_field {
    uint16_t type;
    uint16_t id;
    uint16_t len;
    uint8_t data[0];
}__attribute__((packed));

typedef int (*msg_parser)(uint8_t *buf, struct msg_hdr *hdr, void *data);

/**
 * Get a string from a field.
 *
 * Confirms that field type is FIELD_TYPE_STRING. Confirms string length.
 * Field header needs to be in host byte order.
 *
 * @param field (struct msg_field *) - Field header for string field.
 * @warn - The field string is not checked for NULL.
 * @fail - Field type is not FIELD_TYPE_STRING.
 * @fail - Field length does not equal string length + NULL byte.
 * @return (char *) - String described by field header, NULL otherwise.
 */
char *get_field_string(struct msg_field *field);

/**
 * Get message.
 *
 * Reads a message from the file descriptor. Passes the data to the caller
 * using the data argument.
 *
 * @param fd (int) - File descriptor to read message from.
 * @param data (void *) - Struct to store message data.
 * @param parser (msg_parser) - Function to parse message.
 * @warn - The message data is not checked for NULL.
 * @fail - Fail to read message header.
 * @fail - Fail to allocate buffer for message data.
 * @fail - Fail to read message data.
 * @fail - Fail to parse message data.
 * @return (int) - EXIT_SUCCESS on success, EXIT_FAILURE on failure.
 */
int get_msg(int fd, void *data, msg_parser parser);

/**
 * Parse message from new user.
 *
 * Assumes the message header is in host byte order. Assumes the message
 * buffer is in network byte order. The message buffer is modified by this
 * function.
 *
 * @param buf (uint8_t *) - Buffer containing message from new user.
 * @param hdr (struct msg_hdr *) - Message header.
 * @param data (void *) - Struct to store message data.
 * @cast data (void *) -> (struct msg_new_user *)
 * @warn - The message buffer length is not validated.
 * @warn - The message buffer, header and data are not checked for NULL.
 * @malloc data->name (char *) - Allocate buffer to store name of new user.
 * @return (int) - EXIT_SUCCESS on success, EXIT_FAILURE on failure.
 */
int parse_msg_new_user(uint8_t *buf, struct msg_hdr *hdr, void *data);

/**
 * Parse message from registered user.
 *
 * Assumes the message header is in host byte order. Assumes the message
 * buffer is in network byte order. The message buffer is modified by this
 * function.
 *
 * @param buf (uint8_t *) - Buffer containing message from registered user.
 * @param hdr (struct msg_hdr *) - Message header.
 * @param data (void *) - Struct to store message data.
 * @cast data (void *) -> (struct msg_registered_user *)
 * @warn - The message buffer length is not validated.
 * @warn - The message buffer, header and data are not checked for NULL.
 * @malloc data->msg (char *) - Allocate buffer to store msg from user.
 * @return (int) - EXIT_SUCCESS on success, EXIT_FAILURE on failure.
 */
int parse_msg_registered_user(uint8_t *buf, struct msg_hdr *hdr, void *data);

/**
 * Send data to fd.
 *
 * @param fd (int) - File descriptor to send data to.
 * @param buf (void *) - Data to send to fd.
 * @param len (int) - Number of bytes to send to fd
 * @warn - The buffer is not check for NULL.
 * @fail - Fail to write to fd.
 * @return (int) - EXIT_SUCCESS on success, EXIT_FAILURE on failure.
 */
int send_data(int fd, void *buf, int len);

#endif  // SRC_MESSAGE_MESSAGE_INTERNAL_H_
