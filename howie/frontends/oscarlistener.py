"""This class encapsulates the common functionality of the Oscar protocol,
used by AIM and ICQ.
"""

import getpass
import random
import time
from twisted.internet import selectreactor
#default.install()
from twisted.words.protocols import oscar
from twisted.internet import protocol, reactor

import howie.configFile
import frontend

# Hack: this isn't the real frontend class.
class OscarListener(oscar.BOSConnection, frontend.IFrontEnd):
    capabilities = [oscar.CAP_CHAT]
    def initDone(self):
        self.requestSelfInfo().addCallback(self.gotSelfInfo)
        self.requestSSI().addCallback(self.gotBuddyList)
    def gotSelfInfo(self, user):
        #print user.__dict__
        self.name = user.name
    def gotBuddyList(self, l):
        #print l
        self.activateSSI()
        config = howie.configFile.get()
        try: profile = config['icq.profile']
        except KeyError: profile = "No profile."
        self.setProfile(profile)
        try: self._maxdelay = int(config['general.maxdelay'])
        except KeyError: self._maxdelay = 0		
        self.setIdleTime(0)
        self.clientReady()
        #self.createChat('%s Chat'%SN).addCallback(self.createdRoom)
    def createdRoom(self, (exchange, fullName, instance)):
        #print 'created room',exchange, fullName, instance
        self.joinChat(exchange, fullName, instance).addCallback(self.chatJoined)
    def updateBuddy(self, user):
        #print user
        pass
    def offlineBuddy(self, user):
        #print 'offline', user.name
        pass
    def receiveMessage(self, user, multiparts, flags):
        #print user.name, multiparts, flags
        self.getAway(user.name).addCallback(self.gotAway, user.name)
        if multiparts[0][0].find('away')!=-1:
            self.setAway('I am away from my computer right now.')
        elif multiparts[0][0].find('back')!=-1:
            self.setAway(None)
        if self.awayMessage:
            self.sendMessage(user.name,'<html><font color="#0000ff">'+self.awayMessage,autoResponse=1)
        else:
            self.lastUser = user.name
            time.sleep( random.random() * self._maxdelay )
            response = self.submit(oscar.dehtml(multiparts[0][0]), user.name)
            self.sendMessage(user.name, response, wantAck = 1, autoResponse = (self.awayMessage!=None)).addCallback( \
                self.messageAck)
    def messageAck(self, (username, message)):
        #print 'message sent to %s acked' % username
        pass
    def gotAway(self, away, user):
        #if away != None:
        #    print 'got away for',user,':',away
        pass
    def receiveWarning(self, newLevel, user):
        #print 'got warning from', hasattr(user,'name') and user.name or None
        #print 'new warning level', newLevel
        if not user:
            #username = self.lastUser
            return 
        else:
            username = user.name
        self.warnUser(username).addCallback(self.warnedUser, username)
    def warnedUser(self, oldLevel, newLevel, username):
        #self.sendMessage(username,'muahaha :-p')
        pass
    def receiveChatInvite(self, user, message, exchange, fullName, instance, shortName, inviteTime):
        #print 'chat invite from',user.name,'for room',shortName,'with message:',message
        self.joinChat(exchange, fullName, instance).addCallback(self.chatJoined)
    def chatJoined(self, chat):
        #print 'joined chat room', chat.name
        #print 'members:',map(lambda x:x.name,chat.members)
        pass
    def chatReceiveMessage(self, chat, user, message):
        #print 'message to',chat.name,'from',user.name,':',message
        if user.name!=self.name: chat.sendMessage(user.name+': '+message)
        #if message.find('leave')!=-1 and chat.name!='%s Chat'%SN: chat.leaveChat()
    def chatMemberJoined(self, chat, member):
        #print member.name,'joined',chat.name
        pass
    def chatMemberLeft(self, chat, member):
        #print member.name,'left',chat.name
        #print 'current members',map(lambda x:x.name,chat.members)
        if len(chat.members)==1:
            #print 'leaving', chat.name
            chat.leaveChat()
