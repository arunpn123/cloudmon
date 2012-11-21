import time

#from django.utils.dateparse import parse_datetime
#from django.utils.datastructures import SortedDict
from django.utils.translation import ugettext_lazy as _

#from openstack_dashboard import api
from horizon import api

#from horizon import exceptions

import os
import whisper

##WHISPER_DBS_PREFIX = '/opt/graphite/storage/whisper/monitor'
WHISPER_DBS_PREFIX = '/opt/openstack_extra/monitor_data'

# TODO: probably best to leave data analysis to whisper!
def average(data_points):
    def _in_range(dp, low=0, hi=110):
        if dp is None:
            return False
        elif dp < low or dp > hi:
            return False
        return True
        
    l = [p for p in data_points if _in_range(p)]
    if not l:
        return 0
    return float(sum(l)) / float(len(l))


def get_whisper_files(topdir):
    files = []
    def add_file(arg, dirname, fnames):
        for f in fnames:
            whisper_file = os.path.join(dirname, f)
            if os.path.isfile(whisper_file) and whisper_file.endswith('.wsp'):
                files.append(whisper_file)

    os.path.walk(topdir, add_file, None)
    return files


def get_whisper_files_by_node(node, files=None):
    if files is None:
        files = get_whisper_files(WHISPER_DBS_PREFIX)
    out = []
    for f in files:
        node_id = f.split('/')[-3]
        if node in node_id:
            out.append(f)
    return out


def get_whisper_files_by_instance_id(instance, files=None):
    if files is None:
        files = get_whisper_files(WHISPER_DBS_PREFIX)
    out = []
    for f in files:
        instance_id = f.split('/')[-2]
        if instance in instance_id:
            out.append(f)
    return out


def get_whisper_files_by_metric(metric, files=None):
    if files is None:
        files = get_whisper_files(WHISPER_DBS_PREFIX)
    out = []
    for f in files:
        metric_id = f.split('/')[-1]
        if metric in metric_id:
            out.append(f)
    return out


def get_whisper_files_list(
        topdir,
        node_filter=None,
        instance_filter=None,
        metric_filter=None):

    def match_filter(path, nfilt, ifilt, mfilt):
        if nfilt is None and ifilt is None and mfilt is None:
            return True

        match_nfilt = match_ifilt = match_mfilt = False
        if nfilt is None:
            match_mfilt = True
        if ifilt is None:
            match_ifilt = True
        if mfilt is None:
            match_mfilt = True

        try:
            # last 3 components of path should be node/instUUID/*.wsp
            node, instance_id, fname = path.split('/')[-3:]
            
            if nfilt is not None and nfilt in node:
                match_nfilt = True
            if ifilt is not None and ifilt in instance_id:
                match_ifilt = True
            if mfilt is not None and mfilt in fname:
                match_mfilt = True
        except IndexError:
            return False

        return match_nfilt and match_ifilt and match_mfilt

    out = []
    def add_whisper_file(arg, dirname, fnames):
        for f in fnames:
            whisper_file = os.path.join(dirname, f)
            if os.path.isfile(whisper_file) and whisper_file.endswith('.wsp'):
                if match_filter(whisper_file, *arg):
                    out.append(whisper_file)

    filters = (node_filter, instance_filter, metric_filter)
    os.path.walk(topdir, add_whisper_file, filters)

    return out




###
# note: below comes from the "flocking" tutorial
# doesn't seem to work in my dev environment...
###

### def correlate_tenants(request, instances):
###     # Gather our tenants to correlate against IDs
###     try:
###         tenants = api.keystone.tenant_list(request, admin=True)
###     except:
###         tenants = []
###         msg = _('Unable to retrieve instance tenant information.')
###         exceptions.handle(request, msg)
###     tenant_dict = SortedDict([(t.id, t) for t in tenants])
###     for inst in instances:
###         tenant = tenant_dict.get(inst.tenant_id, None)
###         inst._apiresource._info['tenant'] = tenant._info
###         inst.tenant = tenant
### 
### 
### def correlate_flavors(request, instances):
###     # Gather our flavors to correlate against IDs
###     try:
###         flavors = api.nova.flavor_list(request)
###     except:
###         flavors = []
###         msg = _('Unable to retrieve instance size information.')
###         exceptions.handle(request, msg)
### 
###     flavors_dict = SortedDict([(f.id, f) for f in flavors])
###     for inst in instances:
###         flavor = flavors_dict.get(inst.flavor["id"], None)
###         inst._apiresource._info['flavor'] = flavor._info
###         inst.flavor = flavor
### 
### 
### def correlate_users(request, instances):
###     # Gather our users to correlate against IDs
###     try:
###         users = api.keystone.user_list(request)
###     except:
###         users = []
###         msg = _('Unable to retrieve instance user information.')
###         exceptions.handle(request, msg)
###     user_dict = SortedDict([(u.id, u) for u in users])
###     for inst in instances:
###         user = user_dict.get(inst.user_id, None)
###         inst._apiresource._info['user'] = user._info
###         inst.user = user
### 
### 
### def calculate_ages(instances):
###     for instance in instances:
###         dt = parse_datetime(instance._apiresource.created)
###         timestamp = time.mktime(dt.timetuple())
###         instance._apiresource._info['created'] = timestamp
###         instance.age = dt


#def get_instances_data(request):
#    instances = api.nova.server_list(request, all_tenants=True)
#
#    # Get the useful data... thanks Nova :-P
#    if instances:
#        correlate_flavors(request, instances)
#        correlate_tenants(request, instances)
#        correlate_users(request, instances)
#        calculate_ages(instances)
#
#    return instances

def get_instances_data(request):
    return api.nova.server_list(request)

