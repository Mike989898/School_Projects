#
# This file is the default set of rules to compile a Pebble project.
#

import os.path

top = '.'
out = 'build'

def options(ctx):
    ctx.load('pebble_sdk')

def configure(ctx):
    """
    This method is used to configure your build.
    ctx.load(`pebble_sdk`) automatically configures a build for each valid platform in `targetPlatforms`.
    Platform-specific configuration: add your change after calling ctx.load('pebble_sdk') and make sure to set the
    correct environment first.
    Universal configuration: add your change prior to calling ctx.load('pebble_sdk').
    """
    ctx.load('pebble_sdk')

def copy_appinfo():
    """ CS 50 Hack!

    This method is used to copy appinfo.json to src/js/appinfo.js as a variable, 
    making it accessible to app.js -- the pebble app (field agent) and proxy 
    shares this one config file, making it easier for students to manage the 
    various components in the Mission Incomputable project.
    """
    with open('appinfo.json', 'r') as infile:
        data=infile.read()

        outfile = open("./src/js/appinfo.js","w")
        outfile.write("var config = ")
        outfile.write(data.rstrip())
        outfile.write(";")
        outfile.write("\n\nmodule.exports.config = config;")
        outfile.close()

def debug_files(ctx, show):
    if (show):
        print
        print '----- pebble srcs -----'
        print ctx.path.ant_glob('src/**/*.c')
        print '----- ../common srcs -----'
        print ctx.path.find_node('../').ant_glob('common/**/*.c')
        print '----- ../lib srcs (not supported yet - this is just debug) -----'
        print ctx.path.find_node('../').ant_glob('lib/**/*.c')
        print

def build(ctx):
    ctx.load('pebble_sdk')

    # CS50 Hack!
    copy_appinfo()

    build_worker = os.path.exists('worker_src')
    binaries = []

    for p in ctx.env.TARGET_PLATFORMS:
        ctx.set_env(ctx.all_envs[p])
        ctx.set_group(ctx.env.PLATFORM_NAME)
        app_elf = '{}/pebble-app.elf'.format(ctx.env.BUILD_DIR)

        # CS50 Hack! (rather, an extension)
        # -> https://forums.pebble.com/t/how-to-include-external-source-directories-in-a-pebble-build/11014/7 <<< including files outside of the pebble dir/
        debug_files(ctx, True)
        ctx.pbl_program(
            source=ctx.path.ant_glob('src/**/*.c') 
                + ctx.path.find_node('../').ant_glob('common/**/*.c'),
            target=app_elf)

        if build_worker:
            worker_elf = '{}/pebble-worker.elf'.format(ctx.env.BUILD_DIR)
            binaries.append({'platform': p, 'app_elf': app_elf, 'worker_elf': worker_elf})
            ctx.pbl_worker(source=ctx.path.ant_glob('worker_src/**/*.c'), target=worker_elf)
        else:
            binaries.append({'platform': p, 'app_elf': app_elf})

    ctx.set_group('bundle')
    ctx.pbl_bundle(binaries=binaries, js=ctx.path.ant_glob(['src/js/**/*.js', 'src/js/**/*.json']), js_entry_file='src/js/app.js')
