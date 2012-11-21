from django.shortcuts import render_to_response
from django import http

from horizon import views
from horizon import tabs
from .tabs import MonitorTabs
from .tables import MonitorTable

#from openstack_dashboard import api
from horizon import api

import os
import whisper
import time

import utils

class MonitoredDomain(object):
    """
    represents a single *set* of .wsp files for a particular
    instance (e.g. a node/domain combination).  There may be multiple
    .wsp files representing different collection metrics
    """
    def __init__(self, node_id, domain_id):
        self.files = utils.get_whisper_files_list(WHISPER_DBS_PREFIX, node_id, domain_id)
        self.node_id = node_id
        self.domain_id = domain_id

    def get_metric_names(self):
        return set(
            [os.path.basename(f).replace('.wsp', '') for f in self.files])

    def map_metric_to_files(self, metric):
        return [f for f in self.files if metric in f]

    def extract_data(self, metric, start, end=None):
        f = self.map_metric_to_files(metric)
        if not f:
            return None

        fetched = whisper.fetch(f[0], start, end)
        return fetched

def index_view_basic(*args, **kwargs):
    #return http.HttpResponse("hello, world")
    return render_to_response('nova/monitor/index.html')


#class IndexView(views.APIView):
class IndexView(tabs.TabbedTableView):
    # A very simple class-based view...
    tab_group_class = MonitorTabs
    table_class = MonitorTable
    #template_name = 'project/monitor/index.html'
    template_name = 'nova/monitor/index.html'

    def get(self, request, *args, **kwargs):
        return super(IndexView, self).get(request, *args, **kwargs)

    def _my_get_data(self, request, context, *args, **kwargs):
        # Add data to the context here...

        instances = api.server_list(self.request)
        for i in instances[0]._attrs:
            has = getattr(instances[0], i, "no")
            print "%s? %s" % (i, has)


        context["data"] = []
        for instance in instances:
            whisper_files = utils.get_whisper_files_list(WHISPER_DBS_PREFIX)

            this_instance_files = [x for x in whisper_files if instance.id in x]

            for f in this_instance_files:
                if "cpu" not in f: continue

                fetched = whisper.fetch(f, time.time() - 10)
                times, data = fetched
                context["data"].append((instance.id, data[-1]))

##        prefix = WHISPER_DBS_PREFIX
##        whisper_files = get_whisper_files_list(prefix, domain_filter=instances[0].id)
##
##        # for each whisper file found, query the last 30 seconds of data from
##        # it and format it it into the context
##        context["data"] = []
##        for f in sorted(whisper_files):
##            key = f.replace(prefix + '/', '')
##            
##            this_context = {}
##            this_context["name"] = key
##            
##            fetched = whisper.fetch(f, time.time() - 30)
##            times, data = fetched
##            
##            if len(set(data)) == 1 and None in set(data):
##                continue
##
##            start, end, step = times
##            values = []
##            for tv, val in zip(xrange(start, end, step), data):
##                if val is None:
##                    val = "n/a"
##                values.append({"time": tv, "value": val})
##
##            this_context["items"] = values
##
##            context["data"].append(this_context)
##
##        print "\n\n\n", context, "\n\n\n"
        return context


class NodeDetailView(views.APIView):
    table_class = MonitorTable
    #template_name = 'project/monitor/node_detail.html'
    template_name = 'nova/monitor/node_detail.html'

    def get_data(self, request, context, *args, **kwargs):
        node_id = self.kwargs['node_id']

        context["node_id"] = node_id

        return context


class InstanceDetailView(views.APIView):
    table_class = MonitorTable
    #template_name = 'project/monitor/instance_detail.html'
    template_name = 'nova/monitor/instance_detail.html'

    def get_data(self, request, context, *args, **kwargs):
        instance_id = self.kwargs['instance_id']
        context['instance_id'] = instance_id

##        start = time.time()
##
##        monitor = MonitoredDomain(node_id, domain_id)
##        for metric in monitor.get_metric_names():
##            last = [60, 60*5, 60*15]
##
##            for when in last:
##                fetched = monitor.extract_data(metric, start - when)
##                key = "avg_%s_%d" % (metric, when)
##                context[key] = average(fetched[1])
##
##                print key
##                print context[key]

        return context

