# -*- python -*-
import os,re,commands
import SCons.Builder
import SCons.Action
from fermidebug import fdebug

def generate(env, **kw):
    def createDoxygen(target = None, source = None, env = None):
        fdebug("Creating Doxygen configuration for '%s'" % (source[0]))
        defaultDoxy = open(source[0].abspath).read()
        defaultDoxy = defaultDoxy.replace("${PROJECT-NAME}", os.path.splitext(os.path.split(target[0].abspath)[1])[0])
        sconscript = open(source[1].abspath).read();
        version = re.compile("\s*Version:\s*(.+)\s*").search(sconscript).group(1)
        defaultDoxy = defaultDoxy.replace("${PROJECT-NUMBER}", version)
        defaultDoxy = defaultDoxy.replace("${HTML-OUTPUT}", env['DOXYGENOUTPUT'])
        if len(source) == 3:
            overrideDoxy = open(source[2].abspath).read()
            defaultDoxy += overrideDoxy
        open(str(target[0]), 'w').write(defaultDoxy)
        return 0

    def createDoxygenEmitter(target, source, env):
        source = [env['DEFAULTDOCFILE']]
        source += [env.Dir('.').File("SConscript")]
        if os.path.exists(env.Dir('.').srcnode().Dir('doc').File('Doxyfile').abspath):
            source += [env.Dir('.').Dir('doc').File('Doxyfile')]
        return (target, source)

    builder = env.Builder(action = env.Action(createDoxygen, varlist=['DOXYGENOUTPUT']), emitter = createDoxygenEmitter, source_factory = SCons.Node.FS.Dir, suffix = '.config', ensure_suffix = 1)
    env.Append(BUILDERS = {'CreateDoxygen' : builder })

def exists(env):
    return 1
