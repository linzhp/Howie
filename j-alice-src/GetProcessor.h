/**
 * GetProcessor - Retrieves value of a user predicate
 *
 * @author	Jonathan Roewen
 */
#ifndef GET_PROCESSOR_H
#define GET_PROCESSOR_H

#include "AimlProcessor.h"
#include "Memory.h"
#include "Kernel.h"
#include "Utils.h"

#include <string>
#include <iostream>

using namespace std;

class GetProcessor : public AimlProcessor
{
public:
	~GetProcessor() { }
	
	string getName() const {
		return "get";
	}
	string getVersion() const {
		return "1.0";
	}
	string process(Match *m, PElement e, Responder *r, const string &id) {
		string conjunction = e->getAttribute("conjunction", m, id);
		if (conjunction.empty()) {
			conjunction = " and ";
		}
		//	Ignore conjunction for time being
		string result = Memory::getValue(toLower(e->getAttribute("name", m, id)), id);
		if (result.empty()) {
			result = Kernel::process(m, e, r, id);
		}
		return result;
	}
};

#endif
