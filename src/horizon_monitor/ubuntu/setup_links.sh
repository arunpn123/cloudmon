#!/bin/bash

if [[ `whoami` != "root" ]]; then
    echo "you must run this script as root"
    exit 1
fi

# setup symlinks for the monitor panel
# TODO: this should all be autoconf'd so we can change the install prefix...

PREFIX=/opt/openstack_extra
HORIZON_MONITOR_PATH=$PREFIX/monitor

PYSHARED_LINK=/usr/share/pyshared/horizon/dashboards/nova/monitor
PYDIST_PACKAGES_LINK=/usr/lib/python2.7/dist-packages/horizon/dashboards/nova/monitor

# TODO: just use ln -sf...
if [ ! -h $PYSHARED_LINK ]; then
    ln -s $HORIZON_MONITOR_PATH $PYSHARED_LINK
fi

if [ ! -h $PYDIST_PACKAGES_LINK ]; then
    ln -s $PYSHARED_LINK $PYDIST_PACKAGES_LINK
fi

