// Copyright 2020 author. All rights reserved.

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "include/common.h"
#include "mocks/mock_fileio.h"
#include "mocks/mock_osdep.h"
#include "src/message/message_internal.h"
#include "src/message/message.h"
#include "unity/src/unity.h"

#define MSG_BUF_LEN 256

void setUp(void) {
    /* set stuff up here */
    mock_fileio_Init();
    mock_osdep_Init();
}

void tearDown(void) {
    /* clean stuff up here */
    mock_fileio_Verify();
    mock_fileio_Destroy();
    mock_osdep_Verify();
    mock_osdep_Destroy();
}

int msg_parser_fail(uint8_t *buf, struct msg_hdr *hdr, void *data) {
    UNUSED_PARAMETER(buf);
    UNUSED_PARAMETER(hdr);
    UNUSED_PARAMETER(data);

    return EXIT_FAILURE;
}

int msg_parser_pass(uint8_t *buf, struct msg_hdr *hdr, void *data) {
    const char *name = "John Doe";
    struct msg_new_user *user_info = (struct msg_new_user *)data;

    UNUSED_PARAMETER(buf);
    UNUSED_PARAMETER(hdr);

    user_info->name = malloc(strlen(name) + 1);
    if (NULL == user_info->name) {
        return EXIT_FAILURE;
    }

    memcpy(user_info->name, name, strlen(name));
    user_info->name[strlen(name)] = 0;

    return EXIT_SUCCESS;
}

void test_get_field_string_fail_wrong_field_len_long(void) {
    const char *name = "John Doe";
    uint8_t buf[MSG_BUF_LEN];
    struct msg_field *field = (struct msg_field *)buf;
    uint8_t *field_string = &buf[sizeof(struct msg_field)];
    char *ret;

    field->type = FIELD_TYPE_STRING;
    field->id = FIELD_ID_NAME;
    field->len = strlen(name) + 2;

    memcpy(field_string, name, strlen(name));
    ret = get_field_string((struct msg_field *)buf);

    TEST_ASSERT_NULL(ret);
}

void test_get_field_string_fail_wrong_field_len_short(void) {
    const char *name = "John Doe";
    uint8_t buf[MSG_BUF_LEN];
    struct msg_field *field = (struct msg_field *)buf;
    uint8_t *field_string = &buf[sizeof(struct msg_field)];
    char *ret;

    field->type = FIELD_TYPE_STRING;
    field->id = FIELD_ID_NAME;
    field->len = strlen(name) - 1;

    memcpy(field_string, name, strlen(name));
    ret = get_field_string((struct msg_field *)buf);

    TEST_ASSERT_NULL(ret);
}

void test_get_field_string_fail_wrong_field_type(void) {
    const char *name = "John Doe";
    uint8_t buf[MSG_BUF_LEN];
    struct msg_field *field = (struct msg_field *)buf;
    uint8_t *field_string = &buf[sizeof(struct msg_field)];
    char *ret;

    field->type = FIELD_TYPE_UINT32;
    field->id = FIELD_ID_NAME;
    field->len = strlen(name) + 1;

    memcpy(field_string, name, strlen(name));
    ret = get_field_string((struct msg_field *)buf);

    TEST_ASSERT_NULL(ret);
}

void test_get_field_string_pass(void) {
    const char *name = "John Doe";
    uint8_t buf[MSG_BUF_LEN];
    struct msg_field *field = (struct msg_field *)buf;
    uint8_t *field_string = &buf[sizeof(struct msg_field)];
    char *ret;

    field->type = FIELD_TYPE_STRING;
    field->id = FIELD_ID_NAME;
    field->len = strlen(name) + 1;

    memcpy(field_string, name, strlen(name));
    buf[sizeof(struct msg_field) + strlen(name)] = 0;
    ret = get_field_string((struct msg_field *)buf);

    TEST_ASSERT_NOT_NULL(ret);
    TEST_ASSERT_EQUAL_INT(0, memcmp(ret, name, strlen(name)));
}

void test_get_msg_fail_malloc(void) {
    int fd = 3;
    struct msg_new_user data;
    struct msg_hdr hdr;
    uint16_t msg_len = 1;
    int ret;

    hdr.type = htobe16(MSG_TYPE_NEW_USER);
    hdr.len = htobe16(msg_len);

    reliable_read_ExpectAnyArgsAndReturn(sizeof(struct msg_hdr));
    reliable_read_ReturnMemThruPtr_buf(&hdr, sizeof(hdr));
    os_malloc_ExpectAndReturn(msg_len, NULL);
    reliable_read_ExpectAnyArgsAndReturn(msg_len);
    ret = get_msg(fd, &data, &msg_parser_fail);

    TEST_ASSERT_EQUAL_INT(EXIT_FAILURE, ret);
}

