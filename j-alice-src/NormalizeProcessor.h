/**
 * NormalizeProcessor - Normalizes output
 *
 * @author	Jonathan Roewen
 */
#ifndef NORMALIZE_PROCESSOR_H
#define NORMALIZE_PROCESSOR_H

#include "AimlProcessor.h"
#include "Substituter.h"
#include "Kernel.h"

#include <string>

using namespace std;

class NormalizeProcessor : public AimlProcessor
{
public:
	~NormalizeProcessor() { }
	
	string getName() const {
		return "normalize";
	}
	string getVersion() const {
		return "1.0";
	}
	string process(Match *m, PElement e, Responder *r, const string &id) {
		return Substituter::normalize(Kernel::process(m, e, r, id));
	}
};

#endif
