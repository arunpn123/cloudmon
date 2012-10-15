#include "DomainStatTracker.hh"
#include "AggregateDomainStats.hh"

#include <libvirt/libvirt.h>
#include <unistd.h> // for sleep
#include <signal.h>

#include <cassert>
#include <stdexcept>
#include <iostream>
#include <vector>

bool g_done;

void handle_interrupt(int signum)
{
    std::cout << "terminating on signal " << signum << std::endl;
    g_done = true;
}

/**
 * return a vector of the numeric IDs of currently active domains
 * @todo: using shared_array would be more efficient
 */
std::vector<int> get_active_virt_domains(virConnectPtr conn)
{
    assert(conn);

    int num_domains = virConnectNumOfDomains(conn);

    // should at least have a domain 0
    assert(num_domains > 0);

    std::vector<int> out(num_domains);
    int *active_domains = new int[num_domains];
    
    num_domains = virConnectListDomains(conn, active_domains, num_domains);

    for(int i = 0; i < num_domains; i++)
        out[i] = active_domains[i];

    delete [] active_domains;

    return out;
}

void print_aggregate_stats(const AggregateDomainStats & agg)
{
    std::cout << "time: " << agg.tov << std::endl;
    std::cout << agg.domain_stats.size() << " domains:\n";

    for(int i = 0; i < agg.domain_stats.size(); i++)
    {
        const DomainStats & this_dom = agg.domain_stats[i];
        std::cout << "  " << this_dom.domain_id << ": "
                  << "cpu: " << this_dom.cpu_utilization_pct*100 << "% "
                  << "mem: " << this_dom.mem_utilization_pct*100 << "%"
                  << std::endl;
    }
}

int main(int argc, char **argv)
{
    g_done = false;
    
    signal(SIGINT, handle_interrupt);

    virConnectPtr conn;

    conn = virConnectOpen("xen:///");
    if(conn == NULL)
        throw std::runtime_error("failed to open libvirt connection to xen:///");

    std::vector<int> active_domains = get_active_virt_domains(conn);

    std::cout << "active domains:\n";
    for(int i = 0; i < active_domains.size(); i++)
        std::cout << "  domain " << active_domains[i] << std::endl;

    std::vector<DomainStatTracker *> trackers;

    for(int i = 0; i < active_domains.size(); i++)
    {
        DomainStatTracker *tracker = new DomainStatTracker(conn, active_domains[i]);
        trackers.push_back(tracker);
    }

    AggregateDomainStats agg_stats;
    agg_stats.domain_stats.resize(trackers.size());
    
    while(!g_done)
    {
        for(int i = 0; i < trackers.size(); i++)
        {
            agg_stats.domain_stats[i] = trackers[i]->update();
            //std::cout << agg_stats.domain_stats[i].cpu_utilization_pct << std::endl << std::endl;
        }

        agg_stats.tov = timespec_utils::to_double(timespec_utils::realtime_now());

        // @todo: testing msgpack as a serializatio mechanism if EVPath doesn't work out...
        // disabled for now...
        //msgpack::sbuffer sbuf;
        //msgpack::pack(sbuf, agg_stats);
        //std::cout << "serialized " << sbuf.size() << " bytes\n";
        
        print_aggregate_stats(agg_stats);

        sleep(2);
    }

    std::cout << "shutting down...\n";

    for(int i = 0; i < trackers.size(); i++)
    {
        delete trackers[i];
    }

    virConnectClose(conn);

    return 0;
}

