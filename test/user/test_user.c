// Copyright 2020 author. All rights reserved.

#include <stdlib.h>

#include "mocks/mock_message.h"
#include "mocks/mock_osdep.h"
#include "mocks/mock_user_internal.h"
#include "src/user/user.h"
#include "unity/src/unity.h"

void setUp(void) {
    /* set stuff up here */
    mock_message_Init();
    mock_osdep_Init();
    mock_user_internal_Init();
}

void tearDown(void) {
    /* clean stuff up here */
    mock_message_Verify();
    mock_message_Destroy();
    mock_osdep_Verify();
    mock_osdep_Destroy();
    mock_user_internal_Verify();
    mock_user_internal_Destroy();
}

void test_get_num_users(void) {
    int num_users = 1;
    int ret;

    count_num_users_ExpectAnyArgsAndReturn(num_users);
    ret = get_num_users();

    TEST_ASSERT_EQUAL_INT(num_users, ret);
}

void test_get_user_fds(void) {
    int num_users = 2;
    int user_fds[num_users];
    int ret[num_users];

    user_fds[0] = 3;
    user_fds[1] = 4;

    copy_user_fds_Expect(ret, NULL);
    copy_user_fds_IgnoreArg_users();
    copy_user_fds_ReturnMemThruPtr_user_fds(user_fds, sizeof(user_fds));
    get_user_fds(ret);

    TEST_ASSERT_EQUAL_INT_ARRAY(user_fds, ret, num_users);
}

void test_handle_new_user_fail_add_new_user(void) {
    const char *name = "John Doe";
    struct msg_new_user user_msg;
    int fd = 3;
    int ret;

    user_msg.name = (char *)name;

    get_msg_new_user_ExpectAndReturn(fd, NULL, EXIT_SUCCESS);
    get_msg_new_user_IgnoreArg_data();
    add_new_user_ExpectAndReturn(fd, NULL, NULL, NULL);
    add_new_user_IgnoreArg_msg();
    add_new_user_IgnoreArg_users();
    add_new_user_ReturnMemThruPtr_msg(&user_msg, sizeof(struct msg_new_user));
    os_free_Expect(user_msg.name);
    ret = handle_new_user(fd);

    TEST_ASSERT_EQUAL_INT(EXIT_FAILURE, ret);
}

void test_handle_new_user_fail_create_broadcast_msg(void) {
    const char *name = "John Doe";
    struct msg_new_user user_msg;
    struct user_info user;
    int fd = 3;
    int ret;

    user_msg.name = (char *)name;
    user.fd = fd;
    user.name = user_msg.name;
    user.remove_user = false;

    get_msg_new_user_ExpectAndReturn(fd, NULL, EXIT_SUCCESS);
    get_msg_new_user_IgnoreArg_data();
    add_new_user_ExpectAndReturn(fd, NULL, NULL, &user);
    add_new_user_IgnoreArg_msg();
    add_new_user_IgnoreArg_users();
    add_new_user_ReturnMemThruPtr_msg(&user_msg, sizeof(struct msg_new_user));
    create_broadcast_msg_user_joined_ExpectAndReturn(&user, NULL, EXIT_FAILURE);
    create_broadcast_msg_user_joined_IgnoreArg_msg();
    ret = handle_new_user(fd);

    TEST_ASSERT_EQUAL_INT(EXIT_SUCCESS, ret);
}

void test_handle_new_user_fail_get_user_msg(void) {
    int fd = 3;
    int ret;

    get_msg_new_user_ExpectAndReturn(fd, NULL, EXIT_FAILURE);
    get_msg_new_user_IgnoreArg_data();
    ret = handle_new_user(fd);

    TEST_ASSERT_EQUAL_INT(EXIT_FAILURE, ret);
}

void test_handle_new_user_fail_pass(void) {
    const char *name = "John Doe";
    struct msg_new_user user_msg;
    struct user_info user;
    int fd = 3;
    int ret;

    user_msg.name = (char *)name;
    user.fd = fd;
    user.name = user_msg.name;
    user.remove_user = false;

    get_msg_new_user_ExpectAndReturn(fd, NULL, EXIT_SUCCESS);
    get_msg_new_user_IgnoreArg_data();
    add_new_user_ExpectAndReturn(fd, NULL, NULL, &user);
    add_new_user_IgnoreArg_msg();
    add_new_user_IgnoreArg_users();
    add_new_user_ReturnMemThruPtr_msg(&user_msg, sizeof(struct msg_new_user));
    create_broadcast_msg_user_joined_ExpectAndReturn(&user, NULL, EXIT_SUCCESS);
    create_broadcast_msg_user_joined_IgnoreArg_msg();
    send_broadcast_msg_to_users_ExpectAnyArgs();
    os_free_ExpectAnyArgs();
    ret = handle_new_user(fd);

    TEST_ASSERT_EQUAL_INT(EXIT_SUCCESS, ret);
}

