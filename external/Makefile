SHELL = /bin/bash

ZMQ_VERSION = 3.2.1
ZMQ_VERSION_TAR = $(ZMQ_VERSION)-rc2
ZMQ_SRC = zeromq-$(ZMQ_VERSION_TAR).tar.gz
ZMQ_SRC_URL = http://download.zeromq.org/$(ZMQ_SRC)

MSGPACK_VERSION = 0.5.7
MSGPACK_SRC = msgpack-$(MSGPACK_VERSION).tar.gz
MSGPACK_SRC_URL = http://msgpack.org/releases/cpp/$(MSGPACK_SRC)

MXML_VERSION = 2.7
MXML_SRC = mxml-2.7.tar.gz
MXML_SRC_URL = http://ftp.easysw.com/pub/mxml/$(MXML_VERSION)/$(MXML_SRC)

all: external
external: zeromq msgpack mxml

mxml:
	if [ ! -e $(MXML_SRC) ]; then \
            wget $(MXML_SRC_URL); \
        fi; \
	if [ ! -e $(PWD)/mxml-install/lib/libmxml.so ]; then \
            tar xvzf mxml-$(MXML_VERSION).tar.gz; \
            cd mxml-$(MXML_VERSION); \
            ./configure --prefix=$(PWD)/mxml-install; \
            make; \
            make install; \
        fi

msgpack:
	if [ ! -e $(MSGPACK_SRC) ]; then \
            wget $(MSGPACK_SRC_URL); \
        fi; \
	if [ ! -e $(PWD)/msgpack-install/lib/libmsgpack.so ]; then \
            tar xvzf msgpack-$(MSGPACK_VERSION).tar.gz; \
            cd msgpack-$(MSGPACK_VERSION); \
            ./configure --prefix=$(PWD)/msgpack-install; \
            make; \
            make install; \
	fi

zeromq:
	if [ ! -e $(ZMQ_SRC) ]; then \
            wget $(ZMQ_SRC_URL); \
        fi; \
        if [ ! -e $(PWD)/zeromq-install/lib/libzmq.so ]; then \
            tar xvzf zeromq-$(ZMQ_VERSION_TAR).tar.gz; \
            cd zeromq-$(ZMQ_VERSION); \
            ./configure --prefix=$(PWD)/zeromq-install; \
            make; \
            make install; \
        fi

clean:
	rm -rf msgpack-install zeromq-install $(MSGPACK_SRC) $(ZMQ_SRC)

