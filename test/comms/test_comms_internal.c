// Copyright 2020 author. All rights reserved.

#include <stdlib.h>

#include "mocks/mock_fileio.h"
#include "mocks/mock_osdep.h"
#include "mocks/mock_user.h"
#include "src/comms/comms_internal.h"
#include "unity/src/unity.h"

void setUp(void) {
    /* set stuff up here */
    mock_fileio_Init();
    mock_user_Init();
    mock_osdep_Init();
}

void tearDown(void) {
    /* clean stuff up here */
    mock_fileio_Verify();
    mock_fileio_Destroy();
    mock_user_Verify();
    mock_user_Destroy();
    mock_osdep_Verify();
    mock_osdep_Destroy();
}

void test_process_fds_fail_accept(void) {
    int listenfd = 2;
    int max_fd = 2;
    fd_set rfds;
    int ret;

    os_fd_isset_ExpectAndReturn(0, &rfds, 0);
    os_fd_isset_ExpectAndReturn(1, &rfds, 0);
    os_fd_isset_ExpectAndReturn(listenfd, &rfds, 1);
    os_accept_ExpectAndReturn(listenfd, NULL, NULL, -1);
    ret = process_fds(listenfd, max_fd, &rfds);

    TEST_ASSERT_EQUAL_INT(EXIT_FAILURE, ret);
}

void test_process_fds_fail_handle_new_user(void) {
    int listenfd = 2;
    int userfd = 3;
    int max_fd = 2;
    fd_set rfds;
    int ret;

    os_fd_isset_ExpectAndReturn(0, &rfds, 0);
    os_fd_isset_ExpectAndReturn(1, &rfds, 0);
    os_fd_isset_ExpectAndReturn(listenfd, &rfds, 1);
    os_accept_ExpectAndReturn(listenfd, NULL, NULL, userfd);
    set_fd_nonblocking_ExpectAndReturn(userfd, EXIT_SUCCESS);
    handle_new_user_ExpectAndReturn(userfd, EXIT_FAILURE);
    os_close_ExpectAndReturn(userfd, 0);
    ret = process_fds(listenfd, max_fd, &rfds);

    TEST_ASSERT_EQUAL_INT(EXIT_SUCCESS, ret);
}

void test_process_fds_fail_handle_registered_user(void) {
    int listenfd = 2;
    int userfd = 3;
    int max_fd = 3;
    fd_set rfds;
    int ret;

    os_fd_isset_ExpectAndReturn(0, &rfds, 0);
    os_fd_isset_ExpectAndReturn(1, &rfds, 0);
    os_fd_isset_ExpectAndReturn(listenfd, &rfds, 0);
    os_fd_isset_ExpectAndReturn(userfd, &rfds, 1);
    handle_registered_user_ExpectAndReturn(userfd, EXIT_FAILURE);
    ret = process_fds(listenfd, max_fd, &rfds);

    TEST_ASSERT_EQUAL_INT(EXIT_SUCCESS, ret);
}

void test_process_fds_fail_set_fd_nonblocking(void) {
    int listenfd = 2;
    int userfd = 3;
    int max_fd = 2;
    fd_set rfds;
    int ret;

    os_fd_isset_ExpectAndReturn(0, &rfds, 0);
    os_fd_isset_ExpectAndReturn(1, &rfds, 0);
    os_fd_isset_ExpectAndReturn(listenfd, &rfds, 1);
    os_accept_ExpectAndReturn(listenfd, NULL, NULL, userfd);
    set_fd_nonblocking_ExpectAndReturn(userfd, EXIT_FAILURE);
    os_close_ExpectAndReturn(userfd, 0);
    ret = process_fds(listenfd, max_fd, &rfds);

    TEST_ASSERT_EQUAL_INT(EXIT_FAILURE, ret);
}

void test_process_fds_pass_handle_new_user(void) {
    int listenfd = 2;
    int userfd = 3;
    int max_fd = 2;
    fd_set rfds;
    int ret;

    os_fd_isset_ExpectAndReturn(0, &rfds, 0);
    os_fd_isset_ExpectAndReturn(1, &rfds, 0);
    os_fd_isset_ExpectAndReturn(listenfd, &rfds, 1);
    os_accept_ExpectAndReturn(listenfd, NULL, NULL, userfd);
    set_fd_nonblocking_ExpectAndReturn(userfd, EXIT_SUCCESS);
    handle_new_user_ExpectAndReturn(userfd, EXIT_SUCCESS);
    ret = process_fds(listenfd, max_fd, &rfds);

    TEST_ASSERT_EQUAL_INT(EXIT_SUCCESS, ret);
}

void test_process_fds_pass_handle_registered_user(void) {
    int listenfd = 2;
    int userfd = 3;
    int max_fd = 3;
    fd_set rfds;
    int ret;

    os_fd_isset_ExpectAndReturn(0, &rfds, 0);
    os_fd_isset_ExpectAndReturn(1, &rfds, 0);
    os_fd_isset_ExpectAndReturn(listenfd, &rfds, 0);
    os_fd_isset_ExpectAndReturn(userfd, &rfds, 1);
    handle_registered_user_ExpectAndReturn(userfd, EXIT_SUCCESS);
    ret = process_fds(listenfd, max_fd, &rfds);

    TEST_ASSERT_EQUAL_INT(EXIT_SUCCESS, ret);
}

void test_set_fds_fail_malloc(void) {
    int num_users = 2;
    fd_set rfds;
    int listenfd = 2;
    int ret;

    os_fd_zero_Expect(&rfds);
    os_fd_set_Expect(listenfd, &rfds);
    get_num_users_ExpectAndReturn(num_users);
    os_malloc_ExpectAndReturn(num_users*sizeof(int), NULL);
    ret = set_fds(listenfd, &rfds);

    TEST_ASSERT_EQUAL_INT(listenfd, ret);
}

void test_set_fds_add_users(void) {
    int num_users = 2;
    int user_fds[2] = {3, 4};
    fd_set rfds;
    int listenfd = 2;
    int ret;

    os_fd_zero_Expect(&rfds);
    os_fd_set_Expect(listenfd, &rfds);
    get_num_users_ExpectAndReturn(num_users);
    os_malloc_ExpectAndReturn(num_users*sizeof(int), user_fds);
    get_user_fds_Expect(user_fds);
    os_fd_set_Expect(user_fds[0], &rfds);
    os_fd_set_Expect(user_fds[1], &rfds);
    os_free_Expect(user_fds);
    ret = set_fds(listenfd, &rfds);

    TEST_ASSERT_EQUAL_INT(user_fds[num_users-1], ret);
}

void test_set_fds_no_users(void) {
    fd_set rfds;
    int listenfd = 2;
    int ret;

    os_fd_zero_Expect(&rfds);
    os_fd_set_Expect(listenfd, &rfds);
    get_num_users_ExpectAndReturn(0);
    ret = set_fds(listenfd, &rfds);

    TEST_ASSERT_EQUAL_INT(listenfd, ret);
}

/* not needed when using generate_test_runner.rb */
int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_process_fds_fail_accept);
    RUN_TEST(test_process_fds_fail_handle_new_user);
    RUN_TEST(test_process_fds_fail_handle_registered_user);
    RUN_TEST(test_process_fds_fail_set_fd_nonblocking);
    RUN_TEST(test_process_fds_pass_handle_new_user);
    RUN_TEST(test_process_fds_pass_handle_registered_user);
    RUN_TEST(test_set_fds_fail_malloc);
    RUN_TEST(test_set_fds_add_users);
    RUN_TEST(test_set_fds_no_users);
    return UNITY_END();
}
