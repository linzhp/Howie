/**
 * ThatProcessor - Retrieves bot's last response or something
 *
 * @author	Jonathan Roewen
 */
#ifndef THAT_PROCESSOR_H
#define THAT_PROCESSOR_H

#include "AimlProcessor.h"
#include "Kernel.h"
#include "Memory.h"

#include <string>

using namespace std;

class ThatProcessor : public AimlProcessor
{
public:
	~ThatProcessor() { }
	
	string getName() const {
		return "that";
	}
	string getVersion() const {
		return "1.0";
	}
	string process(Match *, PElement e, Responder *, const string &id) {
		string index = e->getAttribute("index");
		if (index.empty()) {
			index = "1";
		}
		return Memory::pop("that", id, index[0] - '0');
	}
};

#endif
