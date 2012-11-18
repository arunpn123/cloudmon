#!/usr/bin/env python

import whisper
import os
import sys

METRIC_SEC_PER_POINT = 1
METRIC_POINTS_TO_STORE = 60*15

def mkdirs(path):
    try:
        os.makedirs(path)
    except OSError, e:
        if e.errno != 17:
            raise

class Updater(object):
    def __init__(self, data_dir):
        self.root = data_dir

        print "updater: root is %s" % self.root

    def terminate_instance(which_instance):
        pass

    def update(self, metric, dataline):
        data = dataline.split()
        metric = os.path.join(self.root, metric)
        metric += '.wsp'

        if not os.path.exists(metric):
            # create this database
            mkdirs(os.path.dirname(metric))
            print "creating %s" % metric
            whisper.create(metric, [(METRIC_SEC_PER_POINT, METRIC_POINTS_TO_STORE)])

        value = float(data[0])
        timestamp = float(data[1])

        whisper.update(metric, value, timestamp)

    def process_cmd(self, line):
        toks = line.split()
        key = toks[0]
        rest = ' '.join(toks[1:])
    
        if not key.startswith('monitor'):
            return
    
        if key == 'monitor.term_instance':
            which_instance = rest
            self.terminate_instance(which_instance)
        elif key.startswith('monitor.servers.'):
            print "update!"
            path = '/'.join(key.split('.')[1:])
            print "path=%s, rest=%s" % (path, str(rest))
            self.update(path, rest)


def main(argv):
    import optparse
    
    default_data_dir = os.path.join(os.getcwd(), "monitor_data")
    
    parser = optparse.OptionParser()
    parser.add_option('', '--data-dir', default=default_data_dir,
        help="path where data files will be stored [default=%default]")

    opts, args = parser.parse_args(argv)

    data_dir = os.path.abspath(opts.data_dir)
    
    mkdirs(data_dir)

    updater = Updater(data_dir)

    while True:
        line = sys.stdin.readline()
        if not line:
            break
        line = line.strip()
        print "received: %s" % line
        updater.process_cmd(line)


if __name__ == '__main__':
    main(sys.argv[1:])
