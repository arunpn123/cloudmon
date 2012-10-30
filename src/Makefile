ZMQ_PATH = ../external/zeromq-install
CZMQ_PATH = ../external/czmq-install
MSGPACK_PATH = ../external/msgpack-install

CPPFLAGS = \
 -I$(ZMQ_PATH)/include \
 -I$(CZMQ_PATH)/include \
 -I$(MSGPACK_PATH)/include

LDFLAGS = \
 -L$(ZMQ_PATH)/lib \
 -L$(CZMQ_PATH)/lib \
 -L$(MSGPACK_PATH)/lib

LIBS = -lvirt -lrt -lzmq -lmsgpack

all: basic_monitor basic_receiver eat_memory

%.o: %.cc
	g++ $(CPPFLAGS) $(CFLAGS) -c $< -o $@

basic_monitor: common.o DomainStatTracker.o timespec_utils.o basic_monitor.o
	g++ $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o $@ $? $(LIBS)

basic_receiver: common.o basic_receiver.o
	g++ $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o $@ $? $(LIBS)

#basic_monitor: DomainStatTracker.cc DomainStatTracker.hh timespec_utils.cc timespec_utils.hh basic_monitor.cc
#	g++ $(CPPFLAGS) $(LDFLAGS) -o basic_monitor DomainStatTracker.cc timespec_utils.cc basic_monitor.cc $(LIBS)
#
#basic_receiver: basic_receiver.cc
#	g++ $(CPPFLAGS) $(LDFLAGS) -o basic_receiver basic_receiver.cc $(LIBS)

eat_memory: eat_memory.c
	gcc -static -o eat_memory eat_memory.c

clean:
	rm *.o basic_monitor basic_receiver eat_memory
