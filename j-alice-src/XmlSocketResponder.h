#ifndef XML_SOCKET_RESPONDER_H
#define XML_SOCKET_RESPONDER_H

#include "Responder.h"
#include "AimlProcessor.h"
#include "ServerSocket.h"
#include "Socket.h"
#include "Parser.h"

using namespace std;

class XmlSocketResponder : public Responder, public SocketListener
{
public:
	XmlSocketResponder(Socket *);
	virtual ~XmlSocketResponder() {
		delete parser;
	}
	//	RESPONDER
	string respond(Match *, PElement, const string &);
	//	SOCKET LISTENER
	void recv(string &);
	void connected();
	void disconnected(const string &);
	//	THIS
	void send(const string &);
private:
	string buffer;
	Socket *client;
	Parser *parser;
	
	string botName;
};

class XmlSocketProcessor : public AimlProcessor, public ServerSocketListener
{
public:
	XmlSocketProcessor();
	virtual ~XmlSocketProcessor() { }
	//	AIML PROCESSOR
	string getName() const {
		return "xmlsocket";
	}
	string getVersion() const {
		return "1.0";
	}
	string process(Match *, PElement, Responder *, const string &);
	//	SERVER SOCKET LISTENER
	void shutdown(const string &);
	void awaitingClient(Socket *);
	//	THIS
private:
	int port;
	ServerSocket *server;
};

#endif	//	XML_SOCKET_RESPONDER_H