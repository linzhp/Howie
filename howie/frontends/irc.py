import howie.configFile
import frontend

import string
import time
from ircbot import SingleServerIRCBot
from irclib import nm_to_n, irc_lower  

# This string needs to be defined for each front end.  It should
# contain the name of the front-end class defined in this module.
frontEndClass = "FrontEndIRC"

# default config file values for this module
configDefaults = {
	"irc.active":		"no",
	"irc.server":		"",
	"irc.port":			"6667",
	"irc.nick":			"",
	"irc.channel":		"",
	}

class FrontEndIRC(SingleServerIRCBot, frontend.IFrontEnd):
	def __init__(self):
		config = howie.configFile.get()
		try:
			server = config['irc.server']
			if server == "": raise KeyError
		except KeyError: server = raw_input("Server: ")

		try:
			port = int(config['irc.port'])
		except: port = raw_input("Port: ")

		try:
			nickname = config['irc.nick']
			if nickname == "": raise KeyError
		except KeyError: nickname = raw_input("Nick: ")

		try:
			channel = config['irc.channel']
			if channel == "": raise KeyError
		except KeyError: channel = raw_input("Channel: ")

		SingleServerIRCBot.__init__(self, [(server, port)], nickname, nickname)
		self.channel = channel
		try: self._maxdelay = int(config['general.maxdelay'])
		except KeyError: self._maxdelay = 0
		self.start()

	def on_nicknameinuse(self, c, e):
		c.nick(c.get_nickname() + "_")

	def on_welcome(self, c, e):
		c.join(self.channel)

	def on_privmsg(self, c, e):
		self.do_message(e, e.arguments()[0],1)

	def on_pubmsg(self, c, e):
		a = string.split(e.arguments()[0], ":", 1)
		if len(a) > 1 and irc_lower(a[0]) == irc_lower(self.connection.get_nickname()):
			self.do_message(e, string.strip(a[1]))
		return

	def display(self,message,nick,priv=0):
		c = self.connection
		for line in message.split("\n"):
			line = line.strip()
			if line == "":
				continue
			time.sleep( self._maxdelay 
					  * (max(len(line) / 40,1.0)) ) # weighed by "typing time"
			if priv:
				c.privmsg(nick,line)
			else:
				c.privmsg(self.channel, "%s: %s" % (nick,line))

	def do_message(self, e, message,priv=0):
		nick = nm_to_n(e.source())

		response = self.submit(message, nick+"@IRC")
		self.display(response, nick,priv)


if __name__ == "__main__":
	pass
