import os
import os.path
import pyclbr
import random
import re
import string
import threading
import time

# Howie-specific
import aiml
import configFile
import frontends
from frontends import *

class ActiveFrontEnd:
	def __init__(self, inst, thread):
		self._inst = inst
		self._thread = thread

_frontends = {}
kernel = None
def _addFrontEnd(name, cls):
	global _frontends
	
	# verbose output
	config = configFile.get()
	if config['cla.verboseMode'] == "yes":
		print "Creating %s front-end using class %s" % (name, cls)
	
	# Instantiate the frontend object
	feInst = eval("%s.%s()" % (name, cls))
	
	# Create a thread to run this frontend				
	feThread = threading.Thread(name=name, target=feInst.go)
	feThread.start()
	_frontends[name] = ActiveFrontEnd(feInst, feThread)


def init():
	global kernel
	"Initialize the front-ends and back-ends."
	# Fetch the configuration info
	config = configFile.get()
	
	# Initialize the AIML interpreter	
	kernel = aiml.Kernel()
	kernel.verbose(config["general.verbose"] == "yes" or config["cla.verboseMode"] == "yes")
	kernel.setPredicate("secure", "yes") # secure the global session
	kernel.bootstrap(learnFiles="std-startup.xml", commands="bootstrap")
	kernel.setPredicate("secure", "no") # and unsecure it.
	kernel.setBotName(config["general.botname"])
	
	# Handle local mode: only start the tty frontend
	if config['cla.localMode'] == "yes":
		_addFrontEnd("tty", "FrontEndTTY")
	else:
		# Initialize the front-ends.  Pythonic black magic ensues...
		# First we iterate over all frontend modules.
		for fe in frontends.__all__:
			# If this frontend isn't activated in the configuration file,
			# ignore it.
			try: isActive = (config["%s.active" % fe] == "yes")
			except KeyError:
				print "WARNING: no 'active' entry found for module %s in configuration file." % fe
				isActive = False
			if not isActive:
				if config['cla.verboseMode'] == 'yes':
					print "Skipping inactive frontend: %s" % fe
				continue

			# Attempt to extract the name of the front-end class defined in this module.
			# If no such class is defined, or if the class is not a subclass of IFrontEnd,
			# skip this module.
			try:
				cls = eval("frontends.%s.frontEndClass" % fe)
				if issubclass(eval("frontends.%s.%s" % (fe, cls)), frontends.frontend.IFrontEnd):
					# Create an instance of this class in the _frontends dictionary
					_addFrontEnd(fe, cls)
					break
			except:
				# no class defined in this file.
				continue

def submit(input, session):
	"Submits a statement to the back-end. Returns the response to the statement."
	response = kernel.respond(input, session)
	# if logging is enabled, write the input and response to the log.
	try:
		config = configFile.get()
		if config["general.logging"] in ['yes', 'y', 'true']:
			logdir = config["general.logdir"]
			if not os.path.isdir(logdir): os.mkdir(logdir)
			logfile = file("%s/%s.log" % (logdir, session), "a")
			logfile.write(time.strftime("[%m/%d %H:%M:%S]\n"))
			logfile.write("%s: %s\n" % (session, input))
			logfile.write("%s: %s\n" % (kernel.getBotName(), response))
			logfile.close()			
	except:
		pass
	return response