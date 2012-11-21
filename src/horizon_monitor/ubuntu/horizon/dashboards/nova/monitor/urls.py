from django.conf.urls.defaults import patterns, url

from .views import IndexView, NodeDetailView, InstanceDetailView

# ideally, we want a hierarchy something like this:
#   /  (index) -> an overview of all physical nodes in the system, along with
#                 some very basic data about them (# of instances on each, dom0
#                 load, etc.)
#   /nodes     -> redirects to /
#   /nodes/[hostname] -> detail view of a physical node.  Provides a list of
#                        currently active instances on that node, and some 
#                        basic data about them
#   /instances        -> could be the same as /nodes/[hostname] but for all
#                        nodes, and wouldn't aggregate by physical node but
#                        just display all the instances
#   /instances/[instance_id] -> display detailed statistics about a particular
#                               instance
#   /nodes/[hostname]/[instance_id] -> redirects to /instances/[instance_id]
#
# Note that in order for [instance_id] to work correctly, we need a way of
# mapping [instance_id] -> [node_hostname].dom###
# In other words, we need to know which Xen domain an OpenStack instance ID
# maps to in order to extract data about that domain from the RRD
#
# Alternatively - if we can keep these two tightly coupled (e.g. have the
# monitoring daemon explicitly know about OpenStack instance IDs) then the
# mapping becomes more natural.  However, this pretty much just means we
# need to remap a Xen domain ID to an instance ID internally in the monitor
# daemon, which is likely more complicated anyway
#
# For now, let's only support the following URLs:
#    / or /nodes
#    /nodes/[hostname]
#    /nodes/[hostname]/[domain_id]

urlpatterns = patterns('',
    url(r'^$', IndexView.as_view(), name='index'),
###    url(r'^nodes/(?P<node_id>[^/]+)/$', NodeDetailView.as_view(), name='node_detail'),
###
###    url(r'^instances/(?P<instance_id>[^/]+)/$', InstanceDetailView.as_view(), name='instance_detail'),
)

