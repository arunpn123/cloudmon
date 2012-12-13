#ifndef __DOMAIN_STAT_TRACKER_HH__
#define __DOMAIN_STAT_TRACKER_HH__

#include "AggregateDomainStats.hh"
#include "timespec_utils.hh"
#include <libvirt/libvirt.h>

class DomainStatTracker
{
public:
    explicit DomainStatTracker(virConnectPtr conn, unsigned int domID);
    virtual ~DomainStatTracker();

    DomainStats update();

    unsigned int domainID() const
    { return m_domainID; }
    std::string domainUUID() const
    { return m_domainUUID; }

protected:
    // @todo: use shared_ptr?
    unsigned int m_domainID;
    char m_domainUUID[VIR_UUID_STRING_BUFLEN];
    virDomainPtr m_domain;
    virDomainInfoPtr m_domainInfo;
    // this saves a malloc on call to update()
    virDomainInfoPtr m_tmpInfo;
    timespec m_last_tov;

    virDomainBlockInfoPtr m_blockinfo;
    virDomainInterfaceStatsPtr m_interfaceinfo;
    virDomainInterfaceStatsPtr m_lastinterfaceinfo;
    virDomainBlockStatsPtr m_blockstats;
    virDomainBlockStatsPtr m_lastblockstats;
//    virDomainBlockStatsPtr m_tmpblockstats;
//    DomainStats m_stats;
};

#endif

