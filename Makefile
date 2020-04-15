CC := clang

CCFLAGS := \
	-std=c99                        \
	-g                              \
	-D_DEFAULT_SOURCE               \
	-Wall                           \
	-Wextra                         \
	-Wpointer-arith                 \
	-Wcast-align                    \
	-Wwrite-strings                 \
	-Wswitch-default                \
	-Wunreachable-code              \
	-Winit-self                     \
	-Wmissing-field-initializers    \
	-Wno-unknown-pragmas            \
	-Wstrict-prototypes             \
	-Wundef                         \
	-Wold-style-definition

TARGET := server

INC := -I.

SRC_DIR := \
	lib/fileio/*.o  \
	lib/osdep/*.o   \
	src/comms/*.o   \
	src/message/*.o \
	src/user/*.o    \
	src/*.o

DEP := \
	include/common.h                \
	include/list.h                  \
	lib/fileio/fileio.h             \
	lib/osdep/osdep.h               \
	src/comms/comms.h               \
	src/comms/comms_internal.h      \
	src/message/message.h           \
	src/message/message_internal.h  \
	src/user/user.h                 \
	src/user/user_internal.h

OBJ := \
	lib/fileio/fileio.o             \
	lib/osdep/osdep.o               \
	src/comms/comms.o               \
	src/comms/comms_internal.o      \
	src/message/message.o           \
	src/message/message_internal.o  \
	src/user/user.o                 \
	src/user/user_internal.o        \
	src/main.o

TST_TARGET := \
	bin/test_comms                  \
	bin/test_comms_internal         \
	bin/test_message                \
	bin/test_message_internal       \
	bin/test_user                   \
	bin/test_user_internal

TST_INC := \
	-Iunity/src     \
	-Icmock/src     \
	-Ilib/fileio    \
	-Ilib/osdep     \
	-Isrc/comms     \
	-Isrc/message   \
	-Isrc/user

MOCK := \
	lib/fileio/fileio.h             \
	lib/osdep/osdep.h               \
	src/comms/comms_internal.h      \
	src/message/message.h           \
	src/message/message_internal.h  \
	src/user/user.h                 \
	src/user/user_internal.h

TST_DIR := \
	unity/src/*.o       \
	cmock/src/*.o       \
	mocks/*.o           \
	test/comms/*.o      \
	test/message/*.o    \
	test/user/*.o

TST_DEP := \
	mocks/mock_fileio.h              \
	mocks/mock_osdep.h               \
	mocks/mock_comms_internal.h      \
	mocks/mock_message.h             \
	mocks/mock_message_internal.h    \
	mocks/mock_user.h                \
	mocks/mock_user_internal.h

TST_OBJ := \
	unity/src/unity.o               \
	cmock/src/cmock.o               \
	mocks/mock_fileio.o             \
	mocks/mock_osdep.o              \
	mocks/mock_comms_internal.o     \
	mocks/mock_message.o            \
	mocks/mock_message_internal.o   \
	mocks/mock_user.o               \
	mocks/mock_user_internal.o

TST_RUNNER := \
	test/comms/test_comms.o                 \
	test/comms/test_comms_internal.o        \
	test/message/test_message.o             \
	test/message/test_message_internal.o    \
	test/user/test_user.o                   \
	test/user/test_user_internal.o

.PHONY : all clean check lint test

all : $(TARGET)

clean :
	rm -f $(SRC_DIR) $(TARGET) $(TST_DIR) $(TST_TARGET)

setup :
	ruby cmock/lib/cmock.rb -ocmock-config.yml $(MOCK)

check :
	@echo "\n######################## Running cppcheck ########################\n"
	cppcheck --quiet --std=c99 -D_DEFAULT_SOURCE \
		--enable=style,warning,performance,portability,unusedFunction \
		-i cmock -i unity -i mocks -i test .

lint :
	@echo "\n######################## Running cpplint #########################\n"
	python3 -m cpplint --filter=-readability/casting,-runtime/casting --root=. --recursive \
		include lib src

test : setup $(TST_TARGET)

$(OBJ) : %.o : %.c $(DEP)
	$(CC)  $(INC) -c -o $@ $< $(CCFLAGS)

$(TARGET) : % : $(OBJ)
	$(CC) -o $@ $^ $(CCFLAGS)

$(TST_OBJ) $(TST_RUNNER) : %.o : %.c $(DEP) $(TST_DEP)
	$(CC) $(INC) $(TST_INC) -c -o $@ $< $(CCFLAGS)

bin/test_comms : % : $(filter-out mocks/mock_comms.o, $(TST_OBJ)) \
	src/comms/comms.o test/comms/test_comms.o
	$(CC) -o $@ $^ $(CCFLAGS)

bin/test_comms_internal : % : $(filter-out mocks/mock_comms_internal.o, $(TST_OBJ)) \
	src/comms/comms_internal.o test/comms/test_comms_internal.o
	$(CC) -o $@ $^ $(CCFLAGS)

bin/test_message : % : $(filter-out mocks/mock_message.o, $(TST_OBJ)) \
	src/message/message.o test/message/test_message.o
	$(CC) -o $@ $^ $(CCFLAGS)

bin/test_message_internal : % : $(filter-out mocks/mock_message_internal.o, $(TST_OBJ)) \
	src/message/message_internal.o test/message/test_message_internal.o
	$(CC) -o $@ $^ $(CCFLAGS)

bin/test_user : % : $(filter-out mocks/mock_user.o, $(TST_OBJ)) \
	src/user/user.o test/user/test_user.o
	$(CC) -o $@ $^ $(CCFLAGS)

bin/test_user_internal : % : $(filter-out mocks/mock_user_internal.o, $(TST_OBJ)) \
	src/user/user_internal.o test/user/test_user_internal.o
	$(CC) -o $@ $^ $(CCFLAGS)
