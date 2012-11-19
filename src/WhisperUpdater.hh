#ifndef __RRD_PROXY_HH__
#define __RRD_PROXY_HH__

#include "common.hh"
#include "Message.hh"

#include <zmq.h>
#include <msgpack.hpp>

#include <cstring>
#include <cstdio>
#include <cassert>

#include <string>
#include <stdexcept>

#include <iostream>

/**
 * handles proxying updates to whisper RRD files on disk
 * this runs in a separate thread and subscribes for all messages
 * received by the main monitor thread. It manages creation, update,
 * and deletion of RRD files for each virtual host.  This is really
 * a poor workaround for using either (a) the RRDtool C-style interface
 * or (b) combination of Whisper/Carbon (Carbon accepts updates via a BSD
 * socket and handles creation automatically).
 *
 * Be warned: There are a lot of inefficiencies here!
 * This is mostly to cut down on the number of dependencies we need.
 * Alternatively, we could:
 *    - install Carbon and use it
 *    - use RRDtool (which has a C interface) instead of Whisper, but
 *      this complicates the Python code integrated with the Dashboard
 *    - embed the Python interpreter, and make Whisper calls directly
 *      (but this requires Python development headers/libraries)
 *
 * This uses popen() as a crappy alternative.  Whisper updates are managed
 * via a python script that reads commands via stdin.
 */

class WhisperRRDProxy
{
public:
    WhisperRRDProxy(
        void * zmq_context,
        const std::string & basepath)
        : m_context(zmq_context),
          m_basepath(basepath),
          m_shutdown(false),
          m_updater_path(basepath + "/utils/whisper_updater.sh"),
          m_rrd_data_path(basepath + "/monitor_data"),
          m_updater(NULL)
    { }

    virtual ~WhisperRRDProxy()
    {
        if(m_updater)
            pclose(m_updater);
    }

    void terminate()
    { m_shutdown = true; }

    void run()
    {
        std::string cmd = m_updater_path + " --data-dir " + m_rrd_data_path;

        m_updater = popen(cmd.c_str(), "w");
        if(!m_updater)
        {
            perror("popen");
            throw std::runtime_error(
                "WhisperRRDProxy: unable to create pipe to child process!");
        }
        
        run_proxy_loop();
    }

protected:
    void run_proxy_loop()
    {
        void * sock = zmq_socket(m_context, ZMQ_SUB);
        assert(sock);

        const char * mon_filter = "monitor.servers.";
        zmq_setsockopt(sock, ZMQ_SUBSCRIBE, mon_filter, strlen(mon_filter));
        const char * term_filter = "monitor.term_instance";
        zmq_setsockopt(sock, ZMQ_SUBSCRIBE, term_filter, strlen(term_filter));

        zmq_connect(sock, "inproc://monitor");

        zmq_pollitem_t pollitems = { sock, 0, ZMQ_POLLIN, 0 };

        while(!m_shutdown)
        {
            int rc = zmq_poll(&pollitems, 1, 100);
            if(rc > 0)
            {
                // receive the next message from the ZMQ side
                Message next;
                next.receive(sock);

                msgpack::unpacked msg;
                msgpack::unpack(&msg, next.data, next.data_len);

                if(next.key == "monitor.term_instance")
                {
                    std::string which_instance;
                    msg.get().convert(&which_instance);

                    fprintf(
                        m_updater,
                        "%s %s\n",
                        next.key.c_str(), which_instance.c_str());
                }
                else
                {
                    AggregateDomainStats agg;
                    msg.get().convert(&agg);

                    std::list<std::string> updates = get_whisper_updates(agg);
                    for(std::list<std::string>::const_iterator it = updates.begin();
                        it != updates.end();
                        ++it)
                    {
                        fprintf(m_updater, "%s\n", it->c_str());
                        fflush(m_updater);
                    }
                }
            }
        }

        zmq_close(&sock);
    }

    void * m_context;
    std::string m_basepath;
    std::string m_updater_path;
    std::string m_rrd_data_path;
    bool m_shutdown;
    FILE * m_updater;
};

#endif

