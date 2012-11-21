SHELL = /bin/bash

MSGPACK_VERSION = 0.5.4
ZMQ_VERSION = 3.2.1
ZMQ_VERSION_TAR = $(ZMQ_VERSION)-rc2
MXML_VERSION = 2.7

all: external
external: zeromq msgpack mxml

mxml:
	if [ ! -e $(PWD)/mxml-install/lib/libmxml.so ]; then \
            tar xvzf mxml-$(MXML_VERSION).tar.gz; \
            cd mxml-$(MXML_VERSION); \
            ./configure --prefix=$(PWD)/mxml-install; \
            make; \
            make install; \
        fi

msgpack:
	if [ ! -e $(PWD)/msgpack-install/lib/libmsgpack.so ]; then \
            tar xvzf msgpack-$(MSGPACK_VERSION).tar.gz; \
            cd msgpack-$(MSGPACK_VERSION); \
            ./configure --prefix=$(PWD)/msgpack-install; \
            make; \
            make install; \
	fi

zeromq:
	if [ ! -e $(PWD)/zeromq-install/lib/libzmq.so ]; then \
            tar xvzf zeromq-$(ZMQ_VERSION_TAR).tar.gz; \
            cd zeromq-$(ZMQ_VERSION); \
            ./configure --prefix=$(PWD)/zeromq-install; \
            make; \
            make install; \
        fi

clean:
	rm -rf msgpack-install zeromq-install
