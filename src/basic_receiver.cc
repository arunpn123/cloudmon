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
#include <vector>

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

    void * context = zmq_ctx_new();
    void * subscriber = zmq_socket(context, ZMQ_SUB);
    int rc = zmq_bind(subscriber, "tcp://*:12345");
    assert(rc == 0);

    // setup a relay to republish every message received
    // this allows other clients to connect to a stable component
    // to receive monitored data
    void * publisher = zmq_socket(context, ZMQ_PUB);
    zmq_bind(publisher, "tcp://*:12346");
    // allow other threads within this process to see the data as well
    zmq_bind(publisher, "inproc://monitor");

    std::vector<std::string> filters;
    // receive data from all physical nodes
    filters.push_back("monitor.servers.");
    // watch for instance termination messages
    filters.push_back("monitor.term_instance");
    
    for(int i = 0; i < filters.size(); i++)
    {
        rc = zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, filters[i].c_str(), filters[i].length());
        assert(rc == 0);
    }

    while(!g_done)
    {
        try
        {
            Message next;
            next.receive(subscriber);
            
            // republish this message via our relay socket
            next.send(publisher);

            if(next.key == "monitor.term_instance")
            {
                msgpack::unpacked msg;
                msgpack::unpack(&msg, next.data, next.data_len);

                std::string which_instance;
                msg.get().convert(&which_instance);

                std::cout << "instance terminated: " << which_instance << "\n";
            }
            else
            {
                msgpack::unpacked msg;
                msgpack::unpack(&msg, next.data, next.data_len);

                AggregateDomainStats stats;
                msg.get().convert(&stats);

                //print_aggregate_stats(stats);
                print_carbon_update_lines(stats);
            }
        }
        catch(std::exception & e)
        {
            std::cout << "error: " << e.what() << "\n";
            g_done = true;
        }
    }

    std::cout << "shutting down\n";

    //TODO: debugging only...
    zmq_close(publisher);

    zmq_close(subscriber);
    zmq_ctx_destroy(context);

    return 0;
}

