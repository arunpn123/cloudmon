#ifndef __AGGREGATE_DOMAIN_STATS_HH__
#define __AGGREGATE_DOMAIN_STATS_HH__

//#include <msgpack.hpp>
#include <vector>

// simple struct encapsulating a set of domain statistics
// (only mem and cpu utilization % for now) at a given time
struct DomainStats
{
    // currently, time of validity is only stored at the 
    // aggregate level (e.g. not here, see AggregateDomainStats)
    // this loses precision, but simplifies "batching" a set of updates
    // with a single timestamp
    int domain_id;
    double cpu_utilization_pct;
    double mem_utilization_pct;

    //MSGPACK_DEFINE(domain_id, cpu_utilization_pct, mem_utilization_pct);
};

struct AggregateDomainStats
{
    // @todo: don't think msgpack can serialize a timespec :(
    //timespec tov;
    double tov;
    std::vector<DomainStats> domain_stats;

    //MSGPACK_DEFINE(tov, domain_stats);
};

#endif

