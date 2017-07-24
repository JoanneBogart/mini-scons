import os, os.path
import SCons.Action
import SCons.Builder
from   fermidebug import fdebug



def generate(env):
    # Write a file in $INST_DIR/data directory listing packages
    # under $INST_DIR
    def createPkgList(target = None, source = None, env = None):
        ###if env == None or target == None: return 1

        fp = open(str(target[0]), 'w')
        fp.write("# packages belonging to this file hierarchy\n\n")
        for pkg in env['packageNameList']:
            print("writePkgList: Processing pkg " + pkg)
            fp.write(os.path.basename(str(pkg)))
            fp.write("\n")
        fp.close()
        return 0


    def createPkgListGenerator(source, target, env, for_signature):
        actions = [env.Action(createPkgList, "Creating pkg list")]

        return actions

    suf = 'pkgList'

    env.Append(BUILDERS={'GeneratePkgList' : env.Builder(generator=createPkgListGenerator,
                                                         suffix=suf)})

def exists(env):
    return 1;

