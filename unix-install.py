"""
This script automates some UNIX-specific installation steps.  It looks
for scripts in the "scripts" directory, and does whatever is necessary to
make them executable (e.g. setting the executable bit, adding a bang-path to
the first line of the script, etc.)
"""

import glob
import os
import os.path
import string
import sys

if __name__ == "__main__":
    # This is the directory to search for scripts
    scriptsDir = "scripts/"

    # This dictionary maps script file extensions to a tuple
    # containing the name of the scripting language, the name of the
    # interpreter executable for that language, and the path to the
    # interpreter (initially None -- this is specified by the user if
    # it can't be found automatically)
    scriptTypes = {
        ".py":      ("Python", "python", None),
        ".pl":      ("Perl", "perl", None),
        ".lua":     ("Lua", "lua", None),
    }

    
    for f in os.listdir(scriptsDir):
        root, ext = os.path.splitext(f)
        if not scriptTypes.has_key(ext):
            # This is not a recognized script file
            continue
        name,exe,exepath = scriptTypes[ext]
        if exepath is None:
            # We haven't found an executable for this language yet.
            # Use 'which' to try and find a reasonable default, and
            # then prompt the user to confirm.
            exepath = ""
            for line in os.popen("which %s" % exe): exepath += line.strip()
            userpath = raw_input("Path to %s interpreter [%s]: " % (name, exepath))
            if len(userpath.strip()) > 0: exepath = userpath
            scriptTypes[ext] = (name,exe,exepath)