/**
 * StarProcessor - Retrieves value of the wildcard in the
 *		input pattern
 *
 * @author	Jonathan Roewen
 */
#ifndef STAR_PROCESSOR_H
#define STAR_PROCESSOR_H

#include "AimlProcessor.h"

#include <string>

using namespace std;

#include "Utils.h"

class StarProcessor : public AimlProcessor
{
public:
	~StarProcessor() { }
	
	string getName() const {
		return "star";
	}
	string getVersion() const {
		return "1.0";
	}
	string process(Match *m, PElement e, Responder *, const string &) {
		string index = e->getAttribute("index");
		if (index.empty()) {
			index = "1";
		}
		//	replace all this crap with atoi .. which header tho?
		int realIndex = 0;
		for (unsigned int ix = 0; ix < index.length(); ++ix) {
			realIndex *= 10;
			realIndex += index[ix] - '0';
		}
		return m->getInputStar(realIndex);
	}
};

#endif
