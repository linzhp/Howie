/**
 * AimlProcessor - ABC for all processors (of tags)
 *
 * @author	Jonathan Roewen
 */
#ifndef AIML_PROCESSOR_H
#define AIML_PROCESSOR_H

#include <string>

#include "Element.h"
#include "Match.h"
#include "Responder.h"

using namespace std;

//class Match;
//class Responder;
	
class AimlProcessor
{
public:
	virtual ~AimlProcessor() { };
	
	virtual string getName() const = 0;
	virtual string getVersion() const = 0;
	virtual string process(Match *, PElement, Responder *, const string &) = 0;
};

#endif
