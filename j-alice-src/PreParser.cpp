/**
 * PreParser - Is a SAX Parser used to pre-parse AIML files.
 *
 * Need to change currentTag to be a stack! Then we can see what the last tag
 * was, to make all this code simpler, and much more robust :-)
 *
 * @author	Taras Glek, Jonathan Roewen
 */
#include "PreParser.h"

#include "Utils.h"

#include "StringTokenizer.h"
#include "Kernel.h"
#include "Template.h"
#include "Nodemaster.h"

#include <iostream>

extern int totalCategories;

int templateDepth = 0;

void PreParser::elementStarted(const string &name, int index) {
	elementStarted(name, NULL, index);
}

//	Index points to position in file that we are at
//	This _should_ be the character after the '>' of the opening tag...
void PreParser::elementStarted(const string &name, map<string, string> *attr, int index) {
	if (name == "topic") {
		if (attr != NULL) {
			map<string, string>::const_iterator itr = attr->find("name");
			if (itr != attr->end()) {
				topic = (*itr).second;
			}
		}
	} else if (name == "pattern" && templateDepth == 0) {
		pattern = "";
	} else if (name == "template") {
		if (++templateDepth == 1) {
			templateBegin = index;	//	right?
		}
	} else if (name == "that") {
		//	This may not work until we use a stack for currentTag
		if (that == "*") {
			that = "";
		}
	} else if (name == "bot") {
		if (currentTag == "pattern") {
			if (attr != NULL) {
				map<string, string>::const_iterator itr = attr->find("name");
				if (itr != attr->end()) {
					string value = (*itr).second;
					if (value == "name") {
						pattern += "j-alice";
					} else {
						//	Avoid double space..
						pattern = trim(pattern);
						if (!pattern.empty()) {
							pattern += " ";
						}
					}
				}
			}
		}
	} else if (name == "name") {
		if (currentTag == "pattern") {
			pattern += "j-alice";
		}
	} else {
		return;
	}
	currentTag = name;
}

//	Index points to position in file that we are at
//	This _should_ be the character after the '>' of the closing tag...
void PreParser::elementClosed(const string &name, int index) {
	if (name == "topic") {
		topic = "*";
	} else if (name == "pattern") {
		;
	} else if (name == "template" && --templateDepth == 0) {
		int len = (index - templateBegin) - 11;	//	11 or 10?
		//	"</template>" has length: 11
		
		Nodemaster *node = Kernel::add(pattern, that, topic);
		that = "*";
		if (node != NULL) {
			node->addTemplate(new Template(filename, templateBegin, (index - 11)));
			++totalCategories;	//	Right?
		}
		templateBegin = -1;
	} else if (name == "that") {
		if (that.empty()) {
			that = "*";
		}
	} else {
		return;
	}
	currentTag.erase();
}

//	Index would be one character after the end of the text string in the file
void PreParser::elementText(const string &text, int) {
	if (currentTag == "pattern" && templateDepth == 0) {
		pattern += text;
	} else if (currentTag == "that") {
		if (that.empty()) {
			that = text;
		}
	}
}

void PreParser::elementCData(const string &text, int) {
	;
}

//	Much of this code is from Parser.cpp
void PreParser::parse(istream &in, const string &filename) {
	char buf;
	string buffer;
	this->filename = filename;
	
	while (in.get(buf)) {
		//	char-by-char parsing .. quite effective and simple
		if (buf == '\n' || buf == '\t' || buf == '\r' || buf == '\0') {
			continue;
		}
		buffer += buf;
		if (buf == '-' && buffer == "<!--") {
			//	Then skip comment
			while (in.get(buf)) {
				buffer += buf;
				if (buffer.find("-->") == buffer.length() - 4) {
					buffer.erase();
					break;
				}
			}
			continue;
		}
		if ((buf == 'l' && buffer == "<?xml") ||
				(buf == 'E' && buffer == "<!DOCTYPE")) {
			//	Then skip doctype or xml declaration
			while (in.get(buf) && buf != '>') {
				;
			}
			buffer.erase();
			continue;
		}
		if (buf == '[' && buffer == "<![CDATA[") {
			//	Then CDATA section has begun
			string contents = "";
			while (in.get(buf)) {
				buffer += buf;
				contents += buf;
				if (buf == '>' && buffer.find("]]>") == buffer.length() - 3) {
					//	The CDATA section has finished
					contents = contents.substr(0, contents.length() - 3);
					elementCData(contents, in.tellg());
					break;
				}
			}
			buffer.erase();
			continue;
		}
		if (buf == '>' && buffer.find("<") == 0 &&
				buffer.find(">") == buffer.length() - 1) {
			//	Then a opening of [self] closing tag
			addElement(buffer, in.tellg());
			buffer.erase();
			continue;
		}
		if (buf == '<' && buffer.length() > 1 &&
				buffer.find("<") == buffer.length() - 1) {
			//	Then we have text, and a tag is starting
			elementText(buffer.substr(0, buffer.length() - 1), in.tellg());
			buffer.erase();
			buffer += "<";
			continue;
		}
	}
}

void PreParser::addElement(string &str, int index) {
	str = trim(str, "<>");
	string::size_type tagSeparator = str.find(" ");
	if (str[0] == '/') {
		//	Closing tag
		elementClosed(str.substr(1, str.length() - 1), index);
		return;
	}
	if (tagSeparator == string::npos) {
		//	No attributes for this tag
		if (str[str.length() - 1] == '/') {
			//	Self closing tag
			elementStarted(str.substr(0, str.length() - 1), index);
			elementClosed(str.substr(0, str.length() - 1), index);
			return;
		} else {
			//	Opening tag
			elementStarted(str, index);
			return;
		}
	} else {
		//	Attributes for this tag
		string tagName = str.substr(0, tagSeparator);
		
		//	Attribute handling to be added here...
		map<string, string> attributeMap;
		
		string attributes = trim(str.substr(tagSeparator), "</>");
		string name, value = name = "";
		StringTokenizer pairs(attributes, "=");
		while (pairs.hasMoreTokens()) {
			StringTokenizer pair(trim(pairs.nextToken(), "\""), "\"");
			if (name.empty()) {
				name = trim(pair.nextToken());
			} else {
				value = pair.nextToken();
				//hmm..
				attributeMap[name] = value;
				name = trim(pair.nextToken());
			}
		}
		//	End attribute handling
		elementStarted(tagName, &attributeMap, index);
		
		if (str[str.length() - 1] == '/') {
			//	Self closing tag
			elementClosed(tagName, index);
			return;
		} else {
			//	Opening tag....
			return;
		}
	}
}