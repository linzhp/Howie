/**
 * RandomProcessor - Selects randomly between list of choices
 *
 * @author	Jonathan Roewen
 */
#ifndef RANDOM_PROCESSOR_H
#define RANDOM_PROCESSOR_H

#include "AimlProcessor.h"
#include "Kernel.h"

#include <string>

using namespace std;

#include <time.h>
#include <stdlib.h>

class RandomProcessor : public AimlProcessor
{
public:
	RandomProcessor() {
		srand(time(NULL));
	}
	~RandomProcessor() { }
	
	string getName() const {
		return "random";
	}
	string getVersion() const {
		return "1.0";
	}
	string process(Match *m, PElement e, Responder *r, const string &id) {
		velement choices;
		e->getChildren("li", &choices);
		int chosen = rand() % choices.size();
		velement_it ix;
		//I wonder if this code is correct :)
		int i=0;
		for(ix = choices.begin();i < chosen;i++)
			ix++;
		PElement ele = *ix;
		
		return Kernel::process(m, ele, r, id);
	}
};

#endif
