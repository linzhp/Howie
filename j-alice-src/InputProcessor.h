/**
 * InputProcessor - Learns AIML files (for now)
 *
 * @author	Jonathan Roewen
 */
#ifndef INPUT_PROCESSOR_H
#define INPUT_PROCESSOR_H

#include "AimlProcessor.h"
#include "Memory.h"
#include "Kernel.h"

#include <string>

using namespace std;

class InputProcessor : public AimlProcessor
{
public:
	~InputProcessor() { }
	
	string getName() const {
		return "input";
	}
	string getVersion() const {
		return "1.0";
	}
	string process(Match *, PElement e, Responder *, const string &id) {
		string index = e->getAttribute("index");
		if (index.empty()) {
			index = "1";
		}
		return Memory::pop("input", id, index[0] - '0');
	}
};

#endif
