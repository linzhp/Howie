/**
 * IdProcessor - Returns id of the user
 *
 * @author	Jonathan Roewen
 */
#ifndef ID_PROCESSOR_H
#define ID_PROCESSOR_H

#include "AimlProcessor.h"
#include "Kernel.h"

#include <string>

using namespace std;

class IdProcessor : public AimlProcessor
{
public:
	~IdProcessor() { }
	
	string getName() const {
		return "id";
	}
	string getVersion() const {
		return "1.0";
	}
	string process(Match *, PElement, Responder *, const string &id) {
		return id;
	}
};

#endif
