# -*- python -*-
# $Header: /nfs/farm/g/glast/u52/cvs_test/cvs/ScienceTools_User-scons/SConstruct,v 1.1 2017/07/21 23:32:24 jrb Exp $
# Authors: Navid Golpayegani <golpa@slac.stanford.edu>, Joanne Bogart <jrb@slac.stanford.edu>
# Version: mini-scons-02-01-00

import os,platform,SCons,sys
#########################
#   Global Environment  #
#########################

print "\nThis build is running on: ", platform.node(), "\n"

print "Argument list (one per line):"
for arg in sys.argv:
    print "=> ",arg
print "\n"

myVars = {'myPrefix': 'mini'}
SConscript(os.path.join('SConsShared','SConstruct_common.scons'), exports='myVars')
