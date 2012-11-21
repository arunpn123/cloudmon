from django.utils.translation import ugettext_lazy as _

import horizon

##from openstack_dashboard.dashboards.project import dashboard
from horizon.dashboards.nova import dashboard

class Monitor(horizon.Panel):
    name = _("Monitor")
    slug = "monitor"


dashboard.Nova.register(Monitor)
