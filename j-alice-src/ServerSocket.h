#ifndef SERVER_SOCKET_H
#define SERVER_SOCKET_H

#include <string>

#include "Socket.h"

using namespace std;

class ServerSocketListener /*: public SocketListener*/ {
public:
	virtual void awaitingClient(Socket *) = 0;
	virtual void shutdown(const string &) = 0;
};

class ServerSocket : public Socket {
public:
	ServerSocket(const int &port);
	virtual ~ServerSocket();

	void init();
	
	void setServerListener(ServerSocketListener *);
	ServerSocketListener *getServerListener();

	virtual void process();
protected:
	ServerSocketListener *serverListener;
};

#endif*
