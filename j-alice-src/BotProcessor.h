/**
 * BotProcessor - Retrieves patterns-side bot property
 *
 * @author	Jonathan Roewen
 */
#ifndef BOT_PROCESSOR_H
#define BOT_PROCESSOR_H

#include "AimlProcessor.h"
#include "Kernel.h"

#include <string>

using namespace std;

class BotProcessor : public AimlProcessor
{
public:
	~BotProcessor() { }
	
	string getName() const {
		return "bot";
	}
	string getVersion() const {
		return "1.0";
	}
	string process(Match *, PElement e, Responder *, const string &id) {
		string property = e->getAttribute("name");
		return Kernel::respond("BOT " + property, id);
	}
};

#endif
