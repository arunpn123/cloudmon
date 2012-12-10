#!/bin/bash

sandbox_dir=cloud_monitor-installer

zmq_dir=external/zeromq-install
msgpack_dir=external/msgpack-install
mxml_dir=external/mxml-install

set -x

mkdir -p $sandbox_dir/bin
mkdir -p $sandbox_dir/lib

cp src/basic_monitor src/basic_receiver $sandbox_dir/bin
cp src/sm_ctl $sandbox_dir/bin

cp -d $zmq_dir/lib/*.so* $zmq_dir/lib/*.la $sandbox_dir/lib
cp -d $msgpack_dir/lib/*.so* $sandbox_dir/lib
cp -d $mxml_dir/lib/*.so* $sandbox_dir/lib

cp install.sh $sandbox_dir

