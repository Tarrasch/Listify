TARGET=listify
LDLIBS += -lreadline
CFLAGs += -Werror

include common.mk

$(TARGET): listify.o listify_posix.o appkey.o cmd.o list.o link.o
