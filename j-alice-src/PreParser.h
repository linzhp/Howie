/**
 * PreParser - Is a SAX Parser used to pre-parse AIML files.
 *
 * @author	Taras Glek, Jonathan Roewen
 */
#ifndef PRE_PARSER_H
#define PRE_PARSER_H

#include <string>
#include <iostream>
#include <map>

using namespace std;

class PreParser
{
public:
	PreParser() {
		topic = "*";
		that = "*";
		pattern = "";
		templateBegin = -1;
	}
	
	void elementStarted(const string &, int);
	void elementClosed(const string &, int);
	void elementStarted(const string &, map<string, string> *, int);
	void elementText(const string &, int);
	void elementCData(const string &, int);
	void parse(istream &, const string &);
	void addElement(string &, int);
	
private:
	string pattern, topic, that, currentTag, filename;
	int templateBegin;
};

#endif