void test_handle_registered_user_fail_create_broadcast_msg(void) {
    const char *name = "John Doe";
    struct user_info user;
    int fd = 3;
    int ret;

    user.fd = fd;
    user.name = (char *)name;
    user.remove_user = false;

    get_user_from_fd_ExpectAndReturn(fd, NULL, &user);
    get_user_from_fd_IgnoreArg_users();
    get_msg_registered_user_ExpectAndReturn(fd, NULL, EXIT_FAILURE);
    get_msg_registered_user_IgnoreArg_data();
    create_broadcast_msg_user_left_ExpectAndReturn(&user, NULL, EXIT_FAILURE);
    create_broadcast_msg_user_left_IgnoreArg_msg();
    ret = handle_registered_user(fd);

    TEST_ASSERT_EQUAL_INT(EXIT_FAILURE, ret);
    TEST_ASSERT_TRUE(user.remove_user);
}

void test_handle_registered_user_fail_get_msg(void) {
    const char *name = "John Doe";
    struct user_info user;
    int fd = 3;
    int ret;

    user.fd = fd;
    user.name = (char *)name;
    user.remove_user = false;

    get_user_from_fd_ExpectAndReturn(fd, NULL, &user);
    get_user_from_fd_IgnoreArg_users();
    get_msg_registered_user_ExpectAndReturn(fd, NULL, EXIT_FAILURE);
    get_msg_registered_user_IgnoreArg_data();
    create_broadcast_msg_user_left_ExpectAndReturn(&user, NULL, EXIT_SUCCESS);
    create_broadcast_msg_user_left_IgnoreArg_msg();
    send_broadcast_msg_to_users_ExpectAnyArgs();
    os_free_ExpectAnyArgs();
    ret = handle_registered_user(fd);

    TEST_ASSERT_EQUAL_INT(EXIT_SUCCESS, ret);
    TEST_ASSERT_TRUE(user.remove_user);
}

void test_handle_registered_user_fail_get_user(void) {
    int fd = 3;
    int ret;

    get_user_from_fd_ExpectAndReturn(fd, NULL, NULL);
    get_user_from_fd_IgnoreArg_users();
    ret = handle_registered_user(fd);

    TEST_ASSERT_EQUAL_INT(EXIT_FAILURE, ret);
}

void test_handle_registered_user_pass(void) {
    const char *name = "John Doe";
    struct user_info user;
    int fd = 3;
    int ret;

    user.fd = fd;
    user.name = (char *)name;
    user.remove_user = false;

    get_user_from_fd_ExpectAndReturn(fd, NULL, &user);
    get_user_from_fd_IgnoreArg_users();
    get_msg_registered_user_ExpectAndReturn(fd, NULL, EXIT_SUCCESS);
    get_msg_registered_user_IgnoreArg_data();
    os_timestamp_ExpectAndReturn(0);
    send_broadcast_msg_to_users_ExpectAnyArgs();
    os_free_ExpectAnyArgs();
    ret = handle_registered_user(fd);

    TEST_ASSERT_EQUAL_INT(EXIT_SUCCESS, ret);
    TEST_ASSERT_FALSE(user.remove_user);
}

void test_init_user(void) {
    init_user_list_ExpectAnyArgs();
    init_user();
}

void test_update_user_list(void) {
    remove_flagged_users_ExpectAnyArgs();
    update_user_list();
}

/* not needed when using generate_test_runner.rb */
int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_get_num_users);
    RUN_TEST(test_get_user_fds);
    RUN_TEST(test_handle_new_user_fail_add_new_user);
    RUN_TEST(test_handle_new_user_fail_create_broadcast_msg);
    RUN_TEST(test_handle_new_user_fail_get_user_msg);
    RUN_TEST(test_handle_new_user_fail_pass);
    RUN_TEST(test_handle_registered_user_fail_create_broadcast_msg);
    RUN_TEST(test_handle_registered_user_fail_get_msg);
    RUN_TEST(test_handle_registered_user_fail_get_user);
    RUN_TEST(test_handle_registered_user_pass);
    RUN_TEST(test_init_user);
    RUN_TEST(test_update_user_list);
    return UNITY_END();
}
