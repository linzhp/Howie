#ifndef SOCKET_HANDLER_H
#define SOCKET_HANDLER_H

#include <vector>
#include <string>

using namespace std;

class Socket;

class SocketHandler {
public:
	static void runLoop();
	static void addSocket(Socket *s);
private:
	static vector<Socket *> sockets;
	static bool alreadyRunning;
	friend class Socket;
};

#endif