void test_get_msg_fail_msg_parser(void) {
    int fd = 3;
    struct msg_new_user data;
    struct msg_hdr hdr;
    uint16_t msg_len = 1;
    uint8_t buf[msg_len];
    int ret;

    hdr.type = htobe16(MSG_TYPE_NEW_USER);
    hdr.len = htobe16(msg_len);

    reliable_read_ExpectAnyArgsAndReturn(sizeof(struct msg_hdr));
    reliable_read_ReturnMemThruPtr_buf(&hdr, sizeof(hdr));
    os_malloc_ExpectAndReturn(msg_len, &buf);
    reliable_read_ExpectAnyArgsAndReturn(msg_len);
    os_free_Expect(&buf);
    ret = get_msg(fd, &data, &msg_parser_fail);

    TEST_ASSERT_EQUAL_INT(EXIT_FAILURE, ret);
}

void test_get_msg_fail_read_data(void) {
    int fd = 3;
    struct msg_new_user data;
    struct msg_hdr hdr;
    uint16_t msg_len = 1;
    uint8_t buf[msg_len];
    int ret;

    hdr.type = htobe16(MSG_TYPE_NEW_USER);
    hdr.len = htobe16(msg_len);

    reliable_read_ExpectAnyArgsAndReturn(sizeof(struct msg_hdr));
    reliable_read_ReturnMemThruPtr_buf(&hdr, sizeof(hdr));
    os_malloc_ExpectAndReturn(msg_len, &buf);
    reliable_read_ExpectAnyArgsAndReturn(-1);
    os_free_Expect(&buf);
    ret = get_msg(fd, &data, &msg_parser_fail);

    TEST_ASSERT_EQUAL_INT(EXIT_FAILURE, ret);
}

void test_get_msg_fail_read_hdr_error(void) {
    int fd = 3;
    struct msg_new_user data;
    int ret;

    reliable_read_IgnoreAndReturn(-1);
    ret = get_msg(fd, &data, msg_parser_fail);

    TEST_ASSERT_EQUAL_INT(EXIT_FAILURE, ret);
}

void test_get_msg_fail_read_hdr_fd_closed(void) {
    int fd = 3;
    struct msg_new_user data;
    int ret;

    reliable_read_IgnoreAndReturn(0);
    ret = get_msg(fd, &data, msg_parser_fail);

    TEST_ASSERT_EQUAL_INT(EXIT_FAILURE, ret);
}

void test_get_msg_pass(void) {
    const char *name = "John Doe";
    int fd = 3;
    struct msg_new_user data;
    struct msg_hdr hdr;
    uint16_t msg_len = 1;
    uint8_t buf[msg_len];
    int ret;

    memset(&data, 0, sizeof(struct msg_new_user));
    hdr.type = htobe16(MSG_TYPE_NEW_USER);
    hdr.len = htobe16(msg_len);

    reliable_read_ExpectAnyArgsAndReturn(sizeof(struct msg_hdr));
    reliable_read_ReturnMemThruPtr_buf(&hdr, sizeof(hdr));
    os_malloc_ExpectAndReturn(msg_len, &buf);
    reliable_read_ExpectAnyArgsAndReturn(msg_len);
    os_free_Expect(&buf);
    ret = get_msg(fd, &data, &msg_parser_pass);

    TEST_ASSERT_EQUAL_INT(EXIT_SUCCESS, ret);
    TEST_ASSERT_NOT_NULL(data.name);
    TEST_ASSERT_EQUAL_INT(0, memcmp(data.name, name, strlen(name)));
}

void test_parse_msg_new_user_fail_get_name_fail(void) {
    struct msg_field field;
    struct msg_hdr hdr;
    struct msg_new_user data;
    int ret;

    hdr.type = MSG_TYPE_NEW_USER;
    field.type = htobe16(FIELD_TYPE_UINT32);
    field.id = htobe16(FIELD_ID_NAME);

    ret = parse_msg_new_user((uint8_t *)&field, &hdr, &data);

    TEST_ASSERT_EQUAL_INT(EXIT_FAILURE, ret);
}

