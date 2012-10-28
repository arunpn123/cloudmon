#include "AggregateDomainStats.hh"
#include "my_zhelpers.h"
#include "Message.hh"
#include "common.hh"

#include <zmq.h>
#include <msgpack.hpp>

#include <signal.h>
#include <assert.h>
#include <iostream>
#include <iomanip>

bool g_done;

void handle_interrupt(int signum)
{
    std::cout << "terminating on signal " << signum << std::endl;
    g_done = true;
}

int main()
{
    g_done = false;

    signal(SIGINT, handle_interrupt);

    void *context = zmq_ctx_new();
    void *subscriber = zmq_socket(context, ZMQ_SUB);
    int rc = zmq_bind(subscriber, "tcp://*:12345");
    assert(rc == 0);

    //const char *filter = "domain_stats";
    const char *filter = "monitor_data";
    rc = zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, filter, strlen(filter));
    assert(rc == 0);

    while(!g_done)
    {
        Message next;
        next.receive(subscriber);

        msgpack::unpacked msg;
        msgpack::unpack(&msg, next.data, next.data_len);

        AggregateDomainStats stats;
        msg.get().convert(&stats);

        //print_aggregate_stats(stats);
        print_carbon_update_lines(stats);
    }

    zmq_close(subscriber);
    zmq_ctx_destroy(context);

    return 0;
}

