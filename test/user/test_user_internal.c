// Copyright 2020 author. All rights reserved.

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "include/common.h"
#include "mocks/mock_message.h"
#include "mocks/mock_osdep.h"
#include "src/user/user_internal.h"
#include "unity/src/unity.h"

char test_user_join_msg[24] = {
    0x20, 0x68, 0x61, 0x73, 0x20, 0x6a, 0x6f, 0x69,
    0x6e, 0x65, 0x64, 0x20, 0x74, 0x68, 0x65, 0x20,
    0x63, 0x68, 0x61, 0x6e, 0x6e, 0x65, 0x6c, 0x00
};

char test_user_left_msg[22] = {
    0x20, 0x68, 0x61, 0x73, 0x20, 0x6c, 0x65, 0x66,
    0x74, 0x20, 0x74, 0x68, 0x65, 0x20, 0x63, 0x68,
    0x61, 0x6e, 0x6e, 0x65, 0x6c, 0x00
};

void setUp(void) {
    /* set stuff up here */
    mock_message_Init();
    mock_osdep_Init();
}

void tearDown(void) {
    /* clean stuff up here */
    mock_message_Verify();
    mock_message_Destroy();
    mock_osdep_Verify();
    mock_osdep_Destroy();
}

int send_broadcast_msg_dont_call(int fd,
                                 struct msg_broadcast *data,
                                 int num_calls) {
    UNUSED_PARAMETER(fd);
    UNUSED_PARAMETER(data);
    UNUSED_PARAMETER(num_calls);

    TEST_FAIL_MESSAGE("send_broadcast_msg() should not have been called.");
    return EXIT_SUCCESS;
}

void test_add_new_user_fail_malloc(void) {
    int fd = 3;
    const char *name = "John Doe";
    struct msg_new_user msg;
    struct list_head users;
    struct user_info *ret;

    msg.name = (char *)name;

    INIT_LIST_HEAD(&users);
    TEST_ASSERT_TRUE(list_empty(&users));

    os_malloc_ExpectAndReturn(sizeof(struct user_info), NULL);
    ret = add_new_user(fd, &msg, &users);

    TEST_ASSERT_NULL(ret);
    TEST_ASSERT_TRUE(list_empty(&users));
}

void test_add_new_user_pass(void) {
    int fd = 3;
    const char *name = "John Doe";
    struct msg_new_user msg;
    struct user_info user_buf;
    struct list_head users;
    struct user_info *ret;

    msg.name = (char *)name;

    INIT_LIST_HEAD(&users);
    TEST_ASSERT_TRUE(list_empty(&users));

    os_malloc_ExpectAndReturn(sizeof(struct user_info), &user_buf);
    ret = add_new_user(fd, &msg, &users);

    TEST_ASSERT_NOT_NULL(ret);
    TEST_ASSERT_FALSE(list_empty(&users));
    TEST_ASSERT_EQUAL_INT(fd, ret->fd);
    TEST_ASSERT_FALSE(ret->remove_user);
    TEST_ASSERT_EQUAL_CHAR_ARRAY(msg.name, ret->name, strlen(msg.name));
}

void test_copy_user_fds_pass(void) {
    int fd_user1 = 3;
    int fd_user2 = 4;
    const char *name1 = "John Doe";
    const char *name2 = "Jane Doe";
    struct user_info user1;
    struct user_info user2;
    struct list_head users;
    int users_fds[2];

    user1.fd = fd_user1;
    user1.remove_user = true;
    user1.name = (char *)name1;

    user2.fd = fd_user2;
    user2.remove_user = false;
    user2.name = (char *)name2;

    INIT_LIST_HEAD(&users);
    list_add(&user1.list, &users);
    list_add(&user2.list, &users);

    copy_user_fds(users_fds, &users);

    TEST_ASSERT_EQUAL_INT(fd_user2, users_fds[0]);
    TEST_ASSERT_EQUAL_INT(fd_user1, users_fds[1]);
}

void test_count_num_users_pass_count_0(void) {
    struct list_head users;
    int ret;

    INIT_LIST_HEAD(&users);
    ret = count_num_users(&users);

    TEST_ASSERT_EQUAL_INT(0, ret);
}

void test_count_num_users_pass_count_2(void) {
    struct user_info user1;
    struct user_info user2;
    struct list_head users;
    int ret;

    INIT_LIST_HEAD(&users);
    list_add(&user1.list, &users);
    list_add(&user2.list, &users);

    ret = count_num_users(&users);

    TEST_ASSERT_EQUAL_INT(2, ret);
}

