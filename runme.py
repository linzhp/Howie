#!/usr/bin/python

"""
HOWIE
Description: Magical conversational/sysadmin bot.
Author: Cort Stratton (cort@andrew.cmu.edu)

Usage: runme.py [OPTIONS]

Valid Options:
    -h,--help:               Prints this help message.
    -v,--verbose:            Enables verbose mode (extra debug output)
    -l,--local:              Enabled "local-only" mode; only the TTY interface
                             to Howie is initialized.  Useful for having quick
		             conversations with Howie without putting him
			     online.
    -f,--config-file=FILE    Specifies an alternate location for Howie's
                             configuration file (default is "howie.ini")
"""

import getopt
#from optparse import Option, OptionParser
import os
import random
import string
import sys
import time

# Add the current directory to the python search path
sys.path.append(os.getcwd())

import howie.configFile
import howie.core	

def usage():
	"Prints a usage message."
	print __doc__

def main():
	random.seed()

	# Process command-line arguments
	try:
		opts,args = getopt.getopt(sys.argv[1:], "hlf:v",
					  ["help","local","config-file","verbose"])
	except getopt.GetoptError:
		usage()
		sys.exit(2)
	localMode = "no"
	configFile = "howie.ini"
	verboseMode = "no"
	for o,a in opts:
		if o in ["-h","--help"]:
			usage()
			sys.exit(2)
		elif o in ["-v","--verbose"]:
			verboseMode = "yes"
		elif o in ["-f","--config-file"]:
			configFile = a
		elif o in ["-l","--local"]:
			localMode = "yes"
	
	### The following method uses the optparse module, which is
	### only available in Python 2.3 or later.  Let's replace it
	### with something more backwards-compatible...
	
	#usageString = "usage: %prog [options]"
	#versionString = "howie snapshot-20040109"
	#parser = OptionParser(usage=usageString, version=versionString)
	#parser.add_option("-l", "--local", action="store_const", const="yes", dest="localMode",
	#				  help="Local mode (only open the TTY interface)", default="no")
	#parser.add_option("-f", "--config-file", action="store", type="string",
	#				  dest="configFile", default="howie.ini", metavar="FILE",
	#				  help="Use FILE instead of the default config file")
	#parser.add_option("-v", "--verbose", action="store_const", const="yes",
	#				  dest="verboseMode",  help="Enables verbose output", default="no")
	#(options, args) = parser.parse_args()
	#if len(args) != 0:
	#	parser.error("incorrect number of arguments")

	# Read config file
	config = howie.configFile.load(configFile)

	# Add config data from command-line arguments to the "cla" group.
	# TODO: find a clean way to iterate over the destination variables in
	# options, so the following code can be used instead:
	## for k,v in options.dict:
	##	config["cla.%s" % k] = str(v)
	for k in ["localMode", "configFile", "verboseMode"]:
		config["cla.%s" % k] = str( eval("%s" % k) )

	# Bootstrap the AI.
	howie.core.init()

	# Initialize the front-ends.  Each one runs in its own thread.
##	if not options.bLocalMode and config['aim.active'] == 'yes':
##		aimbot = HowieAIM.factory(config['aim.screenname'],
##								  config['aim.password'])
##		if aimbot is not None:
##			aimThread = threading.Thread(name="aim", target=aimbot.go)
##			aimThread.start()
##
##	if options.bLocalMode or config['tty.active'] == 'yes':
##		ttybot = HowieTTY.factory()
##		if ttybot is not None:
##			ttyThread = threading.Thread(name="tty", target=ttybot.go)
##			ttyThread.start()

# if this file is run directly, call main.
if __name__ == "__main__":
	main()
