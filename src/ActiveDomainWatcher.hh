#ifndef __ACTIVE_DOMAIN_WATCHER_HH__
#define __ACTIVE_DOMAIN_WATCHER_HH__

#include <libvirt/libvirt.h>
#include <set>

#include <functional>
#include <cassert>
#include <iostream>

class DomainStatPublisher;

class ActiveDomainWatcher
{
public:
    // TODO: with std::bind or boost::bind, these become much more secure
    // and clean!
    typedef void (*DomainAddedHandler)(DomainStatPublisher & pub, int domainID);
    typedef void (*DomainRemovedHandler)(DomainStatPublisher & pub, int domainID);

    typedef std::set<int> DomainSet;

    ActiveDomainWatcher(
        virConnectPtr conn,
        DomainStatPublisher & stat_publisher,
        DomainAddedHandler add_domain_handler,
        DomainRemovedHandler remove_domain_handler)
            : m_conn(conn),
              m_publisher(stat_publisher),
              m_add_domain_handler(add_domain_handler),
              m_remove_domain_handler(remove_domain_handler)
    { }

    void poll()
    {
        // query the hypervisor to get a list of active domains, and
        // compare with our current set
        // call add_domain_handler for any new domains that have appeared,
        // and remove_domain_handler for those that have disappeared
        int num_domains = virConnectNumOfDomains(m_conn);

        // should have at least a domain 0
        assert(num_domains > 0);

        DomainSet domains_this_update;
        int *hv_active_domains = new int[num_domains];

        num_domains = virConnectListDomains(m_conn, hv_active_domains, num_domains);
        for(int i = 0; i < num_domains; i++)
            domains_this_update.insert(hv_active_domains[i]);

        // for any domains in our current active set that are not in this
        // update, assume they have disappeared
        for(DomainSet::const_iterator it = m_active.begin();
            it != m_active.end();
            ++it)
        {
            if(domains_this_update.find(*it) == domains_this_update.end())
            {
                std::cout << "ActiveDomainWatcher: domain ID "
                          << *it << " seems to have disappeared\n";
                m_remove_domain_handler(m_publisher, *it);
            }
        }

        // for any domains in the new set that do not appear in our previous
        // set, assume they are newly created
        for(DomainSet::const_iterator it = domains_this_update.begin();
            it != domains_this_update.end();
            ++it)
        {
            if(m_active.find(*it) == m_active.end())
            {
                std::cout << "ActiveDomainWatcher: new domain with ID "
                          << *it << " has appeared\n";
                m_add_domain_handler(m_publisher, *it);
            }
        }

        // TODO: inefficient...
        m_active = domains_this_update;
    }

private:
    virConnectPtr m_conn;

    DomainStatPublisher & m_publisher;
    DomainAddedHandler m_add_domain_handler;
    DomainRemovedHandler m_remove_domain_handler;

    DomainSet m_active;
};


#endif

