#ifndef __DOMAIN_STAT_PUBLISHER_HH__
#define __DOMAIN_STAT_PUBLISHER_HH__

#include "Message.hh"
#include "ActiveDomainWatcher.hh"
#include "AggregateDomainStats.hh"
#include "DomainStatTracker.hh"

#include <string>
#include <cstring> // for strerror
#include <unistd.h>

class DomainStatPublisher;

namespace detail {
void _add_stat_tracker(DomainStatPublisher & self, int domID);
void _remove_stat_tracker(DomainStatPublisher & self, int domID);
}

class DomainStatPublisher
{
public:
    DomainStatPublisher(
        void * zmq_context,
        const std::string & endpoint)
        : m_context(zmq_context),
          m_publisher(NULL),
          m_endpoint(endpoint)
    {
        char hostname[512];
        int rc = gethostname(hostname, 512);
        if(rc != 0)
        {
            std::cerr << "DomainStatPublisher: "
                      << "WARNING: unable to get hostname: " 
                      << strerror(errno) << "\n";
            m_hostname = "unknown-host";
        }
        else
        {
            m_hostname = hostname;
        }

        m_publisher = zmq_socket(m_context, ZMQ_PUB);
        rc = zmq_connect(m_publisher, m_endpoint.c_str());
        assert(rc == 0);
    }

    virtual ~DomainStatPublisher()
    {        
        if(m_publisher)
        {
            int linger = 0;
            zmq_setsockopt(m_publisher, ZMQ_LINGER, &linger, sizeof(linger));
            zmq_close(m_publisher);
        }
    }

    virtual void update() = 0;
    virtual void addStatTracker(int) = 0;
    virtual void removeStatTracker(int) = 0;

protected:
    void * m_context;
    void * m_publisher;
    std::string m_endpoint;
    std::string m_hostname;
};

class LinuxHostStatPublisher : public DomainStatPublisher
{
public:
    LinuxHostStatPublisher(
        void * zmq_context,
        const std::string & endpoint,
        int fake_domain_id = 9999)
        : DomainStatPublisher(zmq_context, endpoint)
    {
        m_aggStats.domain_stats.push_back(DomainStats());
        m_aggStats.hostname = m_hostname;
        m_fake_domain_id = fake_domain_id;
    
        std::cout << "LinuxHostStatPublisher: creating stat publisher for "
                  << "local kernel statistics with fake domain id "
                  << fake_domain_id << "\n";
    }

    virtual ~LinuxHostStatPublisher()
    { }

    virtual void update()
    {
        m_aggStats.tov = timespec_utils::to_double(
            timespec_utils::realtime_now());
        m_aggStats.domain_stats[0].domain_id = m_fake_domain_id;

        m_aggStats.domain_stats[0].cpu_utilization_pct = 1;
        m_aggStats.domain_stats[0].mem_utilization_pct = 0.5;

        msgpack::sbuffer sbuf;
        msgpack::pack(sbuf, m_aggStats);

        Message m("monitor_data", sbuf.data(), sbuf.size());
        m.send(m_publisher);
    }

    virtual void addStatTracker(int)
    { }
    virtual void removeStatTracker(int)
    { }

protected:
    AggregateDomainStats m_aggStats;
    int m_fake_domain_id;
};

class LibvirtDomainStatPublisher : public DomainStatPublisher
{
public:
    LibvirtDomainStatPublisher(
        void * zmq_context,
        const std::string & endpoint,
        virConnectPtr hv_conn)
        : DomainStatPublisher(zmq_context, endpoint),
          m_conn(hv_conn),
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
        m_aggStats.hostname = m_hostname;
    }

    virtual ~LibvirtDomainStatPublisher()
    {
        for(StatTrackerMapType::const_iterator it = m_trackers.begin();
            it != m_trackers.end();
            ++it)
        {
            if(it->second)
                delete it->second;
        }
    }

    virtual void update()
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

    virtual void addStatTracker(int domID)
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

    virtual void removeStatTracker(int domID)
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
    virConnectPtr m_conn;
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

