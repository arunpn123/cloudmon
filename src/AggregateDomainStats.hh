#ifndef __AGGREGATE_DOMAIN_STATS_HH__
#define __AGGREGATE_DOMAIN_STATS_HH__

#include <msgpack.hpp>
#include <vector>

// simple struct encapsulating a set of domain statistics
// (only mem and cpu utilization % for now) at a given time
struct DomainStats
{
    // currently, time of validity is only stored at the 
    // aggregate level (e.g. not here, see AggregateDomainStats)
    // this loses precision, but simplifies "batching" a set of updates
    // with a single timestamp
    
    // this is the hypervisor-assigned domain ID
    int domain_id;
    // this is the hypervisor-assigned UUID for the domain
    std::string domain_uuid;

    double cpu_utilization_pct;
    double mem_utilization_pct;
    long long disk_read_req;
    long long disk_write_req;
    long long disk_read_size; //in Bytes
    long long disk_write_size; //in Bytes
    long long disk_errors;

    MSGPACK_DEFINE(
        domain_id, 
        domain_uuid,
        cpu_utilization_pct, 
        mem_utilization_pct, 
        disk_read_req, 
        disk_write_req, 
        disk_read_size, 
        disk_write_size, 
        disk_errors);
};

struct AggregateDomainStats
{
    // @todo: don't think msgpack can serialize a timespec :(
    //timespec tov;

    // time of this update (seconds since UNIX epoch)
    double tov;
    // hostname of the bare-metal host machine
    std::string hostname;
    // virtual domain statistics for this host
    std::vector<DomainStats> domain_stats;

    MSGPACK_DEFINE(tov, hostname, domain_stats);
};

#endif