void test_create_broadcast_msg_user_joined_fail_malloc(void) {
    struct msg_broadcast msg;
    struct user_info user;
    const char *name = "John Doe";
    int broadcast_msg_len = strlen(name) + sizeof(test_user_join_msg);
    int ret;

    user.name = (char *)name;

    os_malloc_ExpectAndReturn(broadcast_msg_len, NULL);
    ret = create_broadcast_msg_user_joined(&user, &msg);

    TEST_ASSERT_EQUAL_INT(EXIT_FAILURE, ret);
}

void test_create_broadcast_msg_user_joined_pass(void) {
    struct msg_broadcast msg;
    struct user_info user;
    const char *name = "John Doe";
    int broadcast_msg_len = strlen(name) + sizeof(test_user_join_msg);
    uint8_t msg_buf[broadcast_msg_len];
    int ret;

    user.name = (char *)name;

    os_malloc_ExpectAndReturn(broadcast_msg_len, msg_buf);
    os_memcpy_ExpectAndReturn(msg_buf,
                              user.name,
                              strlen(user.name),
                              msg_buf);
    os_memcpy_ReturnMemThruPtr_dest(user.name,
                                    strlen(user.name));
    os_memcpy_ExpectAndReturn(&msg_buf[strlen(user.name)],
                              test_user_join_msg,
                              sizeof(test_user_join_msg),
                              &msg_buf[strlen(user.name)]);
    os_memcpy_IgnoreArg_src();
    os_memcpy_ReturnMemThruPtr_dest(test_user_join_msg,
                                    sizeof(test_user_join_msg));
    os_timestamp_ExpectAndReturn(0);
    ret = create_broadcast_msg_user_joined(&user, &msg);

    TEST_ASSERT_EQUAL_INT(EXIT_SUCCESS, ret);
}

void test_create_broadcast_msg_user_left_fail_malloc(void) {
    struct msg_broadcast msg;
    struct user_info user;
    const char *name = "John Doe";
    int broadcast_msg_len = strlen(name) + sizeof(test_user_left_msg);
    int ret;

    user.name = (char *)name;

    os_malloc_ExpectAndReturn(broadcast_msg_len, NULL);
    ret = create_broadcast_msg_user_left(&user, &msg);

    TEST_ASSERT_EQUAL_INT(EXIT_FAILURE, ret);
}

void test_create_broadcast_msg_user_left_pass(void) {
    struct msg_broadcast msg;
    struct user_info user;
    const char *name = "John Doe";
    int broadcast_msg_len = strlen(name) + sizeof(test_user_left_msg);
    uint8_t msg_buf[broadcast_msg_len];
    int ret;

    user.name = (char *)name;

    os_malloc_ExpectAndReturn(broadcast_msg_len, msg_buf);
    os_memcpy_ExpectAndReturn(msg_buf,
                              user.name,
                              strlen(user.name),
                              msg_buf);
    os_memcpy_ReturnMemThruPtr_dest(user.name,
                                    strlen(user.name));
    os_memcpy_ExpectAndReturn(&msg_buf[strlen(user.name)],
                              test_user_left_msg,
                              sizeof(test_user_left_msg),
                              &msg_buf[strlen(user.name)]);
    os_memcpy_IgnoreArg_src();
    os_memcpy_ReturnMemThruPtr_dest(test_user_left_msg,
                                    sizeof(test_user_left_msg));
    os_timestamp_ExpectAndReturn(0);
    ret = create_broadcast_msg_user_left(&user, &msg);

    TEST_ASSERT_EQUAL_INT(EXIT_SUCCESS, ret);
}

void test_get_user_from_fd_fail(void) {
    int fd_known = 3;
    int fd_unknwon = 4;
    const char *name = "John Doe";
    struct user_info user1;
    struct list_head users;
    struct user_info *ret;

    user1.fd = fd_known;
    user1.remove_user = false;
    user1.name = (char *)name;
    INIT_LIST_HEAD(&users);
    list_add(&user1.list, &users);

    ret = get_user_from_fd(fd_unknwon, &users);

    TEST_ASSERT_NULL(ret);
}

