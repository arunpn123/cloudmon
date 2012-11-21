#include "DomainStatTracker.hh"
#include <iostream>
#include <stdexcept>
#include <stdio.h>
#include <mxml.h>

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

    m_domainInfo = new virDomainInfo;
    m_tmpInfo = new virDomainInfo;
   // m_blockinfo = new virDomainBlockInfo;
    m_interfaceinfo = new virDomainInterfaceStatsStruct;
    m_blockstats = new virDomainBlockStatsStruct;
    m_lastblockstats = new virDomainBlockStatsStruct;
}

DomainStatTracker::~DomainStatTracker()
{
    std::cout << "DomainStatTracker: destroying tracker for domain "
              << m_domainID << "\n";
    if(m_domainInfo)
        delete m_domainInfo;
    if(m_tmpInfo)
        delete m_tmpInfo;
    if(m_blockstats)
        delete m_blockstats;
    if(m_lastblockstats)
        delete m_lastblockstats;
    if(m_interfaceinfo)
        delete m_interfaceinfo;
}

DomainStats DomainStatTracker::update()
{
    DomainStats out;
    //virDomainInfoPtr cur_info = new virDomainInfo;
    if(m_domainID==0) 
       return out;    
   
    virDomainGetInfo(m_domain, m_tmpInfo);
 
    timespec prev_tov = m_last_tov;
    m_last_tov = timespec_utils::realtime_now();

    //getting disk stats   
    //parsing domain xml using mxml
    const char* diskpath;
    int success;
    mxml_node_t *xmltree;
    mxml_node_t *xmldisknode1, *xmldisknode2;
    char * domainxml;
    domainxml = virDomainGetXMLDesc(m_domain, 0);
    xmltree= mxmlLoadString(NULL, domainxml,MXML_TEXT_CALLBACK);
    // xmldisknode1= mxmlFindElement(xmltree, xmltree,"disk","type","block", MXML_DESCEND);
    xmldisknode2= mxmlFindElement(xmltree, xmltree, "target", NULL, NULL, MXML_DESCEND);
    diskpath= mxmlElementGetAttr(xmldisknode2,"dev");
    success= virDomainBlockStats(m_domain, diskpath, m_blockstats, sizeof(m_blockstats));
    if(success==-1)
        std::cout<<"blockstats api error"<<std::endl;


    if(!m_domainInfo)
    {
        // dont have a previous update yet (this is the first)
        *m_domainInfo = *m_tmpInfo;
        m_lastblockstats->rd_req= m_blockstats->rd_req;
        m_lastblockstats->wr_req= m_blockstats->wr_req ;
        m_lastblockstats->rd_bytes= m_blockstats->rd_bytes;
        m_lastblockstats->wr_bytes= m_blockstats->wr_bytes;
        m_lastblockstats->errs= m_blockstats->errs;

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
    out.domain_id = m_domainID;

    //std::cout << "domain " << m_domainID
    //          << " CPU util: " << cpu_util_pct*100 << "%; "
    //          << " Mem util: " << mem_util_pct*100 << "%\n";

    *m_domainInfo = *m_tmpInfo;
    

    out.disk_read_req= m_blockstats->rd_req - m_lastblockstats->rd_req;
    out.disk_write_req= m_blockstats->wr_req - m_lastblockstats->wr_req;
    out.disk_read_size= m_blockstats->rd_bytes - m_lastblockstats->rd_bytes;
    out.disk_write_size= m_blockstats->wr_bytes - m_lastblockstats->wr_bytes;
    out.disk_errors= m_blockstats->errs - m_lastblockstats->errs;
    free(domainxml);

    *m_lastblockstats = *m_blockstats;
   
  
   //network interface stats
    std::cout<<"Domain id :"<<m_domainID<<std::endl;
//   const char* interfacepath= "vif<domainid>.0";
    char interfacepath[64];
    snprintf(interfacepath, 64, "vif%d.0", m_domainID);
    success= virDomainInterfaceStats(m_domain, interfacepath, m_interfaceinfo, sizeof(m_interfaceinfo));
    if(success== -1)
        std::cout<<"Interface stats api Error"<<std::endl;
    return out;
}

