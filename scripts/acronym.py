#!/usr/bin/python
"""
Acronym Decoder v1.0

usage: acronym.py [-max N] [-f] A.C.R.O.N.Y.M.

Prints the known expansions of the specified acronym.  If the optional -max
parameter is specified, at most N matches will be output.  If -f is passed,
the output is printed in "friendly" mode; otherwise, the matches will be output
one per line.
"""

def usage(): print __doc__

import getopt
import os
import re
import string
import sys
import urllib

### Compiled regular expressions

# Removes all unwanted characters from an acronym.
acroFormatRE = re.compile(r'[^A-Z\-]')
# matches the line BEFORE a definition
firstLineRE = re.compile(r'<td valign="middle" width="15%".*><b>(?P<acro>[A-Z\-]+)</b></td>')
# matches the definition of an acronym, excluding any paranthetical elaborations
secondLineRE = re.compile(r'<td valign="middle" width="75%".*>(<b>)?(?P<def>[^<(]+)(\([^<]+\))?(</b>)?$')
# matches the string indicating that no definitions were found
failureRE = re.compile(r'Sorry, <b>[A-Z\-]+</b> was not found in the database')

def formatAcro(acro):
    return acroFormatRE.sub("", acro.upper())
    
def parse(f, acro, max=-1):
    defOnNextLine = False
    linenum = 0
    found = []
    for line in f.readlines():
        # If we've found the maximum number of matches,
        # stop now.
        if max >= 0 and len(found) >= max:
            break
        
        # if we haven't found anything yet, check for failure
        if len(found) == 0 and not defOnNextLine:
            match = failureRE.search(line)
            if match is not None:
                break

        # check this line against the appropriate RE.
        # If the first line has already matched, look for an actual
        # definition on the second line.
        line = line.strip()
        linenum += 1        
        if defOnNextLine:
            defOnNextLine = False
            match = secondLineRE.search(line)
            if match is None:
                # This is bad; there should be a definition here...
                print "WARNING: did not find expected definition on line", linenum
                continue
            # add this def to the found list.
            found.append( match.group("def").strip() )
        else:
            match = firstLineRE.search(line)
            if match is not None and match.group("acro") == acro:
                defOnNextLine = True
    return found

if __name__ == "__main__":
    # process command-line args
    try:
        opts, args = getopt.getopt(sys.argv[1:], "m:f", ["max"])
    except getopt.GetoptError:
        usage()
        sys.exit(2)
    maxMatches = -1
    friendlyMode = False
    for o,a in opts:
        if o in ["-m", "-max"]:
            maxMatches = int(a)
        elif o == "-f":
            friendlyMode = True
    if len(args) != 1:
        usage()
        sys.exit(2)
    
    # format the acronym to search for
    acro = formatAcro(args[0])

    # Submit the query and open a file handle to the results page.
    class AppURLopener(urllib.FancyURLopener):
         def __init__(self, *args):
             self.version = "Mozilla/4.0"
             urllib.FancyURLopener.__init__(self, *args)
    urllib._urlopener = AppURLopener()
    f = urllib.urlopen("http://www.acro" + "nymfind"+"er.com/af-q"+"uery.asp?Str"+"ing=exact&A"+"cronym=%s" % acro)

    # Parse the results page to find a list of definitions. The list
    # will be empty if none were found.
    defs = parse(f, acro, maxMatches)

    # Print the definitions.    
    if len(defs) == 0:
        if friendlyMode: print "I don't know what %s stands for." % acro
    else:
        if friendlyMode:
            print acro, "stands for:",
            for d in defs[:-1]:
                print d + ",",
            print defs[-1] + "."
        else:
            for d in defs: print d
