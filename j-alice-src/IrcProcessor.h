/**
 * IrcProcessor - Configures and connects to IRC
 *
 * @author	Jonathan Roewen
 */
#ifndef IRC_PROCESSOR_H
#define IRC_PROCESSOR_H

#include "AimlProcessor.h"
#include "IrcResponder.h"
#include "Kernel.h"

#include <string>

using namespace std;

class IrcProcessor : public AimlProcessor
{
public:
	~IrcProcessor() { }
	
	string getName() const {
		return "irc";
	}
	string getVersion() const {
		return "1.0";
	}
	string process(Match *m, PElement e, Responder *r, const string &id) {
		string nick = Kernel::process(m, e->getChild("nick"), r, id);
		string server = Kernel::process(m, e->getChild("server"), r, id);
		int port = atoi(Kernel::process(m, e->getChild("port"), r, id).c_str());
		string channel = Kernel::process(m, e->getChild("channel"), r, id);
		new IrcResponder(server, port, nick, channel);
		
	/*	static IrcClient *ic = NULL;
		if (e->getTagname() == "topic" && ic) {
			return ic->getTopic();
		} else
		if (e->getTagname() == "channel" && ic) {
			return ic->getChannel();
		} else
		if (e->getTagname() == "lastnick" && ic) {
			return ic->getLastnick();
		} else
		if (e->getTagname() == "command" && ic) {
			ic->sendCommand(Kernel::process(m, e, r, id));
		} else {
			string nick = Kernel::process(m, e->getChild("nick"), r, id);
			string server = Kernel::process(m, e->getChild("server"), r, id);
			int port = atoi(Kernel::process(m, e->getChild("port"), r, id).c_str());
			string channel = Kernel::process(m, e->getChild("channel"), r, id);
			
			ic = new IrcClient(server, port);
			ic->connect(nick, channel);
		}*/
		return "";
	}
};

#endif
