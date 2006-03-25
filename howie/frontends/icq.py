import getpass
import random
import time
from twisted.internet import selectreactor
#default.install()
from twisted.words.protocols import oscar
from twisted.internet import protocol, reactor

import howie.configFile
import frontend
import oscarlistener

# This string needs to be defined for each front end.  It should
# contain the name of the front-end class defined in this module.
frontEndClass = "FrontEndICQ"

# default config file values for this module
configDefaults = {
	"icq.active":			"no",
	"icq.screenname":		"",
	"icq.password":			"",
	"icq.profile":			"Your profile goes here."
	}

class FrontEndICQ(frontend.IFrontEnd):
	def __init__(self):
		config = howie.configFile.get()
		try:
			screenname = config['icq.screenname']
			if screenname == "": raise KeyError
		except KeyError: screenname = raw_input("ICQ screenname: ")
		try:
			password = config['icq.password']
			if password == "": raise KeyError
		except KeyError: password = getpass.getpass("ICQ password for %s: " % screenname)

		hostport = ('login.icq.com', 5238)
		#hostport = ('login.oscar.aol.com', 5190)

		class OAuth(oscar.OscarAuthenticator):
			BOSClass = oscarlistener.OscarListener
		client = protocol.ClientCreator(reactor, OAuth, screenname, password, icq=1)
		client.connectTCP(*hostport)
				
	def go(self):
		# Serve forever
		reactor.run(installSignalHandlers=0)

	def display(self, output, user):
		"Sends output as an IM to the specified user."
		#self.do_SEND_IM(user, output)
		pass

# if this file is run directly...
if __name__ == "__main__":
	pass