import os
import pyclbr
import random
import re
import string
import threading

# Howie-specific
import configFile
import frontends
from frontends import *
import frontends.frontend
from backends import jalice
from backends import googlism

class ActiveFrontEnd:
	def __init__(self, inst, thread):
		self.__inst = inst
		self.__thread = thread

__frontends = {}
__backends = {}
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
	"Initialize the front-ends and back-ends."
	# Fetch the configuration info
	config = configFile.get()
	
	# Initialize the backends	
	jalice.bootstrap()
	
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
			
			# Extract defintions of all classes defined in this module.
			# We could alternately use the builtin function issubclass(C,B)
			# on all objects defined in the module, but this seems cleaner
			# (if perhaps slower).
			classes = pyclbr.readmodule("howie.frontends.%s" % fe)
			
			# Look for classes which are descended from IFrontEnd
			for cls, clsInfo in classes.items():
				if issubclass(eval("frontends.%s.%s" % (fe, cls)), frontend.IFrontEnd):
					# Create an instance of this class in the __frontends dictionary.
					__addFrontEnd(fe, cls)
					break


def submit(input, user):
	"Submits a statement to the back-end. Returns the response to the statement."
	# Googlisms
	beGoog = googlism.BackEndGooglism()
	result = beGoog.respondTo(input)
	if result is not None:
		return result
	
	# Fall-back: let jalice respond
	return jalice.respond(input, user)