import os,platform,os.path

#  Usual case:  find where package is; add to env include path
#  If 'subdir' argument, instead set construction env variable
#  to point to it
def generate(env, **kw):
    pkgName = kw.get('package', '')
    if pkgName == '': 
        print 'findPkgPath called with no arg'
        return None
    path = None
    subDir = kw.get('subDir', '')
    usualCase = (subDir == '')

    # paths all start with .\ so strip it off
    #if forStatic.first == True: 
    #    print 'findPkgPath called with argument ', pkgName
    ##  First look in env['packageNameList'].  If we *have* supersede,
    ##  this is the supersede list.  Otherwise it's the base list
    for p in env['packageNameList']:
        bname = os.path.basename(str(p[2:]))
        if pkgName == bname: 
            if env.GetOption('supersede') == '.':
                path = '#' + str(p[2:])
            else: 
                path = os.path.join(str(env['absSuperPath']), pkgName)
        if pkgName + '-' == bname[:len(bname)+1]:
            if env.GetOption('supersede') == '.': 
                    path = '#' + str(p[2:])       
            else: path = os.path.join(str(env['absSuperPath']), str(p[2:]))

    # If not found up to this point, we must be in case
    #    -have supersede dir
    #    -package we have been called for is not in supersede area
    if path == None and env.GetOption('supersede') != '.': # look in base
        for p in env['basePackageNameList']:
            bname = os.path.basename(str(p[2:]))
            ##if pkgName == bname: path = '#' + str(p[2:])
            if pkgName == bname: 
                path = os.path.join(env['absBasePath'], str(p[2:]))
            if pkgName + '-' == bname[:len(bname)+1]: 
                path = os.path.join(env['absBasePath'], str(p[2:]))
    if path != None:
        if usualCase:
            env.AppendUnique(CPPPATH = [path])
        else:
            conVarName = pkgName + '_' + subDir
            env[conVarName] = os.path.join(path, subDir)
def exists(env):
    return 1
