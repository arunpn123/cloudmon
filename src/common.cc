#include "common.hh"

#include <sys/stat.h>

#include <list>
#include <string>
#include <iostream>
#include <iomanip>
#include <stdexcept>

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

std::list<std::string> get_whisper_updates(const AggregateDomainStats & agg)
{
    std::list<std::string> out;

    for(int i = 0; i < agg.domain_stats.size(); ++i)
    {
        const DomainStats & dom = agg.domain_stats[i];

        char stat_key[512];
        char line[1024];
        
        snprintf(
            stat_key,
            512,
            "monitor.servers.%s.%s.cpu",
            agg.hostname.c_str(),
            dom.domain_uuid.c_str());
        snprintf(
            line,
            1024,
            "%s %f %f",
            stat_key, dom.cpu_utilization_pct, agg.tov);

        out.push_back(line);

        snprintf(
            stat_key,
            512,
            "monitor.servers.%s.%s.mem",
            agg.hostname.c_str(),
            dom.domain_uuid.c_str());
        snprintf(
            line,
            1024,
            "%s %f %f",
            stat_key, dom.mem_utilization_pct, agg.tov);

        out.push_back(line);

        snprintf(
            stat_key,
            512,
            "monitor.servers.%s.%s.disk_read_req",
            agg.hostname.c_str(),
            dom.domain_uuid.c_str());
        snprintf(
            line,
            1024,
            "%s %lld %f",
            stat_key, dom.disk_read_req, agg.tov);

        out.push_back(line);

        snprintf(
            stat_key,
            512,
            "monitor.servers.%s.%s.disk_write_req",
            agg.hostname.c_str(),
            dom.domain_uuid.c_str());
        snprintf(
            line,
            1024,
            "%s %lld %f",
            stat_key, dom.disk_write_req, agg.tov);
        
        out.push_back(line);

    }

    return out;
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
            "servers.%s.%s.cpu",
            agg.hostname.c_str(),
            dom.domain_uuid.c_str());
        snprintf(
            line,
            1024,
            "%s %f %f",
            stat_key, dom.cpu_utilization_pct, agg.tov);

        std::cout << line << std::endl;

        snprintf(
            stat_key,
            512,
            "servers.%s.%s.mem",
            agg.hostname.c_str(),
            dom.domain_uuid.c_str());
        snprintf(
            line,
            1024,
            "%s %f %f",
            stat_key, dom.mem_utilization_pct, agg.tov);

        std::cout << line << std::endl;
    }
}

void pubsub_topic_to_path(std::string & key)
{
    for(std::string::iterator it = key.begin();
        it != key.end();
        ++it)
    {
        if(*it == '.')
            *it = '/';
    }
}

//
// really crappy (and probably insecure) substitute for "mkdir -p" because I
// am too lazy to install Boost.Filesystem
//

// return the path components of a topic
// this is the equivalent of: dirname $(key.replace('.', '/'))
std::list<std::string> pubsub_topic_to_path_components(const std::string & key)
{
    std::list<std::string> out;
    
    std::string cur;
    for(std::string::const_iterator it = key.begin();
        it != key.end();
        ++it)
    {
        if(*it == '.')
        {
            cur += '/';
            out.push_back(cur);
        }
        else
            cur += *it;
    }

    return out;
}

void create_directories_from_pubsub_key(const std::string & key)
{
    std::list<std::string> paths = pubsub_topic_to_path_components(key);
    for(std::list<std::string>::const_iterator p = paths.begin();
        p != paths.end();
        ++p)
    {
        const char * path = p->c_str();
        std::cout << "create directory: " << path << "\n";

        struct stat sb;
        if(stat(path, &sb) == 0 && S_ISDIR(sb.st_mode))
            continue; // directory already exists
        else
        {
            int rc = mkdir(path, 0755);
            if(rc != 0)
            {
                perror("mkdir");
                throw std::runtime_error("unable to create directory: " + *p);
            }
        }
    }
}

