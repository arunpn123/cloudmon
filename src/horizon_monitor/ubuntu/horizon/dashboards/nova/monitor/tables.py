from horizon import tables
from django.utils.translation import ugettext_lazy as _

import utils
import whisper
import time

def get_vcpu_load_avgs(instance):
    cpu_files = utils.get_whisper_files_by_metric(
                "cpu",
                utils.get_whisper_files_by_instance_id(instance.id))

    if not cpu_files:
        return "N/A"

    # TODO: what if we have more than one CPU database??
    f = cpu_files[0]
    avgs = []

    for duration in [1, 5, 15]:
        fetched = whisper.fetch(f, time.time() - 60*duration)
        times, data = fetched
        load_avg = utils.average(data)
        avgs.append(load_avg)

    return "%f | %f | %f" % tuple(avgs)


def get_cpu_utilization_snapshot(instance, back=60):
    cpu_files = utils.get_whisper_files_by_metric(
                "cpu",
                utils.get_whisper_files_by_instance_id(instance.id))
    
    if not cpu_files:
        return "N/A"

    # TODO: what if we have more than one CPU database??
    fetched = whisper.fetch(cpu_files[0], time.time() - back)
    times, data = fetched

    # return the most recent data point that's not None
    for val in reversed(data):
        if val is not None:
            return val * 100

    return "N/A"


def get_mem_utilization_snapshot(instance, back=60):
    mem_files = utils.get_whisper_files_by_metric(
                "mem",
                utils.get_whisper_files_by_instance_id(instance.id))

    if not mem_files:
        return "N/A"

    fetched = whisper.fetch(mem_files[0], time.time() - back)
    times, data = fetched

    for val in reversed(data):
        if val is not None:
            return val * 100

    return "N/A"


class MonitorTable(tables.DataTable):
    name = tables.Column("name",
                         #link=("horizon:project:instances:detail"),
                         link=("horizon:nova:instances_and_volumes:instances:detail"),
                         verbose_name=_("Instance Name"))
    host = tables.Column("OS-EXT-SRV-ATTR:host",
                         verbose_name=_("Host"))
    cpu = tables.Column(get_cpu_utilization_snapshot, verbose_name=_("CPU%"))
    mem = tables.Column(get_mem_utilization_snapshot, verbose_name=_("Mem%"))
    vcpu_load = tables.Column(get_vcpu_load_avgs,
                              verbose_name=_("vCPU Load Avg (1, 5, 15 min)"))

    class Meta:
        name = 'instances'
        verbose_name = _("Instances")

