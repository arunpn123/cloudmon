#include "common.hh"

#include <iostream>
#include <iomanip>

void print_aggregate_stats(const AggregateDomainStats & agg)
{
    std::cout << "time: " << std::setprecision(12) << agg.tov << std::endl;
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

void print_carbon_update_lines(const AggregateDomainStats & agg)
{
    for(int i = 0; i < agg.domain_stats.size(); i++)
    {
        const DomainStats & dom = agg.domain_stats[i];

        char stat_key[512];
        char line[1024];
        
        snprintf(
            stat_key,
            512,
            "servers.%s.dom%d.cpu",
            agg.hostname.c_str(),
            dom.domain_id);
        snprintf(
            line,
            1024,
            "%s %f %f",
            stat_key, dom.cpu_utilization_pct, agg.tov);

        std::cout << line << std::endl;

        snprintf(
            stat_key,
            512,
            "servers.%s.dom%d.mem",
            agg.hostname.c_str(),
            dom.domain_id);
        snprintf(
            line,
            1024,
            "%s %f %f",
            stat_key, dom.mem_utilization_pct, agg.tov);

        std::cout << line << std::endl;
    }
}

