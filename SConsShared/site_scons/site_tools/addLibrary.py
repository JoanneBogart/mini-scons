import os,platform
## Add items in library list to env var LIBS
def generate(env, **kw):
#    if kw.get('package', '') != '':
#        env.AppendUnique(LIBPATH = ['#'+os.path.join(kw.get('package'),'build',platform.system())])
    if kw.get('library','') != '':
        if env.has_key('LIBS'):
            for lib in kw.get('library'):
                if lib in env['LIBS']:
                    env['LIBS'].remove(lib)
                env['LIBS'].append(lib)
        else:
            env.Append(LIBS = kw.get('library'))

def exists(env):
    return 1
