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

protected:
    // @todo: use shared_ptr?
    unsigned int m_domainID;
    virDomainPtr m_domain;
    virDomainInfoPtr m_domainInfo;
    // this saves a malloc on call to update()
    virDomainInfoPtr m_tmpInfo;
    timespec m_last_tov;
//    DomainStats m_stats;
};

#endif
