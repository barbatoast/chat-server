// Copyright 2020 author. All rights reserved.

#include <stdlib.h>

#include "include/common.h"
#include "mocks/mock_message_internal.h"
#include "src/message/message.h"
#include "unity/src/unity.h"

void setUp(void) {
    /* set stuff up here */
    mock_message_internal_Init();
}

void tearDown(void) {
    /* clean stuff up here */
    mock_message_internal_Verify();
    mock_message_internal_Destroy();
}

void test_get_msg_new_user_fail(void) {
    int fd = 3;
    struct msg_new_user data;
    int ret;

    get_msg_IgnoreAndReturn(EXIT_FAILURE);
    ret = get_msg_new_user(fd, &data);

    TEST_ASSERT_EQUAL_INT(EXIT_FAILURE, ret);
}

void test_get_msg_new_user_pass(void) {
    int fd = 3;
    struct msg_new_user data;
    int ret;

    get_msg_IgnoreAndReturn(EXIT_SUCCESS);
    ret = get_msg_new_user(fd, &data);

    TEST_ASSERT_EQUAL_INT(EXIT_SUCCESS, ret);
}

void test_get_msg_registered_user_fail(void) {
    int fd = 3;
    struct msg_registered_user data;
    int ret;

    get_msg_IgnoreAndReturn(EXIT_FAILURE);
    ret = get_msg_registered_user(fd, &data);

    TEST_ASSERT_EQUAL_INT(EXIT_FAILURE, ret);
}

void test_get_msg_registered_user_pass(void) {
    int fd = 3;
    struct msg_registered_user data;
    int ret;

    get_msg_IgnoreAndReturn(EXIT_SUCCESS);
    ret = get_msg_registered_user(fd, &data);

    TEST_ASSERT_EQUAL_INT(EXIT_SUCCESS, ret);
}

void test_send_broadcast_msg_fail_1st_send(void) {
    int fd = 3;
    struct msg_broadcast data;
    int ret;

    data.timestamp = 0;
    data.name = (char *)"John Doe";
    data.msg = (char *)"Hello, World!";

    send_data_ExpectAnyArgsAndReturn(EXIT_FAILURE);
    ret = send_broadcast_msg(fd, &data);

    TEST_ASSERT_EQUAL_INT(EXIT_FAILURE, ret);
}

void test_send_broadcast_msg_fail_2nd_send(void) {
    int fd = 3;
    struct msg_broadcast data;
    int ret;

    data.timestamp = 0;
    data.name = (char *)"John Doe";
    data.msg = (char *)"Hello, World!";

    send_data_ExpectAnyArgsAndReturn(EXIT_SUCCESS);
    send_data_ExpectAnyArgsAndReturn(EXIT_FAILURE);
    ret = send_broadcast_msg(fd, &data);

    TEST_ASSERT_EQUAL_INT(EXIT_FAILURE, ret);
}

void test_send_broadcast_msg_fail_3rd_send(void) {
    int fd = 3;
    struct msg_broadcast data;
    int ret;

    data.timestamp = 0;
    data.name = (char *)"John Doe";
    data.msg = (char *)"Hello, World!";

    send_data_ExpectAnyArgsAndReturn(EXIT_SUCCESS);
    send_data_ExpectAnyArgsAndReturn(EXIT_SUCCESS);
    send_data_ExpectAnyArgsAndReturn(EXIT_FAILURE);
    ret = send_broadcast_msg(fd, &data);

    TEST_ASSERT_EQUAL_INT(EXIT_FAILURE, ret);
}

void test_send_broadcast_msg_fail_4th_send(void) {
    int fd = 3;
    struct msg_broadcast data;
    int ret;

    data.timestamp = 0;
    data.name = (char *)"John Doe";
    data.msg = (char *)"Hello, World!";

    send_data_ExpectAnyArgsAndReturn(EXIT_SUCCESS);
    send_data_ExpectAnyArgsAndReturn(EXIT_SUCCESS);
    send_data_ExpectAnyArgsAndReturn(EXIT_SUCCESS);
    send_data_ExpectAnyArgsAndReturn(EXIT_FAILURE);
    ret = send_broadcast_msg(fd, &data);

    TEST_ASSERT_EQUAL_INT(EXIT_FAILURE, ret);
}

