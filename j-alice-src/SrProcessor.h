/**
 * SrProcessor - Symbolic Reduction on <star/>
 *
 * @author	Jonathan Roewen
 */
#ifndef SR_PROCESSOR_H
#define SR_PROCESSOR_H

#include "AimlProcessor.h"
#include "Kernel.h"

#include <string>

using namespace std;

class SrProcessor : public AimlProcessor
{
public:
	~SrProcessor() { }
	
	string getName() const {
		return "sr";
	}
	string getVersion() const {
		return "1.0";
	}
	string process(Match *m, PElement, Responder *, const string &id) {
		string star = m->getInputStar(1);
		return Kernel::respond(star, id, true);
	}
};

#endif
