#$Id$

def generate(env, **kw):
    if not kw.get('depsOnly',0):
	env.Tool('addLibrary', library = ['facilities'])
	if env['PLATFORM'] == 'win32' and env.get('CONTAINERNAME','')=='GlastRelease':

	    env.Tool('findPkgPath', package = 'facilities') 
def exists(env):
    return 1;