void test_send_broadcast_msg_fail_5th_send(void) {
    int fd = 3;
    struct msg_broadcast data;
    int ret;

    data.timestamp = 0;
    data.name = (char *)"John Doe";
    data.msg = (char *)"Hello, World!";

    send_data_ExpectAnyArgsAndReturn(EXIT_SUCCESS);
    send_data_ExpectAnyArgsAndReturn(EXIT_SUCCESS);
    send_data_ExpectAnyArgsAndReturn(EXIT_SUCCESS);
    send_data_ExpectAnyArgsAndReturn(EXIT_SUCCESS);
    send_data_ExpectAnyArgsAndReturn(EXIT_FAILURE);
    ret = send_broadcast_msg(fd, &data);

    TEST_ASSERT_EQUAL_INT(EXIT_FAILURE, ret);
}

void test_send_broadcast_msg_fail_6th_send(void) {
    int fd = 3;
    struct msg_broadcast data;
    int ret;

    data.timestamp = 0;
    data.name = (char *)"John Doe";
    data.msg = (char *)"Hello, World!";

    send_data_ExpectAnyArgsAndReturn(EXIT_SUCCESS);
    send_data_ExpectAnyArgsAndReturn(EXIT_SUCCESS);
    send_data_ExpectAnyArgsAndReturn(EXIT_SUCCESS);
    send_data_ExpectAnyArgsAndReturn(EXIT_SUCCESS);
    send_data_ExpectAnyArgsAndReturn(EXIT_SUCCESS);
    send_data_ExpectAnyArgsAndReturn(EXIT_FAILURE);
    ret = send_broadcast_msg(fd, &data);

    TEST_ASSERT_EQUAL_INT(EXIT_FAILURE, ret);
}

void test_send_broadcast_msg_fail_7th_send(void) {
    int fd = 3;
    struct msg_broadcast data;
    int ret;

    data.timestamp = 0;
    data.name = (char *)"John Doe";
    data.msg = (char *)"Hello, World!";

    send_data_ExpectAnyArgsAndReturn(EXIT_SUCCESS);
    send_data_ExpectAnyArgsAndReturn(EXIT_SUCCESS);
    send_data_ExpectAnyArgsAndReturn(EXIT_SUCCESS);
    send_data_ExpectAnyArgsAndReturn(EXIT_SUCCESS);
    send_data_ExpectAnyArgsAndReturn(EXIT_SUCCESS);
    send_data_ExpectAnyArgsAndReturn(EXIT_SUCCESS);
    send_data_ExpectAnyArgsAndReturn(EXIT_FAILURE);
    ret = send_broadcast_msg(fd, &data);

    TEST_ASSERT_EQUAL_INT(EXIT_FAILURE, ret);
}

void test_send_broadcast_msg_pass(void) {
    int fd = 3;
    struct msg_broadcast data;
    int ret;

    data.timestamp = 0;
    data.name = (char *)"John Doe";
    data.msg = (char *)"Hello, World!";

    send_data_ExpectAnyArgsAndReturn(EXIT_SUCCESS);
    send_data_ExpectAnyArgsAndReturn(EXIT_SUCCESS);
    send_data_ExpectAnyArgsAndReturn(EXIT_SUCCESS);
    send_data_ExpectAnyArgsAndReturn(EXIT_SUCCESS);
    send_data_ExpectAnyArgsAndReturn(EXIT_SUCCESS);
    send_data_ExpectAnyArgsAndReturn(EXIT_SUCCESS);
    send_data_ExpectAnyArgsAndReturn(EXIT_SUCCESS);
    ret = send_broadcast_msg(fd, &data);

    TEST_ASSERT_EQUAL_INT(EXIT_SUCCESS, ret);
}

/* not needed when using generate_test_runner.rb */
int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_get_msg_new_user_fail);
    RUN_TEST(test_get_msg_new_user_pass);
    RUN_TEST(test_get_msg_registered_user_fail);
    RUN_TEST(test_get_msg_registered_user_pass);
    RUN_TEST(test_send_broadcast_msg_fail_1st_send);
    RUN_TEST(test_send_broadcast_msg_fail_2nd_send);
    RUN_TEST(test_send_broadcast_msg_fail_3rd_send);
    RUN_TEST(test_send_broadcast_msg_fail_4th_send);
    RUN_TEST(test_send_broadcast_msg_fail_5th_send);
    RUN_TEST(test_send_broadcast_msg_fail_6th_send);
    RUN_TEST(test_send_broadcast_msg_fail_7th_send);
    RUN_TEST(test_send_broadcast_msg_pass);
    return UNITY_END();
}
