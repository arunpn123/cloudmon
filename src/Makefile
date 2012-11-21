ZMQ_PATH = ../external/zeromq-install
CZMQ_PATH = ../external/czmq-install
MSGPACK_PATH = ../external/msgpack-install
MXML_PATH= ../external/mxml-install

DEBUG =
#DEBUG = -ggdb -O0

CFLAGS = $(DEBUG)

CPPFLAGS = \
 -I$(ZMQ_PATH)/include \
 -I$(CZMQ_PATH)/include \
 -I$(MSGPACK_PATH)/include\
 -I$(MXML_PATH)/include

LDFLAGS = \
 -L$(ZMQ_PATH)/lib \
 -L$(CZMQ_PATH)/lib \
 -L$(MSGPACK_PATH)/lib\
 -L$(MXML_PATH)/lib

COMMON_LIBS = -lrt -lzmq -lmsgpack -lmxml
MONITOR_LIBS = $(COMMON_LIBS) -lvirt
RECEIVER_LIBS = $(COMMON_LIBS)

all: basic_monitor basic_receiver eat_memory
.PHONY: all

%.o: %.cc
	g++ $(CPPFLAGS) $(CFLAGS) -c $< -o $@

basic_monitor: common.o DomainStatTracker.o timespec_utils.o basic_monitor.o
	g++ $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o $@ $? $(MONITOR_LIBS)

basic_receiver: common.o basic_receiver.o
	g++ $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o $@ $? $(RECEIVER_LIBS)

eat_memory: eat_memory.c
	gcc -static -o eat_memory eat_memory.c

.PHONY: clean
clean:
	rm *.o basic_monitor basic_receiver eat_memory
