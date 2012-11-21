from horizon import tabs
from .tables import MonitorTable

import utils

class DataTab(tabs.TableTab):
    name = _("Data")
    slug = 'data'
    table_classes = (MonitorTable,)
    template_name = 'horizon/common/_detail_table.html'
    preload = False

    def get_instances_data(self):
        return utils.get_instances_data(self.request)


class MonitorTabs(tabs.TabGroup):
    slug = 'monitor'
    tabs = (DataTab,)

