#!/usr/bin/python

"""
HOWIE
    Description: Magical conversational/sysadmin AIM bot.
    Author: Cort Stratton (cort@andrew.cmu.edu)
    Version: 0.1

USAGE:
    howie [OPTIONS]

OPTIONS:
    -h,--help                        Prints this message.
    -u,--username=<username>         AIM username.
    -p,--password=<password>         AIM password.

"""

import jalice
import toc

import getopt
import getpass
import os
import random
import re
import string
import sys
import time
import urllib

# Legal values for sSearchType:
# - who
# - what
# - where
# - when
def get_googlisms(sInput, sSearchType = "what"):
	"Retrieve a list of Googlisms for the string sInput."
	nType = 2 # a "what" search
	if string.lower(sSearchType) == "who":
		nType = 1
	elif string.lower(sSearchType) == "what":
		nType = 2
	elif string.lower(sSearchType) == "where":
		nType = 3
	elif string.lower(sSearchType) == "when":
		nType = 4
	else:
		print "ERROR: invalid search type.  must be 'who', 'what', 'where' or 'when'."
		return []

	# Fetch the results.
	url = "http://www.googlism.com/index.htm?ism=" + \
	        string.replace(urllib.quote(sInput.strip()), " ", "+") + \
		  "&type=" + str(nType)
	result_page = urllib.urlopen(url)
	lines = result_page.readlines()

	# Extract the useful results
	results = []
	pattern = string.lower(sInput.strip()) + " is (?P<def>.*)\<br\>"
	googlism_regex = re.compile(pattern)
	for line in lines:
		match = googlism_regex.search(line)
		if match is not None:
			results.append(match.group('def'))
	return results