void test_get_user_from_fd_pass(void) {
    int fd_known = 3;
    const char *name = "John Doe";
    struct user_info user1;
    struct list_head users;
    struct user_info *ret;

    user1.fd = fd_known;
    user1.remove_user = false;
    user1.name = (char *)name;
    INIT_LIST_HEAD(&users);
    list_add(&user1.list, &users);

    ret = get_user_from_fd(fd_known, &users);

    TEST_ASSERT_NOT_NULL(ret);
    TEST_ASSERT_EQUAL_PTR(&user1, ret);
}

void test_init_list_head_pass(void) {
    struct list_head users;

    memset(&users, 0, sizeof(struct list_head));
    init_user_list(&users);

    TEST_ASSERT_EQUAL_PTR(&users, users.next);
    TEST_ASSERT_EQUAL_PTR(&users, users.prev);
}

void test_remove_flagged_users_pass(void) {
    int fd_user1 = 3;
    int fd_user2 = 4;
    const char *name1 = "John Doe";
    const char *name2 = "Jane Doe";
    struct user_info user1;
    struct user_info user2;
    struct list_head users;

    user1.fd = fd_user1;
    user1.remove_user = true;
    user1.name = (char *)name1;

    user2.fd = fd_user2;
    user2.remove_user = false;
    user2.name = (char *)name2;

    INIT_LIST_HEAD(&users);
    list_add(&user1.list, &users);
    list_add(&user2.list, &users);

    os_free_Expect(user1.name);
    os_close_ExpectAndReturn(user1.fd, 0);
    os_free_Expect(&user1);

    remove_flagged_users(&users);

    TEST_ASSERT_EQUAL_PTR(users.prev, &user2.list);
    TEST_ASSERT_EQUAL_PTR(user2.list.next, &users);
}

void test_send_broadcast_msg_to_users_fail(void) {
    int fd = 3;
    const char *name = "John Doe";
    struct user_info user1;
    struct list_head users;
    struct msg_broadcast msg;

    msg.name = (char *)name;
    user1.fd = fd;
    user1.remove_user = false;
    INIT_LIST_HEAD(&users);
    list_add(&user1.list, &users);

    send_broadcast_msg_ExpectAndReturn(fd, &msg, EXIT_FAILURE);
    send_broadcast_msg_to_users(&msg, &users);

    TEST_ASSERT_TRUE(user1.remove_user);
}

void test_send_broadcast_msg_to_users_pass(void) {
    int fd = 3;
    const char *name = "John Doe";
    struct user_info user1;
    struct list_head users;
    struct msg_broadcast msg;

    msg.name = (char *)name;
    user1.fd = fd;
    user1.remove_user = false;
    INIT_LIST_HEAD(&users);
    list_add(&user1.list, &users);

    send_broadcast_msg_ExpectAndReturn(fd, &msg, EXIT_SUCCESS);
    send_broadcast_msg_to_users(&msg, &users);

    TEST_ASSERT_FALSE(user1.remove_user);
}

void test_send_broadcast_msg_to_users_pass_no_send(void) {
    int fd = 3;
    const char *name = "John Doe";
    struct user_info user1;
    struct list_head users;
    struct msg_broadcast msg;

    msg.name = (char *)name;
    user1.fd = fd;
    user1.remove_user = true;
    INIT_LIST_HEAD(&users);
    list_add(&user1.list, &users);

    send_broadcast_msg_Stub(send_broadcast_msg_dont_call);
    send_broadcast_msg_to_users(&msg, &users);
}

/* not needed when using generate_test_runner.rb */
int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_add_new_user_fail_malloc);
    RUN_TEST(test_add_new_user_pass);
    RUN_TEST(test_copy_user_fds_pass);
    RUN_TEST(test_count_num_users_pass_count_0);
    RUN_TEST(test_count_num_users_pass_count_2);
    RUN_TEST(test_create_broadcast_msg_user_joined_fail_malloc);
    RUN_TEST(test_create_broadcast_msg_user_joined_pass);
    RUN_TEST(test_create_broadcast_msg_user_left_fail_malloc);
    RUN_TEST(test_create_broadcast_msg_user_left_pass);
    RUN_TEST(test_get_user_from_fd_fail);
    RUN_TEST(test_get_user_from_fd_pass);
    RUN_TEST(test_init_list_head_pass);
    RUN_TEST(test_remove_flagged_users_pass);
    RUN_TEST(test_send_broadcast_msg_to_users_fail);
    RUN_TEST(test_send_broadcast_msg_to_users_pass);
    RUN_TEST(test_send_broadcast_msg_to_users_pass_no_send);
    return UNITY_END();
}
