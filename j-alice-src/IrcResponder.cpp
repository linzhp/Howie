#include "IrcResponder.h"

#include "StringTokenizer.h"
#include "TokenProxyListener.h"
#include "Kernel.h"
#include "Utils.h"
#include "SocketHandler.h"
#include "Logger.h"
#include "Memory.h"
//#ifdef __BEOS__
// #include <sstream>
// #define stringstream strstream
//#else
 #include <strstream>
//#endif

#include <iostream>

using namespace std;

//	IDENT CRAP FIRST

Socket *identC = NULL;

IdentServer::IdentServer(const string &bn) {
	botNick = bn;
	identServer = new ServerSocket(113);
	identServer->setServerListener(this);
	identServer->init();
	cout << "Initialised IdentServer" << endl;
}

IdentServer::~IdentServer() {
	identServer->setServerListener(NULL);
	cout << "Deleting the identServer socket" << endl;
	delete identServer;
}

void IdentServer::awaitingClient(Socket *identClient) {
	cout << "Got a client" << endl;
	identC = identClient;
	identC->setListener(this);
	identC->process();
//	identC->disconnect();
//	identC->setListener(NULL);
//	delete identC;
//	identC = NULL;
}

void IdentServer::recv(string &s) {
	cout << "IDENT REQUEST: " << s << endl;
	if (trim(s).empty()) {
		return;
	}
	int serverPort, clientPort;
	StringTokenizer st(s, ", ");
	serverPort = atoi(st.nextToken().c_str());
	clientPort = atoi(st.nextToken().c_str());
	
	cout << serverPort << " : " << clientPort << endl;
	
	strstream reply;
	reply << serverPort << ", " << clientPort << " : USERID : UNIX : " << botNick;
	s = reply.str();
	cout << "IDENT REPLY: " << s << endl;
	identC->write(s + "\r\n");
//	TEST PUTTING THIS HERE
//	identC->disconnect();
//	identC->setListener(NULL);
//	delete identC;
//	identC = NULL;
}

//	END OF IDENT

//IdentServer *is = NULL;

IrcResponder::IrcResponder(const string &server, const int &port, const string &nicklist, const string &chanlist) {
//	if (is == NULL) {
//		cout << "Creating new IdentServer" << endl;
//		is = new IdentServer(nicklist);
//	}
	//	Nick & Channel Lists won't yet be implemented
	this->server = server; this->port = port;
	sock = new Socket(server, port);
	sock->setListener(new TokenProxyListener(this));
	nick = nicklist; channel = chanlist;
	sock->connect();
}

void IrcResponder::connected() {
	connect();
}

IrcResponder::~IrcResponder() {
	if (sock != NULL) {
		disconnect();
		delete sock;
	}
	sock = NULL;
}

string IrcResponder::respond(Match *m, PElement element, const string &id) {
	if (element->getNamespace() != "irc") {
		return "";
	} else {
		string tag = element->getTagname();
		if (tag == "channel") {
			return channel;
		} else if (tag == "lastnick") {
			return lastnick;
		} else if (tag == "command") {
			q.push(Kernel::process(m, element, this, id));
		} else if (tag == "exit") {
			//	shutdown the bot ..
			exit(0);	//	Hopefully this calls destructors
		} else {
			cout << "<irc:" << tag << "> not supported" << endl;
		}
	}
	return "";
}

void IrcResponder::execute() {
	while (!q.empty()) {
		sendCommand(q.front());
		q.pop();
	}
}

void IrcResponder::recv(string &recv) {
	//	Don't need a buffer anymore! :-)
	//	Since now with the TokenProxyListener, we will
	//	only get sent a single line at a time, with
	//	the CRLF stripped off as well :-)
	processLine(recv);
}

void IrcResponder::send(const string &send) {
	if (sock == NULL) {
		return;
	}
	//	To simplify code elsewhere, append the CRLF
	//	here :-)
	sock->write(send + "\r\n");
}

void IrcResponder::connect() {
	//	Order as recommended in RFC1459
	//	Later, will let this be configured somehow
	//	Perhaps from <bot name="email"/>
	send("PASS "+ nick +"@j-alice.sf.net");
	send("NICK " + nick);
	send("USER " + nick + " j-alice.sourceforge.net " + server + " :http://j-alice.sourceforge.net");
	send("JOIN " + channel);
	cout << "Connected to " << server << endl;
//	sock->looper();
	SocketHandler::runLoop();
}

