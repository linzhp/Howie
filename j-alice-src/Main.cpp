#include "Kernel.h"
#include "Utils.h"
//#include "SocketHandler.h"
#include "Memory.h"

#include <string>
#include <iostream>

using namespace std;

int main(int argc, char **argv) {
	string inLine = "", outLine;
	bool bootstrap = true;
	bool convert = false;
	if (argc > 0) {
		string s = argv[argc - 1];
		if (s == "--manual") {
			bootstrap = false;
		}
		else if (s == "--convert") {
			bootstrap = false;
			convert = true;
		}
	}
	
	Kernel *k = new Kernel();
	
	if (bootstrap) {
		cout << k->bootstrap() << endl;
	}
	if (convert) {
		k->convertTempAiml();
		return 0;
	}
	
	string botName = k->respond("BOT NAME", "system");
	string userName = "localhost";
	while (inLine != "exit") {
		userName = Memory::getValue("name", "localhost");
		if (userName.empty()) {
			userName = "localhost";
		}
		cout << userName << " > ";
		getline(cin, inLine);
		outLine = Kernel::respond(inLine, "localhost");
		if (outLine.length() > 0) {
			cout << botName << " > " << trim(outLine) << endl;
		}
	}
//	k->respond("run system xml socket server", "system");
//	cout << "Starting socket loop" << endl;
//	SocketHandler::runLoop();
//	cout << "Server shutdown successfully" << endl;
}
