import getpass
import msnp
import random
import time

import howie.configFile
import frontend

# This string needs to be defined for each front end.  It should
# contain the name of the front-end class defined in this module.
frontEndClass = "FrontEndMSN"

# default config file values for this module
configDefaults = {
    "msn.active":			"no",
    "msn.screenname":		"",
    "msn.password":			"",
    "msn.displayname":      "Howie",
}

# HACK: this class derives from IFrontEnd to give it access to
# the submit() method.  the REAL front-end class is down below.
class MsnChatListener(msnp.ChatCallbacks, frontend.IFrontEnd):
    def __init__(self):
        # Get max delay (in seconds) from the config file.
        config = howie.configFile.get()
        try: self._maxdelay = int(config['general.maxdelay'])
        except KeyError: self._maxdelay = 0
        
    def message_received(self, passport_id, display_name, text, charset):
        # Pass the message back to the Kernel for processing.
        response = self.submit(text, passport_id+"@MSN")
        time.sleep( random.random() * self._maxdelay )
        self.chat.send_message(response, charset)

class MsnListener(msnp.SessionCallbacks):
    def __init__(self):
        self._session = None

    def set_session(self, session):
        "Provides a local reference to the Session object."
        self._session = session
    
    def chat_started(self, chat):
        callbacks = MsnChatListener()
        chat.callbacks = callbacks
        callbacks.chat = chat

    def friend_added(self, list_, passport_id, display_name, group_id = -1):
        if list_ == msnp.Lists.REVERSE:
            # somebody's made us a friend.  Add them to our list as well.
            print "%s (%s) wants us to be his friend!" % (passport_id, display_name)
            self._session.add_friend(msnp.Lists.ALLOW, passport_id)
            self._session.add_friend(msnp.Lists.FORWARD, passport_id)
            print "%s is now our friend!" % passport_id

    def friend_list_updated(self, friend_list):
        friends = []
        friends.extend(self._session.friend_list.get_friends(msnp.Lists.ALLOW))
        #friends.extend(self._session.friend_list.get_friends(msnp.Lists.BLOCK))
        friends.extend(self._session.friend_list.get_friends(msnp.Lists.FORWARD))
        #friends.extend(self._session.friend_list.get_friends(msnp.Lists.REVERSE))
        #print "My friends are:"
        #for f in friends:
        #    print " - %s (%s): %s" % (f.get_passport_id(), f.get_display_name(), f.get_state())

class FrontEndMSN(frontend.IFrontEnd):
    def __init__(self):
        # Create session
        self._session = msnp.Session(MsnListener())
        self._session.callbacks.set_session(self._session)
        # Read screenname and password from config file.  If they're not
        # found, prompt the user for them.
        config = howie.configFile.get()
        try:
            screenname = config['msn.screenname']
            if screenname == "": raise KeyError
        except KeyError: screenname = raw_input("MSN Passport ID: ")
        try:
            password = config['msn.password']
            if password == "": raise KeyError
        except KeyError: password = getpass.getpass("MSN password for %s: " % screenname)
        # log in
        self._session.login(screenname, password)
        # Set display name
        try: displayname = config['msn.displayname']
        except KeyError: displayname = configDefaults["msn.displayname"]
        self._session.change_display_name(displayname)
        # Fetch friends list
        self._session.sync_friend_list()


    def display(self, output, user):
        "Sends output as an IM to the specified user."
        # Not sure how to implement this
        print "FrontEndMSN.display() is not yet implemented"

    def go(self):
        # serve forever
        while(True):
            self._session.process(chats = True)
            time.sleep(1)