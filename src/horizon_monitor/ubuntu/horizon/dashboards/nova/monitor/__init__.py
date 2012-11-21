# TODO: assumes python2.7...bad!
# should really just 'apt-get install python-whisper'
WHISPER_BASE = '/opt/openstack_extra/whisper'
WHISPER_PYPATH = WHISPER_BASE + '/lib/python2.7/site-packages'

import sys
if WHISPER_PYPATH not in sys.path:
    sys.path.append(WHISPER_PYPATH)

