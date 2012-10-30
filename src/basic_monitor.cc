#include "DomainStatTracker.hh"
#include "AggregateDomainStats.hh"
#include "ActiveDomainWatcher.hh"
#include "DomainStatPublisher.hh"
#include "common.hh"

#include <zmq.h>
#include "Message.hh"

#include <msgpack.hpp>
#include <libvirt/libvirt.h>
#include <unistd.h> // for sleep
#include <signal.h>

#include <cassert>
#include <stdexcept>
#include <iostream>
#include <vector>
#include <iomanip>

bool g_done;

void handle_interrupt(int signum)
{
    std::cout << "terminating on signal " << signum << std::endl;
    g_done = true;
}

int main(int argc, char **argv)
{
    if(argc < 2)
    {
        std::cout << "usage: " << argv[0] << " [receiver_endpoint]\n";
        exit(1);
    }

    g_done = false;
    unsigned int update_interval = 1;

    signal(SIGINT, handle_interrupt);

    // initialize libvirt and connect to xen
    virConnectPtr conn;

    // setup hypervisor URI
    // use "test:///default" to use a "mock" hypervisor included with libvirt
    // otherwise, use "xen:///" to connect to Xen
    //std::string hypervisor_uri = "test:///default";
    std::string hypervisor_uri = "xen:///";

    conn = virConnectOpen(hypervisor_uri.c_str());
    if(conn == NULL)
    {
        throw std::runtime_error(
            "failed to open libvirt connection to '" +
            hypervisor_uri +
            "'");
    }
    
    // initialize zmq
    void *context = zmq_ctx_new();

    typedef std::vector<DomainStatPublisher *> PubListType;
    PubListType pubs;

    pubs.push_back(new LibvirtDomainStatPublisher(
        context,
        argv[1],
        conn));

    unsigned int num_fake_domains = 0;
    for(int i = 0; i < num_fake_domains; i++)
    {
        pubs.push_back(new LinuxHostStatPublisher(
            context,
            argv[1],
            10000 + i));
    }

    while(!g_done)
    {
        for(PubListType::const_iterator it = pubs.begin();
            it != pubs.end();
            ++it)
        {
            (*it)->update();
        }
        
        sleep(update_interval);
    }

    std::cout << "shutting down...\n";

    for(PubListType::const_iterator it = pubs.begin();
        it != pubs.end();
        ++it)
    {
        delete *it;
    }

    zmq_ctx_destroy(context);

    virConnectClose(conn);

    return 0;
}

