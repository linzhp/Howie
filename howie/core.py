import os
import pyclbr
import random
import re
import string
import threading

# Howie-specific
import aiml
import configFile
import frontends
from frontends import *

class ActiveFrontEnd:
	def __init__(self, inst, thread):
		self.__inst = inst
		self.__thread = thread

__frontends = {}
__kernel = None
def __addFrontEnd(name, cls):
	global __frontends
	
	# verbose output
	config = configFile.get()
	if config['cla.verboseMode'] == "yes":
		print "Creating %s front-end using class %s" % (name, cls)
	
	# Instantiate the frontend object
	feInst = eval("%s.%s()" % (name, cls))
	
	# Create a thread to run this frontend				
	feThread = threading.Thread(name=name, target=feInst.go)
	feThread.start()
	__frontends[name] = ActiveFrontEnd(feInst, feThread)


def init():
	global __kernel
	"Initialize the front-ends and back-ends."
	# Fetch the configuration info
	config = configFile.get()
	
	# Initialize the AIML interpreter	
	__kernel = aiml.Kernel()
	__kernel.bootstrap(learnFiles="std-startup.xml", commands="bootstrap")
	
	# Handle local mode: only start the tty frontend
	if config['cla.localMode'] == "yes":
		__addFrontEnd("tty", "FrontEndTTY")
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
					# Create an instance of this class in the __frontends dictionary
					__addFrontEnd(fe, cls)
					break
			except:
				# no class defined in this file.
				continue

def submit(input, user):
	"Submits a statement to the back-end. Returns the response to the statement."
	
	return __kernel.respond(input, user)