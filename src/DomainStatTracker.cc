#include "DomainStatTracker.hh"
#include <iostream>
#include <stdexcept>

DomainStatTracker::DomainStatTracker(
        virConnectPtr conn,
        unsigned int domID)
    : m_domainID(domID),
      m_domainInfo(0),
      m_tmpInfo(0),
      m_last_tov(timespec_utils::realtime_now())
{
    if(conn == NULL)
        throw std::runtime_error("don't have a valid libvirt connection");

    m_domain = virDomainLookupByID(conn, domID);
    if(m_domain == NULL)
        throw std::runtime_error("can't find domain with given ID");

    virDomainGetUUIDString(m_domain, m_domainUUID);

    m_domainInfo = new virDomainInfo;
    m_tmpInfo = new virDomainInfo;

    std::cout << "DomainStatTracker: monitoring domain ID "
              << m_domainID
              << " (" << m_domainUUID << ")\n";
}

DomainStatTracker::~DomainStatTracker()
{
    std::cout << "DomainStatTracker: destroying tracker for domain "
              << m_domainID << "\n";
    if(m_domainInfo)
        delete m_domainInfo;
    if(m_tmpInfo)
        delete m_tmpInfo;
}

DomainStats DomainStatTracker::update()
{
    DomainStats out;
    out.domain_id = m_domainID;
    out.domain_uuid = m_domainUUID;

    //virDomainInfoPtr cur_info = new virDomainInfo;
    virDomainGetInfo(m_domain, m_tmpInfo);

    timespec prev_tov = m_last_tov;
    m_last_tov = timespec_utils::realtime_now();

    if(!m_domainInfo)
    {
        // dont have a previous update yet (this is the first)
        *m_domainInfo = *m_tmpInfo;
        // @todo: out uninitialized here :(
        return out;
    }

    // calculate CPU utilization
    const timespec & now = m_last_tov;
    double elapsed = timespec_utils::to_double(
        timespec_utils::ts_diff(now, prev_tov));

    unsigned long long cputime_diff = m_tmpInfo->cpuTime - m_domainInfo->cpuTime;
    double cputime_elapsed = (double)cputime_diff / elapsed;
    double cpu_util_pct = cputime_elapsed / 1.0e9;

    // calculate memory utilization
    double mem_util_pct = (double)m_tmpInfo->memory / (double)m_tmpInfo->maxMem;

    out.cpu_utilization_pct = cpu_util_pct;
    out.mem_utilization_pct = mem_util_pct;

    //std::cout << "domain " << m_domainID
    //          << " CPU util: " << cpu_util_pct*100 << "%; "
    //          << " Mem util: " << mem_util_pct*100 << "%\n";

    *m_domainInfo = *m_tmpInfo;

    return out;
}

