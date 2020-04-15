// Copyright 2020 author. All rights reserved.

#include <errno.h>
#include <netinet/in.h>
#include <stdlib.h>

#include "mocks/mock_comms_internal.h"
#include "mocks/mock_osdep.h"
#include "mocks/mock_user.h"
#include "src/comms/comms.h"
#include "unity/src/unity.h"

void setUp(void) {
    /* set stuff up here */
    mock_comms_internal_Init();
    mock_osdep_Init();
    mock_user_Init();
}

void tearDown(void) {
    /* clean stuff up here */
    mock_comms_internal_Verify();
    mock_comms_internal_Destroy();
    mock_osdep_Verify();
    mock_osdep_Destroy();
    mock_user_Verify();
    mock_user_Destroy();
}

void test_init_listen_sock_fail_bind(void) {
    struct sockaddr_in servaddr;
    int fd = 2;
    int ret;

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htobe32(INADDR_LOOPBACK);
    servaddr.sin_port = htobe16(1234);

    os_socket_ExpectAndReturn(AF_INET, SOCK_STREAM, 0, fd);
    os_setsockopt_ExpectAndReturn(fd,
                                  SOL_SOCKET,
                                  SO_REUSEADDR,
                                  NULL,
                                  sizeof(int),
                                  0);
    os_setsockopt_IgnoreArg_optval();
    os_memset_ExpectAndReturn(NULL, 0, sizeof(struct sockaddr_in), &servaddr);
    os_memset_ReturnMemThruPtr_s(&servaddr, sizeof(servaddr));
    os_memset_IgnoreArg_s();
    os_bind_ExpectAndReturn(fd,
                            (struct sockaddr *)&servaddr,
                            sizeof(servaddr),
                            -1);
    ret = init_listen_sock();

    TEST_ASSERT_EQUAL_INT(-1, ret);
}

void test_init_listen_sock_fail_listen(void) {
    struct sockaddr_in servaddr;
    int fd = 2;
    int ret;

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htobe32(INADDR_LOOPBACK);
    servaddr.sin_port = htobe16(1234);

    os_socket_ExpectAndReturn(AF_INET, SOCK_STREAM, 0, fd);
    os_setsockopt_ExpectAndReturn(fd,
                                  SOL_SOCKET,
                                  SO_REUSEADDR,
                                  NULL,
                                  sizeof(int),
                                  0);
    os_setsockopt_IgnoreArg_optval();
    os_memset_ExpectAndReturn(NULL, 0, sizeof(struct sockaddr_in), &servaddr);
    os_memset_ReturnMemThruPtr_s(&servaddr, sizeof(servaddr));
    os_memset_IgnoreArg_s();
    os_bind_ExpectAndReturn(fd,
                            (struct sockaddr *)&servaddr,
                            sizeof(servaddr),
                            0);
    os_listen_ExpectAndReturn(fd, 5, -1);
    ret = init_listen_sock();

    TEST_ASSERT_EQUAL_INT(-1, ret);
}

void test_init_listen_sock_fail_setsockopt(void) {
    int fd = 2;
    int ret;

    os_socket_ExpectAndReturn(AF_INET, SOCK_STREAM, 0, fd);
    os_setsockopt_ExpectAndReturn(fd,
                                  SOL_SOCKET,
                                  SO_REUSEADDR,
                                  NULL,
                                  sizeof(int),
                                  -1);
    os_setsockopt_IgnoreArg_optval();
    ret = init_listen_sock();

    TEST_ASSERT_EQUAL_INT(-1, ret);
}

void test_init_listen_sock_fail_socket(void) {
    int ret;

    os_socket_ExpectAndReturn(AF_INET, SOCK_STREAM, 0, -1);
    ret = init_listen_sock();

    TEST_ASSERT_EQUAL_INT(-1, ret);
}

