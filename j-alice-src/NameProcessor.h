/**
 * NameProcessor - Retrieves the bot's name (shortcut)
 *
 * @author	Jonathan Roewen
 */
#ifndef NAME_PROCESSOR_H
#define NAME_PROCESSOR_H

#include "AimlProcessor.h"
#include "Kernel.h"

#include <string>

using namespace std;

class NameProcessor : public AimlProcessor
{
public:
	~NameProcessor() { }
	
	string getName() const {
		return "name";
	}
	string getVersion() const {
		return "1.0";
	}
	string process(Match *, PElement, Responder *, const string &id) {
		return Kernel::respond("BOT NAME", id);
	}
};

#endif