void test_parse_msg_new_user_fail_malloc(void) {
    const char *name = "John Doe";
    uint8_t buf[MSG_BUF_LEN];
    struct msg_field *field = (struct msg_field *)buf;
    struct msg_hdr hdr;
    struct msg_new_user data;
    uint8_t *field_string = &buf[sizeof(struct msg_field)];
    uint16_t field_len = strlen(name) + 1;
    int ret;

    hdr.type = MSG_TYPE_NEW_USER;
    field->type = htobe16(FIELD_TYPE_STRING);
    field->id = htobe16(FIELD_ID_NAME);
    field->len = htobe16(field_len);

    memcpy(field_string, name, strlen(name));
    buf[sizeof(struct msg_field) + strlen(name)] = 0;

    os_malloc_ExpectAndReturn(field_len, NULL);
    ret = parse_msg_new_user(buf, &hdr, &data);

    TEST_ASSERT_EQUAL_INT(EXIT_FAILURE, ret);
}

void test_parse_msg_new_user_fail_wrong_field_id(void) {
    struct msg_field field;
    struct msg_hdr hdr;
    struct msg_new_user data;
    int ret;

    hdr.type = MSG_TYPE_NEW_USER;
    field.id = htobe16(FIELD_ID_MSG);

    ret = parse_msg_new_user((uint8_t *)&field, &hdr, &data);

    TEST_ASSERT_EQUAL_INT(EXIT_FAILURE, ret);
}

void test_parse_msg_new_user_fail_wrong_msg_type(void) {
    uint8_t buf[4] = {0x01, 0x02, 0x03, 0x00};
    struct msg_hdr hdr;
    struct msg_new_user data;
    int ret;

    hdr.type = MSG_TYPE_REGISTERED_USER;

    ret = parse_msg_new_user(buf, &hdr, &data);

    TEST_ASSERT_EQUAL_INT(EXIT_FAILURE, ret);
}

void test_parse_msg_new_user_pass(void) {
    const char *name = "John Doe";
    uint8_t buf[MSG_BUF_LEN];
    struct msg_field *field = (struct msg_field *)buf;
    struct msg_hdr hdr;
    struct msg_new_user data;
    uint8_t *field_string = &buf[sizeof(struct msg_field)];
    uint16_t field_len = strlen(name) + 1;
    uint8_t malloc_buf[field_len];
    int ret;

    hdr.type = MSG_TYPE_NEW_USER;
    field->type = htobe16(FIELD_TYPE_STRING);
    field->id = htobe16(FIELD_ID_NAME);
    field->len = htobe16(field_len);

    memcpy(field_string, name, strlen(name));
    buf[sizeof(struct msg_field) + strlen(name)] = 0;

    os_malloc_ExpectAndReturn(field_len, &malloc_buf);
    ret = parse_msg_new_user(buf, &hdr, &data);

    TEST_ASSERT_EQUAL_INT(EXIT_SUCCESS, ret);
    TEST_ASSERT_NOT_NULL(data.name);
    TEST_ASSERT_EQUAL_INT(0, memcmp(data.name, name, strlen(name)));
}

void test_parse_msg_registered_user_fail_get_msg_fail(void) {
    struct msg_field field;
    struct msg_hdr hdr;
    struct msg_new_user data;
    int ret;

    hdr.type = MSG_TYPE_REGISTERED_USER;
    field.type = htobe16(FIELD_TYPE_UINT32);
    field.id = htobe16(FIELD_ID_MSG);

    ret = parse_msg_registered_user((uint8_t *)&field, &hdr, &data);

    TEST_ASSERT_EQUAL_INT(EXIT_FAILURE, ret);
}

void test_parse_msg_registered_user_fail_malloc(void) {
    const char *msg = "Hello, World!";
    uint8_t buf[MSG_BUF_LEN];
    struct msg_field *field = (struct msg_field *)buf;
    struct msg_hdr hdr;
    struct msg_registered_user data;
    uint8_t *field_string = &buf[sizeof(struct msg_field)];
    uint16_t field_len = strlen(msg) + 1;
    int ret;

    hdr.type = MSG_TYPE_REGISTERED_USER;
    field->type = htobe16(FIELD_TYPE_STRING);
    field->id = htobe16(FIELD_ID_MSG);
    field->len = htobe16(field_len);

    memcpy(field_string, msg, strlen(msg));
    buf[sizeof(struct msg_field) + strlen(msg)] = 0;

    os_malloc_ExpectAndReturn(field_len, NULL);
    ret = parse_msg_registered_user(buf, &hdr, &data);

    TEST_ASSERT_EQUAL_INT(EXIT_FAILURE, ret);
}

