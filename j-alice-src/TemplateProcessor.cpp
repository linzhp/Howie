/**
 * TemplateProcessor - The big dude on campus
 *
 * @author	Jonathan Roewen
 */
#include "TemplateProcessor.h"
#include "Responder.h"
#include "Handler.h"
#include <iostream>

using namespace std;

AimlProcessor *TemplateProcessor::hook = new TemplateProcessor();

map<string, AimlProcessor *> TemplateProcessor::processors;

TemplateProcessor::~TemplateProcessor() {
	map<string, AimlProcessor *>::iterator itr = processors.begin();
	while (itr != processors.end()) {
		delete (*itr).second;
		*itr++;
	}
}

string TemplateProcessor::getName() const {
	return "template";
}

string TemplateProcessor::getVersion() const {
	return "1.0";
}

string TemplateProcessor::process(Match *m, PElement e, Responder *r, const string &id) {
	string buffer = "";
	if (e == NULL) {
		return buffer;
	}
	try {
		PElement child = e->getChild();
		while (child != NULL) {
			if (child->getTagname() == "#text") {
				buffer += child->getText();
			} else if (child->getTagname() == "#cdata") {
				buffer += child->getText(false);
			} else {
//				child = checkCompatability(child);

/////////////// BEGIN NIGHTMARE /////////////////////////////////

				if (child->hasNamespace()) {
					//	Then we need to use the responder
					if (r == NULL) {
						//	This is temporary!!!!!
					} else {
						buffer += r->respond(m, child, id);
					}
				} else {
				

/////////////// END NIGHTMARE CODE //////////////////////////////
/*				string tagname = child->getTagname();
				if (child->hasNamespace()) {
					tagname = child->getNamespace();
					//	Temporary hack until responders put in
				}	*/
					AimlProcessor *p = TemplateProcessor::getProcessor(child->getTagname());
					if (p == NULL) {
						//	Then nothing to process this tag with
						buffer += process(m, child, r, id);
						//	is that right?
					} else {
						buffer += p->process(m, child, r, id);
					}
				}
			}
			child = child->getNextSibling();
		}
	} catch (int &ex) {
	}
	return buffer;
}

void TemplateProcessor::addProcessor(AimlProcessor *p) {
	processors[p->getName()] = p;
}

AimlProcessor *TemplateProcessor::getProcessor(const string &name) {
	if (processors.find(name) == processors.end()) {
		if(Handler::hasProcessor(name))
			addProcessor((*Handler::processors.find(name)).second);
		else
			return NULL;
	}
	return (*processors.find(name)).second;
}

string TemplateProcessor::processTemplate(Match *m, PElement e, Responder *r, const string &id) {
	string temp = hook->process(m, e, r, id);
	return temp;
}
