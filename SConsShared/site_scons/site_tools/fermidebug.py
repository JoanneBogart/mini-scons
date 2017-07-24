#
# Copyright (c) 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008, 2009, 2010 The SCons Foundation
#
# Permission is hereby granted, free of charge, to any person obtaining
# a copy of this software and associated documentation files (the
# "Software"), to deal in the Software without restriction, including
# without limitation the rights to use, copy, modify, merge, publish,
# distribute, sublicense, and/or sell copies of the Software, and to
# permit persons to whom the Software is furnished to do so, subject to
# the following conditions:
#
# The above copyright notice and this permission notice shall be included
# in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY
# KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
# WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
# LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
# OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
#  $Id: fermidebug.py,v 1.1.1.1 2017/05/06 00:23:53 jrb Exp $
__doc__ = """
Make debugging output stream available via env variable SCONS_FERMI_DEBUG
"""

##import copy
import os
##import subprocess
##import re

###import SCons.Util


logfile = os.environ.get('SCONS_FERMI_DEBUG')
if logfile == '-':
    def fdebug(x):
        print x
elif logfile:
    try:
        import logging
    except ImportError:
        fdebug = lambda x: open(logfile, 'a').write(x + '\n')
    else:
        logging.basicConfig(filename=logfile, level=logging.DEBUG)
        fdebug = logging.debug
else:
    fdebug = lambda x: None




# Local Variables:
# tab-width:4
# indent-tabs-mode:nil
# End:
# vim: set expandtab tabstop=4 shiftwidth=4:
