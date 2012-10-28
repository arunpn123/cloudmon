#ifndef __DOMAIN_STAT_PUBLISHER_HH__
#define __DOMAIN_STAT_PUBLISHER_HH__

#include "Message.hh"
#include "ActiveDomainWatcher.hh"
#include "AggregateDomainStats.hh"
#include "DomainStatTracker.hh"

#include <string>
#include <cstring> // for strerror
#include <unistd.h>

namespace detail {
void _add_stat_tracker(DomainStatPublisher & self, int domID);
void _remove_stat_tracker(DomainStatPublisher & self, int domID);
}

class DomainStatPublisher
{
public:
    DomainStatPublisher(
        void * zmq_context,
        virConnectPtr hv_conn,
        const std::string & endpoint)
        : m_context(zmq_context),
          m_conn(hv_conn),
          m_publisher(NULL),
          m_endpoint(endpoint),
          m_domainWatcher(
              m_conn,
              *this,
              detail::_add_stat_tracker,
              detail::_remove_stat_tracker
          )
          // how i miss boost::bind :(
          //m_domainWatcher(
          //    m_conn,
          //    boost::bind(&DomainStatPublisher::addStatTracker, this, _1),
          //    boost::bind(&DomainStatPublisher::removeStatTracker, this, _1))
    {
        char hostname[512];
        int rc = gethostname(hostname, 512);
        if(rc != 0)
        {
            std::cerr << "DomainStatPublisher: "
                      << "WARNING: unable to get hostname: " 
                      << strerror(errno) << "\n";
        }
        else
        {
            m_aggStats.hostname = hostname;
        }

        m_publisher = zmq_socket(m_context, ZMQ_PUB);
        rc = zmq_connect(m_publisher, m_endpoint.c_str());
        assert(rc == 0);
    }

    virtual ~DomainStatPublisher()
    {
        for(StatTrackerMapType::const_iterator it = m_trackers.begin();
            it != m_trackers.end();
            ++it)
        {
            if(it->second)
                delete it->second;
        }
        
        if(m_publisher)
        {
            int linger = 0;
            zmq_setsockopt(m_publisher, ZMQ_LINGER, &linger, sizeof(linger));
            zmq_close(m_publisher);
        }
    }

    void update()
    {
        m_domainWatcher.poll();

        // TODO: horribly inefficient...
        m_aggStats.domain_stats.clear();
        for(StatTrackerMapType::const_iterator it = m_trackers.begin();
            it != m_trackers.end();
            ++it)
        {
            m_aggStats.domain_stats.push_back(it->second->update());
        }

        m_aggStats.tov = timespec_utils::to_double(
            timespec_utils::realtime_now());

        msgpack::sbuffer sbuf;
        msgpack::pack(sbuf, m_aggStats);

        Message m("monitor_data", sbuf.data(), sbuf.size());
        m.send(m_publisher);
    }

    void addStatTracker(int domID)
    {
        if(m_trackers.find(domID) != m_trackers.end())
        {
            std::cout << "warning: adding domain tracker with id " << domID
                      << ", but I already had an old one?\n";
            if(m_trackers[domID])
                delete m_trackers[domID];
        }
        
        m_trackers[domID] = new DomainStatTracker(m_conn, domID);
    }

    void removeStatTracker(int domID)
    {
        if(m_trackers.find(domID) == m_trackers.end())
        {
            std::cout << "warning: hypervisor removed domain with id " << domID
                      << ", but I wasn't tracking it?  Ignoring...";
            return;
        }

        if(m_trackers[domID])
            delete m_trackers[domID];
    }

private:
    void * m_context;
    void * m_publisher;
    virConnectPtr m_conn;
    const std::string & m_endpoint;

    ActiveDomainWatcher m_domainWatcher;
    
    typedef std::map<int, DomainStatTracker *> StatTrackerMapType;
    StatTrackerMapType m_trackers;

    AggregateDomainStats m_aggStats;
};

// a silly, overly complicated workaround for not having boost::bind :(
namespace detail
{
    void _add_stat_tracker(DomainStatPublisher & self, int domID)
    {
        self.addStatTracker(domID);
    }

    void _remove_stat_tracker(DomainStatPublisher & self, int domID)
    {
        self.removeStatTracker(domID);
    }
}

#endif

