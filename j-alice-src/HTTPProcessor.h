#ifndef HTTPPRCCESSOR_H
#define HTTPPRCCESSOR_H

#include <string>

#include "AimlProcessor.h"

class HTTPProcessor : public AimlProcessor{
public:
	string getName() const{ return "http";}
	string getVersion() const { return "0.1";}
	string process(Match *, PElement, Responder *, const string &);

};

#endif //HTTPPRCCESSOR_H

