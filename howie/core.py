import marshal
import os.path
import sys
import threading
import time
import traceback

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
	if config['cla.verboseMode'] in ["yes", "y", "true"]:
		print "Creating %s front-end using class %s" % (name, cls)
	
	# Instantiate the frontend object
	feInst = eval("%s.%s()" % (name, cls))
	
	# Create a thread to run this frontend
	feThread = threading.Thread(name=name, target=feInst.go)
	feThread.setDaemon(True)
	feThread.start()
	_frontends[name] = ActiveFrontEnd(feInst, feThread)


def init():
	global kernel
	"Initialize the front-ends and back-ends."
	# Fetch the configuration info
	config = configFile.get()
	
	# Initialize the AIML interpreter
	print "Initializing AIML interpreter (please be patient)..."
	kernel = aiml.Kernel()
	#extract config options
	try: verbose = config["general.verbose"] == "yes" or config["cla.verboseMode"] == "yes"
	except: verbose = False
	try: botName = config["general.botname"]
	except: botName = "Nameless"
	try: botMaster = config["general.botmaster"]
	except: botMaster = "The Master"
	try: sessionsPersist = config["general.sessionspersist"].lower() in ["yes", "y", "true"]
	except: sessionsPersist = False
	try: sessionsDir = config["general.sessionsdir"]
	except: sessionsDir = "sessions"
	
	# set up the kernel
	kernel.verbose(verbose)
	kernel.setPredicate("secure", "yes") # secure the global session
	kernel.bootstrap(learnFiles="std-startup.xml", commands="bootstrap")
	kernel.setPredicate("secure", "no") # and unsecure it.

	# Initialize bot predicates
	for k,v in config.items():
		if k[:8] != "botinfo.":
			continue
		kernel.setBotPredicate(k[8:], v)

	# Load persistent session data, if necessary
	if sessionsPersist:
		try:
			for session in os.listdir(sessionsDir):
				# Session files are named "user@protocol.ses", where
				# user@protocol is also the internal name of the session.
				root, ext = os.path.splitext(session)
				if ext != ".ses":
					# This isn't a session file.
					continue
				# Load the contents of the session file (a single dictionary
				# containing all the predicates for this session).
				if verbose: print "Loading session:", root
				f = file("%s/%s" %(sessionsDir, session), "rb")
				d = marshal.load(f)
				f.close()
				# update the predicate values in the Kernel.
				for k,v in d.items():
					kernel.setPredicate(k,v,root)
		except OSError:
			print "WARNING: Error loading session data from", sessionsDir
	
	# Handle local mode: only start the tty frontend
	if config['cla.localMode'].lower() in ["yes", "y", "true"]:
		try: _addFrontEnd("tty", "FrontEndTTY")
		except:
			print "ERROR initializing frontend class frontends.tty.FrontEndTTY"
			traceback.print_tb(sys.exc_info()[2])
	else:
		# Initialize the front-ends.  Pythonic black magic ensues...
		# First we iterate over all frontend modules.
		for fe in frontends.__all__:
			# If this frontend isn't activated in the configuration file,
			# ignore it.
			try: isActive = (config["%s.active" % fe].lower() in ["yes", "y", "true"])
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
				if not issubclass(eval("frontends.%s.%s" % (fe, cls)), frontends.frontend.IFrontEnd):
					continue
			except AttributeError:
				# no valid front-end class defined in this file.
				print "WARNING: could not find valid front-end class in module %s" % fe
				continue

			# Create an instance of this class in the _frontends dictionary
			try: _addFrontEnd(fe, cls)
			except:
				# raise # uncomment for details on error
				print "ERROR initializing frontend class frontends.%s.%s" % (fe,cls)
				traceback.print_tb(sys.exc_info()[2])
				continue
			
def submit(input, session):
	"Submits a statement to the back-end. Returns the response to the statement."
	response = kernel.respond(input, session)

	config = configFile.get()	
	# if logging is enabled, write the input and response to the log.
	try:
		if config["general.logging"].lower() in ["yes", "y", "true"]:
			logdir = config["general.logdir"]
			if not os.path.isdir(logdir): os.mkdir(logdir)
			logfile = file("%s/%s.log" % (logdir, session), "a")
			logfile.write(time.strftime("[%m/%d/%Y %H:%M:%S]\n"))
			logfile.write("%s: %s\n" % (session, input))
			logfile.write("%s: %s\n" % (kernel.getBotPredicate("name"), response))
			logfile.close()			
	except KeyError:
		pass

	# If persistent sessions are enabled, store the session data.
	try:
		if config["general.sessionspersist"].lower() in ["yes", "y", "true"]:
			sessionsdir = config["general.sessionsdir"]
			if not os.path.isdir(sessionsdir): os.mkdir(sessionsdir)
			sessionfile = file("%s/%s.ses" % (sessionsdir, session), "wb")
			marshal.dump(kernel.getSessionData(session), sessionfile)
			sessionfile.close()
	except KeyError:
		pass
	return response
