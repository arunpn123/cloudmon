#ifndef __MY_ZHELPERS_HH__
#define __MY_ZHELPERS_HH__

#include <zmq.h>
#include <stdlib.h>
#include <string.h>

static char *s_recv(void *socket)
{
    zmq_msg_t message;
    zmq_msg_init(&message);
    int size = zmq_msg_recv(&message, socket,  0);
    if(size == -1)
        return NULL;
    char *outstr = (char *)malloc(size+1);
    memcpy(outstr, zmq_msg_data(&message), size);
    zmq_msg_close(&message);
    outstr[size] = 0;
    return outstr;
}

static int s_send(void *socket, const char *outstr)
{
    zmq_msg_t message;
    zmq_msg_init_size(&message, strlen(outstr));
    memcpy(zmq_msg_data(&message), outstr, strlen(outstr));
    int size = zmq_msg_send(&message, socket, 0);
    zmq_msg_close(&message);
    return size;
}

#endif
