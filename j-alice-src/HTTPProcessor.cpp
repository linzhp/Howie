#include "HTTPProcessor.h"

#include "ServerSocket.h"
#include "Socket.h"
#include "SocketHandler.h"

#include <stdlib.h>
#include <string>
//#include <unistd.h>

#include "Kernel.h"
#include "TokenProxyListener.h"
#include "Logger.h"

#include <iostream>

using namespace std;

int num(char c) {
	if (c >= 'a' && c <= 'z') {
		return 10 + c - 'a';
	}
	if (c >= 'A' && c <= 'Z') {
		return 10 + c - 'A';
	}
	char *ch = new char[2];
	ch[0] = c;
	ch[1] = '\0';
	return atoi(ch);
}

string decode(const string &s) {
	string out = "";
	int code = 0;
	for (unsigned int ix = 0; ix < s.length(); ++ix) {
		char ch = s[ix];
		switch (ch) {
			case '+':
				out += ' ';
				break;
			case '%':
				code = num(s[++ix]);
				code *= 16;
				code += num(s[++ix]);
				out += (char)code;
				break;
			default:
				out += ch;
				break;
		}
	}
	return out;
}

class HTTPServer : ServerSocketListener {
public:
	HTTPServer(string &, int );
	virtual ~HTTPServer() { }
	void shutdown(const string &);
	void awaitingClient(Socket *);
private:
	ServerSocket *s;
	string name;
};

class HTTPListener : public SocketListener {
public:
	HTTPListener(Socket *s) {
		socket = s;
		keepalive = false;
		if (bot_name.empty()) {
			bot_name = Kernel::respond("BOT NAME", "system");
		}
	}
	virtual ~HTTPListener() {
	}
	void recv(string &s);
	void disconnected(const string &msg) {
		cout << "HTTP Disconnected: " << msg << endl;
	}
	void connected() {
	//	cout << "Client connected" << endl;
	}
private:
	Socket *socket;
	bool keepalive;
	static string bot_name;
};

string HTTPProcessor::process(Match *m, PElement e, Responder *r, const string &id){
	string name = Kernel::process(m, e->getChild("name"), r, id);
	int port = atoi(Kernel::process(m, e->getChild("port"), r, id).c_str());
	
	new HTTPServer(name,port);
	SocketHandler::runLoop();
	return "";
}

HTTPServer::HTTPServer(string &Name, int port) {
	cout << "Warning: Web Server does not receive requests properly" << endl;
	cout << "If you are having problems, don't use it" << endl;
	s = new ServerSocket(port);
	s->setServerListener(this);
	s->init();
	name = Name;
	cout << "Starting up server" << endl;
}

void HTTPServer::shutdown(const string &msg) {
	cout << "Shutting down server: " << msg << endl;
}

void HTTPServer::awaitingClient(Socket* socket) {
	socket->setListener(new TokenProxyListener(new HTTPListener(socket)));
	socket->process();
}

string HTTPListener::bot_name = "";

void HTTPListener::recv(string &s) {
	string::size_type p;	
	if (s.find("keep-alive") != string::npos) {	//	Needs to be case insensitive search...
		keepalive = true;
	} else if (s.find("Keep-Alive") != string::npos) {
		keepalive = true;
	}
	if(s.find("GET") == 0 && (p = s.find("input=",3))!=string::npos) {
		s.erase(0,p+6);
		if((p=s.find(" "))!=string::npos) {
			s.erase(p);	
		}
		s = decode(s);
	//	cout << "HTTPListener::recv - " + s << endl;
	} else if (s.find("GET") == 0) {
		s = "CONNECT";	//	Try get it to respond on connection...
	} else {
		return;
	}
	//whats the point of waiting for \r\n\r\n since we already know everything we need from GET
	socket->write("HTTP/1.0 200 OK\r\n");
	socket->write("Server: J-Alice_Web_Server_0.3\r\n");
	socket->write("Content-Type: text/html\r\n");
	socket->write("\r\n");	//	Apparently want Server::name or something in place of J-Alice Web Server...
	//	Oh for fuck's suck .. CodeWarrior in Windows is a bitch!
	string str = "<html>";
	str += "<head><title>I am J-Alice!</title></head>";
	str += "<body>";
	str += "<h1>J-Alice Web Server</h1><br>";
	str += bot_name;
	str += ": ";
	string response = Kernel::respond(s, socket->getPeerName());
	str += response;
	str += "<p>";
	str += "<form>";
	str += "Response: <input type=text length=255 name=input> <input type=submit title=Send text=Send>";
	str += "</form>";
	str += "</p>";
	str += "</body>";
	str += "</html>";
	str += "\r\n";
	socket->write(str.c_str());	//	It's lame HTML, so took out the \" and don't need to put as <input .../>
	//	Something to do with this .. and removing a socket from the handler
//	socket->disconnect();
	
	Logger::log(string(socket->getPeerName()) + " > " + s + "\nbot > " + response, "http-server");
	socket->disconnect();
}