class HowieAIM(toc.TocTalk):
	def __init__(self, username, password):
		toc.TocTalk.__init__(self, username, password)
		self._info = "I am Cort Stratton's web server.  F3ar my wrath!"

		# Set this bot's admin.
		self._botmaster = "kujosama"
		

		# Initialize command regexps
		self._addCmd = re.compile("^add (?P<newBuddy>.+)$", re.I)
		self._removeCmd = re.compile("^remove (?P<oldBuddy>.+)$", re.I)
		self._listBuddiesCmd = re.compile("^list buddies$", re.I)
		self._howBusyCmd = re.compile("are you busy", re.I)
		self._whoIsCmd   = re.compile("who (?P<verb>is|are|was|were) (?P<query>[^?]+)", re.I)
		self._whatIsCmd  = re.compile("what (?P<verb>is|are|was|were) (?P<query>[^?]+)", re.I)
		self._whereIsCmd = re.compile("where (?P<verb>is|are|was|were) (?P<query>[^?]+)", re.I)
		self._whenIsCmd  = re.compile("when (?P<verb>is|are|was|were) (?P<query>[^?]+)", re.I)
	
	def start(self):
		"Do stuff here immediately after the bot connects."
		# Placeholder.
		pass

	def _updateConfig(self):
		"Saves our current configuration (buddy list, etc.) back to the server."
		# Create a TOC-style CONFIG string
		newConfig = "m 1\ng Buddies\n"
		for buddy in self._buddyList:
			newConfig += "b " + buddy + "\n"
		self.do_SET_CONFIG(newConfig)

	def _addBuddy(self, newBuddy):
		"Add a new buddy, both locally and remotely."
		if self._buddyList.count(newBuddy) > 0:
			# Buddy already in list.
			return
		self.do_ADD_BUDDY(list(newBuddy))
		self._buddyList.append(newBuddy)
		self._updateConfig()

	def _removeBuddy(self, oldBuddy):
		"Remove an existing buddy from the buddy list."
		if self._buddyList.count(oldBuddy) == 0:
			# No such buddy.
			return
		self.do_REMOVE_BUDDY(list(oldBuddy))
		self._buddyList.remove(oldBuddy)
		self._updateConfig()

	def _howBusy(self):
		outfd = os.popen("/usr/bin/uptime")
		lines = outfd.readlines()
		outfd.close()
		print "output:", lines[0]
		result = re.search("load average\: (?P<one>[\d\.]+)", lines[0])
		if result is not None:
			load = float(result.group("one"))
			print "Load is", load
			if load < 0.25:
				return "I'm not very busy right now."
			elif load < 0.5:
				return "I'm kind of busy right now."
			else:
				return "I'm very busy right now!"
		return "ERROR!"

	def _whoIs(self, user, match):
		query = match.group('query')
		googlisms = get_googlisms(query, "who")
		if len(googlisms) == 0:
			return jalice.respond("Who is " + query + "?", user)
		return query + " " + match.group('verb')+ " " + random.choice(googlisms)

	def _whatIs(self, user, match):
		query = match.group('query')
		googlisms = get_googlisms(query, "what")
		if len(googlisms) == 0:
			return jalice.respond("What is " + query + "?", user)
		return query + " " + match.group('verb')+ " " + random.choice(googlisms)
	
	def _whereIs(self, user, match):
		query = match.group('query')
		googlisms = get_googlisms(query, "where")
		if len(googlisms) == 0:
			return jalice.respond("Where is " + query + "?", user)
		return query + " " + match.group('verb')+ " " + random.choice(googlisms)

	def _whenIs(self, user, match):
		query = match.group('query')
		googlisms = get_googlisms(query, "when")
		if len(googlisms) == 0:
			return jalice.respond("When is " + query + "?", user)
		return query + " " + match.group('verb')+ " " + random.choice(googlisms)

	# Process a command string: parse an incoming message,
	# and dispatch to a handler if necessary.  Returns True if a command
	# was processed, False if not.
	# user is the screenname of the user who issued the command.
	def _parseCommand(self, user, cmd):
		# If the user isn't in our buddy list, they're not allowed
		# to issue any commands!
		
		# add buddy
		result = self._addCmd.search(cmd)
		if result is not None and user == self._botmaster:
			newBuddy = result.group('newBuddy')
			self._addBuddy(newBuddy)
			return True

		# remove buddy
		result = self._removeCmd.search(cmd)
		if result is not None and user == self._botmaster:
			oldBuddy = result.group('oldBuddy')
			self._removeBuddy(oldBuddy)
			return True

		# list buddies
		result = self._listBuddiesCmd.search(cmd)
		if result is not None:
			response = "My buddies are:"
			for bud in self._buddyList:
				response += " " + bud + ","
			self.do_SEND_IM(user, response[:-1])
			return True

		# Are you busy?
		result = self._howBusyCmd.search(cmd)
		if result is not None:
			self.do_SEND_IM(user, self._howBusy())
			return True

		# Googlisms
		result = self._whoIsCmd.search(cmd)
		if result is not None:
			self.do_SEND_IM(user, self._whoIs(user, result))
			return True
		result = self._whatIsCmd.search(cmd)
		if result is not None:
			self.do_SEND_IM(user, self._whatIs(user, result))
			return True
		result = self._whereIsCmd.search(cmd)
		if result is not None:
			self.do_SEND_IM(user, self._whereIs(user, result))
			return True
		result = self._whenIsCmd.search(cmd)
		if result is not None:
			self.do_SEND_IM(user, self._whenIs(user, result))
			return True
		
		# Didn't process a command.
		return False

	
	# Called whenever someone IMs us.
	# data contains "screenname:flag:message", such as
	#   "jamwt:F:hey, ben.. how's work?".
	# flag is "T" if the message was sent as an auto-response, "F"
	# otherwise.
	def on_IM_IN(self, data):
		# we must use the optional maxsplit argument of 2 in case the
		# message contains a colon!
		data_components = data.split(":",2)

		# Store screen name.
		screenname = data_components[0]  # the sender's screenname

		# Store whether the message is an auto-response.
		bAutoResponse = (data_components[1] == "T")

		# Some clients send messages as HTML.  We don't give a damn.
		message = self.strip_html(data_components[2])

		# Attempt to parse as a command.
		if self._parseCommand(screenname, message):
			return
		
		# Send a reply, if we didn't process a command.  Insert
		# a random delay of 0-4 seconds before replying.
		if not bAutoResponse:
			time.sleep( random.random() * 4 )
			self.do_SEND_IM(screenname, jalice.respond(message, screenname))

		
	# Called when there's an incoming user configuration (our
	# buddy list).  Can be empty.  Format of data : The config
	# information is a string containing a series of lines with
	# the first character in each being the item type, followed by
	# a space, with the rest of the line being the item
	# value. Only letters, numbers, and spaces should be used.
	def on_CONFIG(self, data):
		# first time logging in--add buddies from config...
		self._buddyList = []
		budsToAdd = []
		
		# remember the format of config data here:
		# "m 1\ng Buddies\nb bouncebot\nb perlaim\n"
		for item in data.split("\n"):
			if item == '':
				continue
			if item[0] == "b":
				budsToAdd.append(item[1:].strip())
				
				
			#add no more than ~20 at a time, msg len restrictions
			if len(budsToAdd) == 20:
				self.do_ADD_BUDDY(budsToAdd)
				self._buddyList.extend(budsToAdd)
				time.sleep(0.2) # don't SLAM the server...
				budsToAdd = []

		# Clean up any leftovers.
		if len(budsToAdd):
			self.do_ADD_BUDDY(budsToAdd)
			self._buddyList.extend(budsToAdd)
			budsToAdd = []

		# Make sure our botmaster is a buddy!
		if self._buddyList.count(self._botmaster) == 0:
			self._addBuddy(self._botmaster)

		print "Buddies:\n", self._buddyList
					

	# This command handles arrivals, departures, and updates of
	# members on your bots buddy list.
	#
	# Format of data : Colon delimited; first is screenname, then
	# T/F for online status, warning percentage, time of sign-on
	# in Unix epoc format, idle time in minutes, and finally a
	# "User Class" string of 2 or three characters.
	def on_UPDATE_BUDDY(self, data):
		data_components = data.split(":", 5)

	# Some random events I don't understand yet...
	def on_NOID(self, data):
		pass
	def on_NICK(self, data):
		pass

def usage():
	"Prints a usage message."
	print __doc__


def main():
	random.seed()
	bHaveUsername = False
	bHavePassword = False
	# Process command-line options
	try:
		opts, args = getopt.getopt(sys.argv[1:], "hu:p:", ["help", "username=", "password="])
	except getopt.GetoptError:
		# Print help information and exit
		usage()
		sys.exit(2)
	for opt, arg in opts:
	       	if opt in ["-h", "--help"]:
		       	usage()
			sys.exit(0)
		elif opt in ["-u", "--username"]:
		       	aim_username = arg
			bHaveUsername = True
		elif opt in ["-p", "--password"]:
		       	aim_password = arg
			bHavePassword = True

	# Prompt for username and password, if they weren't passed as args.
	if not bHaveUsername:
		aim_username = raw_input("AIM username: ")
	if not bHavePassword:
		aim_password = getpass.getpass("Password: ")

	# Bootstrap the AI.  We'll need to change into the config directory
	# first.
	jalice.bootstrap()

	# Create the bot and go!
	try:
		aimbot = HowieAIM(aim_username, aim_password)
		aimbot.go()
	except toc.TOCError, (strerror):
		print strerror
		sys.exit(-1)

# if this file is run directly, call main.
if __name__ == "__main__":
	main()
