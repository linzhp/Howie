import getpass
import random
import time
import toc

import howie.configFile
import frontend

# This string needs to be defined for each front end.  It should
# contain the name of the front-end class defined in this module.
frontEndClass = "FrontEndAIM"

# default config file values for this module
configDefaults = {
	"aim.active":			"no",
	"aim.screenname":		"botname",
	"aim.password":			"changeme",
	"aim.profile":			"Your profile goes here."
	}

class FrontEndAIM(toc.TocTalk, frontend.IFrontEnd):
	def __init__(self):
		config = howie.configFile.get()
		try: screenname = config['aim.screenname']
		except KeyError: screenname = raw_input("AIM screenname: ")
		try: password = config['aim.password']
		except KeyError: password = getpass.getpass("AIM password for %s: " % screenname)
		toc.TocTalk.__init__(self, screenname, password)
		try: self._info = config['aim.profile']
		except KeyError: self._info = "No profile."

	def start(self):
		"Do stuff here immediately after the bot connects."
		# Placeholder.
		pass

	def display(self, output, user):
		"Sends output as an IM to the specified user."
		self.do_SEND_IM(user, output)

	def __updateConfig(self):
		"Saves our current configuration (buddy list, etc.) back to the server."
		# Create a TOC-style CONFIG string
		newConfig = "m 1\ng Buddies\n"
		for buddy in self._buddyList:
			newConfig += "b " + buddy + "\n"
		self.do_SET_CONFIG(newConfig)

	def __addBuddy(self, newBuddy):
		"Add a new buddy, both locally and remotely."
		if self._buddyList.count(newBuddy) > 0:
			# Buddy already in list.
			return
		self.do_ADD_BUDDY(list(newBuddy))
		self.__buddyList.append(newBuddy)
		self.__updateConfig()

	def __removeBuddy(self, oldBuddy):
		"Remove an existing buddy from the buddy list."
		if self._buddyList.count(oldBuddy) == 0:
			# No such buddy.
			return
		self.do_REMOVE_BUDDY(list(oldBuddy))
		self.__buddyList.remove(oldBuddy)
		self.__updateConfig()

	### AIM event handlers follow

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

		# Send a reply.
		# Insert a random delay of 0-4 seconds before replying.
		if not bAutoResponse:
			time.sleep( random.random() * 4 )
			self.display(self.submit(message, screenname), screenname)

		
	# Called when there's an incoming user configuration (our
	# buddy list).  Can be empty.  Format of data : The config
	# information is a string containing a series of lines with
	# the first character in each being the item type, followed by
	# a space, with the rest of the line being the item
	# value. Only letters, numbers, and spaces should be used.
	def on_CONFIG(self, data):
		# first time logging in--add buddies from config...
		self.__buddyList = []
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
				self.__buddyList.extend(budsToAdd)
				time.sleep(0.2) # don't SLAM the server...
				budsToAdd = []

		# Clean up any leftovers.
		if len(budsToAdd) > 0:
			self.do_ADD_BUDDY(budsToAdd)
			self.__buddyList.extend(budsToAdd)
			budsToAdd = []

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

# if this file is run directly...
if __name__ == "__main__":
	pass
