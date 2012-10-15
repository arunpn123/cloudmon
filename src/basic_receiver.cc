#include "my_zhelpers.h"
#include <zmq.h>
#include <msgpack.hpp>

#include <iostream>
#include <signal.h>
#include <assert.h>

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
    const char *filter = "";
    rc = zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, filter, strlen(filter));
    assert(rc == 0);

    while(!g_done)
    {
        char *received_msg = s_recv(subscriber);
        std::cout << "received " << strlen(received_msg) << " bytes\n";
        free(received_msg);
    }

    zmq_close(subscriber);
    zmq_ctx_destroy(context);

    return 0;
}