void test_parse_msg_registered_user_fail_wrong_field_id(void) {
    struct msg_field field;
    struct msg_hdr hdr;
    struct msg_new_user data;
    int ret;

    hdr.type = MSG_TYPE_REGISTERED_USER;
    field.id = htobe16(FIELD_ID_NAME);

    ret = parse_msg_registered_user((uint8_t *)&field, &hdr, &data);

    TEST_ASSERT_EQUAL_INT(EXIT_FAILURE, ret);
}

void test_parse_msg_registered_user_fail_wrong_msg_type(void) {
    uint8_t buf[4] = {0x01, 0x02, 0x03, 0x00};
    struct msg_hdr hdr;
    struct msg_new_user data;
    int ret;

    hdr.type = MSG_TYPE_NEW_USER;

    ret = parse_msg_registered_user(buf, &hdr, &data);

    TEST_ASSERT_EQUAL_INT(EXIT_FAILURE, ret);
}

void test_parse_msg_registered_user_pass(void) {
    const char *msg = "Hello, World!";
    uint8_t buf[MSG_BUF_LEN];
    struct msg_field *field = (struct msg_field *)buf;
    struct msg_hdr hdr;
    struct msg_registered_user data;
    uint8_t *field_string = &buf[sizeof(struct msg_field)];
    uint16_t field_len = strlen(msg) + 1;
    uint8_t malloc_buf[field_len];
    int ret;

    hdr.type = MSG_TYPE_REGISTERED_USER;
    field->type = htobe16(FIELD_TYPE_STRING);
    field->id = htobe16(FIELD_ID_MSG);
    field->len = htobe16(field_len);

    memcpy(field_string, msg, strlen(msg));
    buf[sizeof(struct msg_field) + strlen(msg)] = 0;

    os_malloc_ExpectAndReturn(field_len, &malloc_buf);
    ret = parse_msg_registered_user(buf, &hdr, &data);

    TEST_ASSERT_EQUAL_INT(EXIT_SUCCESS, ret);
    TEST_ASSERT_NOT_NULL(data.msg);
    TEST_ASSERT_EQUAL_INT(0, memcmp(data.msg, msg, strlen(msg)));
}

void test_send_data_fail_reliable_write_error(void) {
    int fd = 3;
    uint8_t buf[4] = {0x01, 0x02, 0x03, 0x00};
    reliable_write_ExpectAndReturn(fd, buf, sizeof(buf), -1);
    TEST_ASSERT_EQUAL_INT(EXIT_FAILURE, send_data(fd, buf, sizeof(buf)));
}

void test_send_data_pass(void) {
    int fd = 3;
    uint8_t buf[4] = {0x01, 0x02, 0x03, 0x00};
    reliable_write_ExpectAndReturn(fd, buf, sizeof(buf), 4);
    TEST_ASSERT_EQUAL_INT(EXIT_SUCCESS, send_data(fd, buf, sizeof(buf)));
}

/* not needed when using generate_test_runner.rb */
int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_get_field_string_fail_wrong_field_len_long);
    RUN_TEST(test_get_field_string_fail_wrong_field_len_short);
    RUN_TEST(test_get_field_string_fail_wrong_field_type);
    RUN_TEST(test_get_field_string_pass);
    RUN_TEST(test_get_msg_fail_msg_parser);
    RUN_TEST(test_get_msg_fail_read_data);
    RUN_TEST(test_get_msg_fail_read_hdr_error);
    RUN_TEST(test_get_msg_fail_read_hdr_fd_closed);
    RUN_TEST(test_get_msg_pass);
    RUN_TEST(test_parse_msg_new_user_fail_get_name_fail);
    RUN_TEST(test_parse_msg_new_user_fail_malloc);
    RUN_TEST(test_parse_msg_new_user_fail_wrong_field_id);
    RUN_TEST(test_parse_msg_new_user_fail_wrong_msg_type);
    RUN_TEST(test_parse_msg_new_user_pass);
    RUN_TEST(test_parse_msg_registered_user_fail_get_msg_fail);
    RUN_TEST(test_parse_msg_registered_user_fail_malloc);
    RUN_TEST(test_parse_msg_registered_user_fail_wrong_field_id);
    RUN_TEST(test_parse_msg_registered_user_fail_wrong_msg_type);
    RUN_TEST(test_parse_msg_registered_user_pass);
    RUN_TEST(test_send_data_fail_reliable_write_error);
    RUN_TEST(test_send_data_pass);
    return UNITY_END();
}