void test_init_listen_sock_pass(void) {
    struct sockaddr_in servaddr;
    int fd = 2;
    int ret;

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htobe32(INADDR_LOOPBACK);
    servaddr.sin_port = htobe16(1234);

    os_socket_ExpectAndReturn(AF_INET, SOCK_STREAM, 0, fd);
    os_setsockopt_ExpectAndReturn(fd,
                                  SOL_SOCKET,
                                  SO_REUSEADDR,
                                  NULL,
                                  sizeof(int),
                                  0);
    os_setsockopt_IgnoreArg_optval();
    os_memset_ExpectAndReturn(NULL, 0, sizeof(struct sockaddr_in), &servaddr);
    os_memset_ReturnMemThruPtr_s(&servaddr, sizeof(servaddr));
    os_memset_IgnoreArg_s();
    os_bind_ExpectAndReturn(fd,
                            (struct sockaddr *)&servaddr,
                            sizeof(servaddr),
                            0);
    os_listen_ExpectAndReturn(fd, 5, 0);
    ret = init_listen_sock();

    TEST_ASSERT_EQUAL_INT(fd, ret);
}

void test_monitor_socks_fail_process_fds(void) {
    int listenfd = 2;
    int max_fd = 2;
    int ret;

    set_fds_ExpectAndReturn(listenfd, NULL, max_fd);
    set_fds_IgnoreArg_rfds();
    os_select_ExpectAndReturn(max_fd+1, NULL, NULL, NULL, NULL, 1);
    os_select_IgnoreArg_readfds();
    process_fds_ExpectAndReturn(listenfd, max_fd, NULL, EXIT_FAILURE);
    process_fds_IgnoreArg_rfds();
    ret = monitor_socks(listenfd);

    TEST_ASSERT_EQUAL_INT(EXIT_FAILURE, ret);
}

void test_monitor_socks_fail_select_ebadf(void) {
    int listenfd = 2;
    int max_fd = 2;
    int ret;

    set_fds_ExpectAndReturn(listenfd, NULL, max_fd);
    set_fds_IgnoreArg_rfds();
    os_select_ExpectAndReturn(max_fd+1, NULL, NULL, NULL, NULL, -1);
    os_select_IgnoreArg_readfds();
    os_get_error_ExpectAndReturn(EBADF);
    ret = monitor_socks(listenfd);

    TEST_ASSERT_EQUAL_INT(EXIT_FAILURE, ret);
}

void test_monitor_socks_fail_select_eintr(void) {
    int listenfd = 2;
    int max_fd = 2;
    int ret;

    set_fds_ExpectAndReturn(listenfd, NULL, max_fd);
    set_fds_IgnoreArg_rfds();
    os_select_ExpectAndReturn(max_fd+1, NULL, NULL, NULL, NULL, -1);
    os_select_IgnoreArg_readfds();
    os_get_error_ExpectAndReturn(EINTR);
    ret = monitor_socks(listenfd);

    TEST_ASSERT_EQUAL_INT(EXIT_SUCCESS, ret);
}

void test_monitor_socks_pass(void) {
    int listenfd = 2;
    int max_fd = 2;
    int ret;

    set_fds_ExpectAndReturn(listenfd, NULL, max_fd);
    set_fds_IgnoreArg_rfds();
    os_select_ExpectAndReturn(max_fd+1, NULL, NULL, NULL, NULL, 1);
    os_select_IgnoreArg_readfds();
    process_fds_ExpectAndReturn(listenfd, max_fd, NULL, EXIT_SUCCESS);
    process_fds_IgnoreArg_rfds();
    update_user_list_Expect();
    ret = monitor_socks(listenfd);

    TEST_ASSERT_EQUAL_INT(EXIT_SUCCESS, ret);
}

/* not needed when using generate_test_runner.rb */
int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_init_listen_sock_fail_bind);
    RUN_TEST(test_init_listen_sock_fail_listen);
    RUN_TEST(test_init_listen_sock_fail_setsockopt);
    RUN_TEST(test_init_listen_sock_fail_socket);
    RUN_TEST(test_init_listen_sock_pass);
    RUN_TEST(test_monitor_socks_fail_process_fds);
    RUN_TEST(test_monitor_socks_fail_select_ebadf);
    RUN_TEST(test_monitor_socks_fail_select_eintr);
    RUN_TEST(test_monitor_socks_pass);
    return UNITY_END();
}
