#ifndef __MESSAGE_HH__
#define __MESSAGE_HH__

#include <zmq.h>
#include <cstring> // for memcpy
#include <cassert>
#include <string>

struct Message
{
    Message()
        : data(0), data_len(0)
    { }
    
    Message(const char * set_key, const char * set_data, size_t set_data_len)
        : key(set_key), data_len(set_data_len)
    {
        data = new char[data_len];
        memcpy(data, set_data, data_len);
    }

    ~Message()
    {
        if(data)
            delete [] data;
    }

    void send(void * sock)
    {
        int rc;
        zmq_msg_t msg_key;
        zmq_msg_init_size(&msg_key, key.length());
        memcpy(zmq_msg_data(&msg_key), key.c_str(), key.length());
        
        rc = zmq_msg_send(&msg_key, sock, ZMQ_SNDMORE);
        assert(rc != -1);
        zmq_msg_close(&msg_key);

        zmq_msg_t msg_data;
        zmq_msg_init_size(&msg_data, data_len);
        memcpy(zmq_msg_data(&msg_data), data, data_len);

        rc = zmq_msg_send(&msg_data, sock, 0);
        assert(rc != -1);
        zmq_msg_close(&msg_data);
    }

    void receive(void * sock)
    {
        zmq_msg_t msg_key;
        zmq_msg_init(&msg_key);
        int rc = zmq_msg_recv(&msg_key, sock, 0);
        assert(rc != -1);

        size_t size = zmq_msg_size(&msg_key);
        char * buf = new char[size + 1];
        memcpy(buf, zmq_msg_data(&msg_key), size);
        buf[size] = 0;
        key = buf;
        delete [] buf;
        zmq_msg_close(&msg_key);

        int64_t more;
        size_t more_size = sizeof(more);
        rc = zmq_getsockopt(sock, ZMQ_RCVMORE, &more, &more_size);

        assert(rc == 0);
        assert(more);

        zmq_msg_t msg_data;
        zmq_msg_init(&msg_data);
        rc = zmq_msg_recv(&msg_data, sock, 0);
        assert(rc != -1);

        size = zmq_msg_size(&msg_data);
        data = new char[size];
        memcpy(data, zmq_msg_data(&msg_data), size);
        data_len = size;
        zmq_msg_close(&msg_data);
    }

    std::string key;
    char * data;
    unsigned int data_len;
};

#endif

