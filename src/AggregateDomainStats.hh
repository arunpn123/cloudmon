#ifndef __AGGREGATE_DOMAIN_STATS_HH__
#define __AGGREGATE_DOMAIN_STATS_HH__

#include <msgpack.hpp>
#include <vector>

// simple struct encapsulating a set of domain statistics
// (only mem and cpu utilization % for now) at a given time
struct DomainStats
{
    DomainStats() :
        domain_id(-1),
        cpu_utilization_pct(0),
        mem_utilization_pct(0),
        disk_read_req(0),
        disk_write_req(0),
        disk_read_size(0),
        disk_write_size(0),
        disk_errors(0),
        rx_bytes(0),
        rx_packets(0),
        rx_errs(0),
        rx_drop(0),
        tx_bytes(0),
        tx_packets(0),
        tx_errs(0),
        tx_drop(0)
    { }

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
    long long rx_bytes;
    long long rx_packets;
    long long rx_errs;
    long long rx_drop;
    long long tx_bytes;
    long long tx_packets;
    long long tx_errs;
    long long tx_drop;

    MSGPACK_DEFINE(
        domain_id, 
        domain_uuid,
        cpu_utilization_pct, 
        mem_utilization_pct, 
        disk_read_req, 
        disk_write_req, 
        disk_read_size, 
        disk_write_size, 
        disk_errors,
        rx_bytes,
        rx_packets,
        rx_errs,
        rx_drop,
        tx_bytes,
        tx_packets,
        tx_errs,
        tx_drop);
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

