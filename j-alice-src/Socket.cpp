#include "Socket.h"

#include "Logger.h"	//	LOGGING FACILITY

#ifdef WIN32
 #include <winsock2.h>
#else
 #include <netdb.h>
 #include <sys/socket.h>
 #include <netinet/in.h>
 #include <unistd.h>
#endif
#include <stdio.h>
#include <errno.h>
#include "SocketHandler.h"

#include <iostream>

#include "Compat.h"
#include "Utils.h"

using namespace std;

#ifdef WIN32
	extern "C" {
		void win32_init_sockets(void);
		
		int winsock_handle = -1;
		int socket_count = 0;
		
		void win32_init_sockets(void) {
			WSADATA d;
			
			if (winsock_handle == -1) {
				winsock_handle = WSAStartup(MAKEWORD(2, 0), &d);
			}
		}
	}
	#define EWOULDBLOCK WSAEWOULDBLOCK 
#endif

Socket::Socket(int socket, const char* name, int p):listener(NULL), sd(socket),
		port(p), host(name) {
	//	Needed for inheritance
	#ifdef WIN32
		win32_init_sockets();
	#endif
	SocketHandler::addSocket(this);
}

Socket::Socket(const string &host, const int &port) {
	this->host = mystrdup(host.c_str());
	this->port = port;
	#ifdef WIN32
		win32_init_sockets();
	#endif
	listener = NULL;
	sd = -1;
	
	struct sockaddr_in sa;
	
	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		disconnect("socket() failed");
		return;
	}
	
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = htonl(INADDR_ANY);
	sa.sin_port = htons(0);
	memset(sa.sin_zero, 0, sizeof(sa.sin_zero));
	
	if (bind(sd, (struct sockaddr *)&sa, sizeof(sa)) < 0) {
		//	This isn't lethal .. can still continue
		perror("bind");
	}
	
	SocketHandler::addSocket(this);
}

Socket::~Socket() {
	disconnect();
	//TODO: remove self from the socket loop
//	As far as I can tell, this don't work
//	for (int ix = 0; ix < SocketHandler::sockets.size(); ++ix) {
//		if (SocketHandler::sockets[ix] == this) {
//			SocketHandler::sockets[ix]->process();
//			SocketHandler::sockets[ix] = NULL;
//		}
//	}
	delete [] host;
	host = NULL;
}

const int buffer_size = 5000;

void Socket::connect() {
	if (sd < 0) {
		//	Constructing the socket failed previously
		if (listener != NULL) {
			listener->disconnected("couldn't create socket");
		}
	}
	struct sockaddr_in sa;
	struct hostent *he;
	
	he = gethostbyname(host);
	if (he == NULL) {
		disconnect(string(string(host) + " unknown").c_str());
		Logger::log(string(host) + " could not be found [unknown host]", "networking");
		return;
	}
	
	sa.sin_family = he->h_addrtype;
	memcpy((char *)&sa.sin_addr.s_addr, he->h_addr_list[0], he->h_length);
	sa.sin_port = htons(port);
	memset(sa.sin_zero, 0, sizeof(sa.sin_zero));
	
	if (::connect(sd, (struct sockaddr *)&sa, sizeof(sa)) < 0) {
		disconnect(string("failed connect to "+ string(host)).c_str());
		Logger::log("connection to " + string(host) + " failed", "networking");
		return;
	}
	if (listener != NULL) {
		listener->connected();
	}
}

void Socket::disconnect(const char *reason) {
	string out = "";
	if (reason != NULL) {
		if (host == NULL) {
			out = string(reason);
		} else {
			out = string(host) + ": " + string(reason);
		}
	}
	if (sd != -1) {
		closesocket(sd);
		sd = -1;
		if (listener != NULL) {
			listener->disconnected(out);
		}
	}
}

void Socket::setListener(SocketListener *listener) {
	this->listener = listener;
}

SocketListener *Socket::getListener() {
	return listener;
}

int Socket::getSD() {
	return sd;
}

void Socket::process() {
	//	This gets called from SocketHandler when we
	//	have something waiting for us (data)
	string str;
	read(str);
}

int Socket::read(string &str) {
	int n = 0;
	int buf_size = buffer_size;
	if (buf_size > 1) {
		--buf_size;
	}
	char *buffer = new char[buffer_size];
	n = read_raw(buffer, buf_size);
	buffer[n] = '\0';
	str = string(buffer);
	if (listener != NULL) {
		listener->recv(str);
	}
	return n;
}

int Socket::write(const string &str, bool appendNullByte) {
	int n = write_raw(str.c_str(), str.length());
	if (appendNullByte && n >= 0) {
		char ch = '\0';
		return n + write_raw(&ch, 1);
	}
	return n;
}

int Socket::read_raw(void *buffer, int buf_size) {
	int n = 0;
	if (sd < 0) {
		return n;
	}
	
	#ifdef WIN32
		n = recv(sd, (char FAR *)buffer, buf_size, 0);
	#else
		n = recv(sd, (char *)buffer, buf_size, 0);
	#endif
	if (n < 0) {
		if (errno != EWOULDBLOCK) {
			perror("recv");
			disconnect("error receiving data");
		}
	} else if (n == 0) {
		disconnect("connection closed by peer");
	}
	return n;
}

int Socket::write_raw(const void *data, int len) {
	int ret;
	if (sd == -1) {
		return 0;
	}
	#ifdef WIN32
		ret = send(sd, (char FAR *)data, len, 0);
	#else
		ret = send(sd, (const char *)data, len, 0);
	#endif
	if (ret < 0){
		perror("send");
		disconnect();
	}
	return ret;
}