void IrcResponder::disconnect() {
	send("QUIT :joob joob");
	sock->disconnect();
	cout << "Disconnected from " << server << " (software)" << endl;
}

void IrcResponder::disconnected(const string &msg) {
//	if (is != NULL) {
//		cout << "Deleting IdentServer" << endl;
//		delete is;
//		is = NULL;
//	}
	if (sock != NULL) {
	//	delete sock;
	//	sock = NULL;
	}
	cout << "Disconnected from " << server << " (" << msg << ")" << endl;
}

//	sendCommand(string)

void IrcResponder::sendCommand(const string &command) {
	string::size_type pos = command.find(" ");
	if (pos != string::npos) {
		send(toUpper(command.substr(0, pos)) + command.substr(pos));
	} else {
		send(command);
	}
}

//	processLine(string)
/*
<message>  ::= [':' <prefix> <SPACE> ] <command> <params> <crlf>
<prefix>   ::= <servername> | <nick> [ '!' <user> ] [ '@' <host> ]
<command>  ::= <letter> { <letter> } | <number> <number> <number>
<SPACE>    ::= ' ' { ' ' }
<params>   ::= <SPACE> [ ':' <trailing> | <middle> <params> ]

<middle>   ::= <Any *non-empty* sequence of octets not including SPACE
               or NUL or CR or LF, the first of which may not be ':'>
<trailing> ::= <Any, possibly *empty*, sequence of octets not including
                 NUL or CR or LF>

<crlf>     ::= CR LF
//
<target>     ::= <to> [ "," <target> ]
<to>         ::= <channel> | <user> '@' <servername> | <nick> | <mask>
<channel>    ::= ('#' | '&') <chstring>
<servername> ::= <host>
<host>       ::= see RFC 952 [DNS:4] for details on allowed hostnames
<nick>       ::= <letter> { <letter> | <number> | <special> }
<mask>       ::= ('#' | '$') <chstring>
<chstring>   ::= <any 8bit code except SPACE, BELL, NUL, CR, LF and
                 comma (',')>

Other parameter syntaxes are:

<user>       ::= <nonwhite> { <nonwhite> }
<letter>     ::= 'a' ... 'z' | 'A' ... 'Z'
<number>     ::= '0' ... '9'
<special>    ::= '-' | '[' | ']' | '\' | '`' | '^' | '{' | '}'
<nonwhite>   ::= <any 8bit code except SPACE (0x20), NUL (0x0), CR
                     (0xd), and LF (0xa)>
*/

