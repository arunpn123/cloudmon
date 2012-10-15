
all: basic_monitor eat_memory

basic_monitor: DomainStatTracker.cc DomainStatTracker.hh timespec_utils.cc timespec_utils.hh basic_monitor.cc
	g++ -o basic_monitor DomainStatTracker.cc timespec_utils.cc basic_monitor.cc -lvirt -lrt

eat_memory: eat_memory.c
	gcc -static -o eat_memory eat_memory.c

clean:
	rm *.o basic_monitor eat_memory
