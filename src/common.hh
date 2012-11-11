#ifndef __COMMON_HH__
#define __COMMON_HH__

#include "AggregateDomainStats.hh"

#include <list>
#include <string>

void print_aggregate_stats(const AggregateDomainStats & agg);
void print_carbon_update_lines(const AggregateDomainStats & agg);

void pubsub_topic_to_path(std::string & key);
std::list<std::string> pubsub_topic_to_path_components(const std::string & key);
void create_directories_from_pubsub_key(const std::string & key);

#endif