void IrcResponder::processLine(string &line) {
	//	Hehe, we already have a single line ready :)
	//	So no silly while loops or end-of-line searching
	string sender, command, params;
	if (line[0] == ':') {
		//	Then we have a PREFIX
		string prefix = line.substr(1);
		sender = prefix.substr(0, prefix.find("!"));
		string::size_type ix = prefix.find(" ");
		command = prefix.substr(ix + 1, prefix.find(" ", ix + 1) - (ix + 1));
		params = trim(prefix.substr(prefix.find(" ", ix + 1) + 1));
	} else {
		string::size_type ix = line.find(" ");
		command = line.substr(0, ix);
		params = trim(line.substr(ix + 1));
	}
	if (command.empty()) {
		cerr << "Error processing line from server" << endl;
		cerr << line << endl;
		return;
	}
	if (!sender.empty()) {
		lastnick = sender;
	}
//	cout << command << "|" << params << endl;
	//	Now interpret the command, and (possibly) do something :-)
	if (command == "JOIN") {
		//	sender has joined this channel .. so say hello :-)
		//	Use the <pattern>CONNECT</pattern> message for this
		//	What would also like to do is silently set the user's name that the
		//	bot stores to the user's current nick.
		//	Later, can figure out how to use hostmasks to recognise a user.
	} else if (command == "INVITE") {
		//	sender invited me to a channel
		//	first space delimited param is me, 2nd space delimited param is channel
		string::size_type ix = params.rfind(" ");
		string chan = params.substr(ix + 1);
		//	Later, want to spawn a new Responder, so that we know exactly which
		//	channel we are on...
		channel = chan;
		send("JOIN " + chan);
	} else if (command == "KICK") {
		//	sender may have kicked me .. join back
		// Later, maybe have it ask why, based on comment?
		//...
	} else if (command == "PRIVMSG") {
		//	received a privmsg (either to a channel I'm on or directly to me)
		//	message follows first occurence of " :"
		//	target is the first space delimited paramater in params
		if (sender == nick) {
			return;
		}	//	Don't want to reply to ourselves, hehe
		
		string logString = "";
		//	Automagically set person's name to their IRC nick, unless
		//	they've given the bot a different name, like:
		//	IRCUser > IRCBot: what is my name?
		//	IRCBot > IRCUser: your name is IRCUser.
		//	IRCUser > IRCBot: my name is Bob
		//	IRCBot > IRCUser: Okay, I'll call you Bob
		//	IRCUser > IrcBot: what is my name?
		//	IRCBot > IRCUser: your name is Bob.
		if (Memory::getValue("name", sender).empty()) {
			Memory::setValue("name", sender, sender);
		}
		
		string::size_type ix = params.find(" :");
		string target = params.substr(0, ix);
		string message = params.substr(ix + 2);
		if (!target.empty() && (target[0] == '#' || target[0] == '&')) {
			//	The target of the message was a channel
			//	So, now we have to determine if will reply
			if (message.find(nick) == 0) {
				//	Then message is: <nick><message>
				message = trim(message.substr(nick.length()));
				if (message.length() > 0 && message[0] == ':') {
					message = trim(message.substr(1));
				}
				if (!message.empty()) {
					StringTokenizer lines(Kernel::respond(message, sender, this), "\n");
					logString = sender + " > " + message;
					while (lines.hasMoreTokens()) {
						string next = lines.nextToken();
						logString += "\nbot > ";
						send("PRIVMSG " + target + " :" + sender + ": " + next);
						logString += next;
					}
				}
			} else if (message.find(nick) != string::npos) {
				//	My nick ain't at the start, but it's in the message
				string::size_type minSearch = 0;
				string bot_name = Kernel::respond("BOT NAME", sender, this);
				while ((minSearch = message.find(nick, minSearch)) != string::npos) {
					string m_head = message.substr(0, minSearch);
					string m_tail = message.substr(minSearch + nick.length());
					message = m_head + bot_name + m_tail;
				}
				if (!message.empty()) {
					StringTokenizer lines(Kernel::respond(message, sender, this), "\n");
					logString = sender + " > " + message;
					while (lines.hasMoreTokens()) {
						string next = lines.nextToken();
						logString += "\nbot > ";
						send("PRIVMSG " + target + " :" + sender + ": " + next);
						logString += next;
					}
				}
			}
		} else {
			//	The target is me :-)
			if (!message.empty()) {
				StringTokenizer lines(Kernel::respond(message, sender, this), "\n");
				logString = sender + " > " + message;
				while (lines.hasMoreTokens()) {
					string next = lines.nextToken();
					logString += "\nbot > ";
					send("PRIVMSG " + sender + " :" + next);
					logString += next;
				}
			}
		}
		if (!logString.empty()) {
			Logger::log(logString, "irc-" + server);
		}
		execute();	//	seems the place to do it
	} else if (command == "NOTICE" || command == "ERROR") {
		//	Send to console I suppose
		cout << "\t*** IRC: " << command << " " << params << endl;
	} else if (command == "KILL") {
		//	Check if I got KILLed .. and reconnect
	} else if (command == "PING") {
		//	Need to reply to this!
		send("PONG " + params);
	} else {
		//	Try to convert to a numeric code (3-digit)
		int numeric = atoi(command.c_str());
		//	Put here I suppose
		static int notregisteredcount = 0;
		switch (numeric) {
			//	Only consider useful commands...
			case 433:
				//	This is where we would change nick .. but can't, so disconnect
				cerr << "*** IRC: Nick is in use .. disconnecting" << endl;
				disconnect();
				return;
			case 451:
				//	This is a bitchy problem
				++notregisteredcount;
				if (notregisteredcount > 3) {
					//	No success with registering
					cerr << "*** IRC: Couldn't register .. use a different server" << endl;
					disconnect();
				}
				return;
			case 464:
				cerr << "*** IRC: Password invalid (or not given) .. cannot connect" << endl;
				disconnect();
				return;
			case 465:
				cerr << "*** IRC: Banned from this server .. disconnected" << endl;
				disconnect();
				return;
			default:
				return;	//	Do nothing :-)
		}
	}
}
