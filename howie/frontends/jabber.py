import getpass
import random
import time

import xmpp

import howie.configFile
import frontend

# This string needs to be defined for each front end.  It should
# contain the name of the front-end class defined in this module.
frontEndClass = "FrontEndJabber"

# List the default values for the INI file.  This should be a dictionary
# with keys of the form "<fe>.<entry>", where <fe> is the name of the
# frontend (e.g. "aim", "msn"), and entry is the name of the configuration
# variable (e.g. "username", "password").  All values should be strings.
#
# Each frontend must define an "active" entry, whose possible values
# are "yes" and "no", which indicates whether that frontend should
# be activated.
configDefaults = {
    "jabber.active":        "no",
    "jabber.username":      "",
    "jabber.password":      "",
    "jabber.server":        "jabber.org",
    "jabber.port":          "5222",
    "jabber.resource":      "default",
    "jabber.nickname":      "Howie",
}

class FrontEndJabber(frontend.IFrontEnd):
    def __init__(self):
        # Read username, password, server, resource and nickname from
        # config file.  If they're not found, prompt the user for
        # them.
        config = howie.configFile.get()
        try:
            username = config['jabber.username']
            if username == "": raise KeyError
        except KeyError: username = raw_input("Jabber Username: ")
        try:
            password = config['jabber.password']
            if password == "": raise KeyError
        except KeyError: password = getpass.getpass("Jabber password for %s: " % username)
        try:
            server = config['jabber.server']
            if server == "": raise KeyError
        except KeyError: server = raw_input("Jabber Server: ")
        try:
            try: server_port = int(config['jabber.port'])
            except ValueError: server_port = ""
            if server_port == "": raise KeyError
        except KeyError: server_port = raw_input("Jabber Port: ")
        try:
            resource = config['jabber.resource']
            if resource == "": raise KeyError
        except KeyError: resource = raw_input("Jabber Resource: ")
        self._jid = "%s@%s/%s" % (username,server,resource)
        try:
            nickname = config['jabber.nickname']
            if nickname == "": raise KeyError
        except KeyError: nickname = raw_input("Jabber Nickname for %s: " % self._jid)
        
        # Read max delay
        try: self._maxdelay = int(config['general.maxdelay'])
        except KeyError: self._maxdelay = 0

        # Connect to server
        self._con = xmpp.Client(server, port=server_port, debug=None)
        if not self._con.connect(server=(server,server_port)):
            sys.stderr.write("JABBER: Couldn't connect to %s(%d): network error\n" % (server,server_port))
            return
        print "JABBER CONNECTED"

        # Register event handlers
        self._con.RegisterHandler('message',self._messageCB)
        self._con.RegisterHandler('presence',self._presenceCB)
        self._con.RegisterHandler('iq',self._iqCB)
        print "JABBER REGISTERED HANDLERS"        
        # Log in
        if self._con.auth(username,password,resource):
            pass
        else:
            print "JABBER ERROR: login failed:", self._con.lastErr, self._con.lastErrCode
            return
        print "JABBER LOGGED IN"
        # Request roster
        self._con.sendInitPresence(1)
        print "JABBER SENT PRESENCE"
        
    def go(self):
        while True:
            time.sleep(1)
            self._con.Process(1)
            # If connection is broken, restore it
            if not self._con.isConnected(): self._con.reconnectAndReauth()


    def display(self, output, user):
        # "user" must be a legitimate JID (user@server)
        msg = xmpp.Message(user, output.strip())
        msg.setType('chat')
        self._con.send(msg)

    def _messageCB(self, con, msg):
        """Called when a message is recieved"""
        if not msg.getBody(): # Dont process blank messages
            return
        # Jabber IDs contain a slash between the server name and the
        # resource name.  This presents a problem, since Howie uses
        # the session name as the filename for the log/session files.
        # So the session name for Jabber sessions is just
        # "username@server".
        source = str(msg.getFrom()).split("/")[0]
        # message body is Unicode, and Howie currently doesn't like
        # Unicode.  Convert it to ASCII with str() before submitting.
        response = self.submit(str(msg.getBody()), source+"@JABBER")
        time.sleep( random.random() * self._maxdelay )
        self.display(response, source)

    def _presenceCB(self, con, prs):
        """Called when a presence is recieved"""
        who = str(prs.getFrom())
        type = prs.getType()
        if type == None: type = 'available'

        # subscription request: 
        # - accept their subscription
        # - send request for subscription to their presence
        if type == 'subscribe':
            #print u"subscribe request from %s" % (who)
            con.send(xmpp.Presence(to=who, typ='subscribed'))
            con.send(xmpp.Presence(to=who, typ='subscribe'))

        # unsubscription request: 
        # - accept their unsubscription
        # - send request for unsubscription to their presence
        elif type == 'unsubscribe':
            #print u"unsubscribe request from %s" % (who)
            con.send(xmpp.Presence(to=who, typ='unsubscribed'))
            con.send(xmpp.Presence(to=who, typ='unsubscribe'))

        #elif type == 'subscribed':
        #    print u"we are now subscribed to %s" % (who)

        #elif type == 'unsubscribed':
        #    print u"we are now unsubscribed to %s"  % (who)

        #elif type == 'available':
        #    print u"%s is available (%s / %s)" % (who, prs.getShow(), prs.getStatus())
        #elif type == 'unavailable':
        #    print u"%s is unavailable (%s / %s)" % (who, prs.getShow(), prs.getStatus())

    def _iqCB(self, con,iq):
        """Called when an iq is recieved, we just let the library handle it at the moment"""
        pass
