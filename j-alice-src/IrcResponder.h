/**
 * IrcResponder - Interacts with users via IRC
 *
 * @author	Jonathan Roewen
 */
#ifndef IRC_RESPONDER_H
#define IRC_RESPONDER_H

#include "Responder.h"
#include "Socket.h"
#include "ServerSocket.h"

#include <queue>

using namespace std;

class IdentServer : public ServerSocketListener, public SocketListener
{
public:
	IdentServer(const string &);
	~IdentServer();
	
	ServerSocket *identServer;
	
	void awaitingClient(Socket *);
	void shutdown(const string &) { }
	
	void connected() { }
	void disconnected(const string &) { }
	void recv(string &);
	
	string botNick;
};

class IrcResponder : public Responder, public SocketListener
{
public:
	IrcResponder(const string &, const int &, const string &, const string &);
	~IrcResponder();
	
	string respond(Match *, PElement, const string &);
	
	void execute();
	
	//////////
	void connected();
	void disconnected(const string &);
	void processLine(string &);
	void sendCommand(const string &);
	void recv(string &);	//	From SocketListener
	void send(const string &);
	void connect();
	void disconnect();
	void disconnected();	//	From SocketListener
private:
	string server, nick, channel, buffer, lastnick;
	int port;
	Socket *sock;
	queue<string> q;
};

#endif	//	IRC_RESPONDER_H
