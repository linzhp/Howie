"""
This script automates some UNIX-specific installation steps.  It looks
for scripts in the "scripts" directory, and does whatever is necessary to
make them executable (e.g. setting the executable bit, adding a bang-path to
the first line of the script, etc.)
"""

import os
import os.path
import sys

# This is the directory to search for scripts
_scriptsDir = "scripts/"

# This dictionary maps script file extensions to a tuple
# containing the name of the scripting language, the name of the
# interpreter executable for that language, and the path to the
# interpreter (initially None -- this is specified by the user if
# it can't be found automatically)
_scriptTypes = {
    ".py":      ("Python", "python", None),
    ".pl":      ("Perl", "perl", None),
    ".lua":     ("Lua", "lua", None),
}


def makeExecutable(path):
    root, ext = os.path.splitext(path)
    if not _scriptTypes.has_key(ext):
        # This is not a recognized script file
        return
    name,exe,exepath = _scriptTypes[ext]
    if exepath is None:
        # We haven't found an executable for this language yet.
        # Use 'which' to try and find a reasonable default, and
        # then prompt the user to confirm.
        exepath = ""
        for line in os.popen("which %s" % exe): exepath += line.strip()
        userpath = raw_input("Path to %s interpreter [%s]: " % (name, exepath))
        if len(userpath.strip()) > 0: exepath = userpath
        _scriptTypes[ext] = (name,exe,exepath)

    print "Processing %s..." % path

    # Add a "bang-path" to the top of the script file.
    newlines = ["#!%s\n" % exepath]
    linenum = 0
    for line in file(path):
        linenum += 1
        if linenum == 1 and line[:2] == "#!":
            # Already has a bangpath; don't add it to newlines
            continue
        newlines.append(line)
    outfile = file(path, "w")
    outfile.writelines(newlines)
    outfile.close()

    # Set the executable bit for the script file
    os.chmod(path, 0755)


if __name__ == "__main__":
    # Make the main Howie script executable.
    makeExecutable("runme.py")
    
    # And all the scripts in _scriptsDir
    for f in os.listdir(_scriptsDir):
        path = _scriptsDir + f
        makeExecutable(path)
    
