#!/bin/bash

if [[ `whoami` != "root" ]]; then
    echo "you must be root to run this script"
    exit 1
fi

PREFIX=/opt/openstack_extra

mkdir -p $PREFIX
cp -a horizon $PREFIX

