/**
 * Parser - Parses an input stream into tree representation
 * of corresponding XML
 *
 * @author	Jonathan Roewen
 */
#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <iostream>
#include <stack>

using namespace std;

#include "Element.h"

class Parser
{
public:
	Parser() {
		root = current = PElement(NULL);
	}
	
	PElement parse(istream &in, const string &xxx);
	PElement getRoot() const;
	void reset();
private:
	PElement current, root;
	stack<PElement> recurser;
	
	void addElement(string &name);
	void addTextElement(const string &text);
	void addCDataElement(const string &text);
};

#endif 
