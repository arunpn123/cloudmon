#!/bin/bash

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/tmp/blaub6/zeromq-3.2.0-install/lib
./basic_monitor $*
