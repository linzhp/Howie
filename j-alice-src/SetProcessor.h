/**
 * SetProcessor - Used to set value of a user predicate
 *
 * @author	Jonathan Roewen
 */
#ifndef SET_PROCESSOR_H
#define SET_PROCESSOR_H

#include "AimlProcessor.h"
#include "Memory.h"
#include "Kernel.h"
#include "Utils.h"

#include <string>

using namespace std;

class SetProcessor : public AimlProcessor
{
public:
	~SetProcessor() { }
	
	string getName() const {
		return "set";
	}
	string getVersion() const {
		return "1.0";
	}
	string process(Match *m, PElement e, Responder *r, const string &id) {
		string property = toLower(e->getAttribute("name", m, id));
		string value = "";
		if (!e->hasChildren()) {
			value = m->getInputStar(1);
		} else {
			value = Kernel::process(m, e, r, id);
		}
		Memory::setValue(property, id, value);
		return value;
	}
};

#endif
