/**
 * Parser - blah
 *
 * @author	Jonathan Roewen
 */
#include "Parser.h"

#include "Utils.h"

#include "StringTokenizer.h"

PElement Parser::parse(istream &in, const string &) {
	root = current = new Element("doc");
	char buf;
	string buffer;
	
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
					addCDataElement(contents);
					break;
				}
			}
			buffer.erase();
			continue;
		}
		if (buf == '>' && buffer.find("<") == 0 &&
				buffer.find(">") == buffer.length() - 1) {
			//	Then a opening or [self] closing tag
			addElement(buffer);
			buffer.erase();
			continue;
		}
		if (buf == '<' && buffer.length() > 1 &&
				buffer.find("<") == buffer.length() - 1) {
			//	Then we have text, and a tag is starting
		//	if (!trim(buffer.substr(0, buffer.length() -1)).empty()) {
				addTextElement(buffer.substr(0, buffer.length() - 1));
		//	}
			buffer.erase();
			buffer += "<";
			continue;
		}
	}
	if (!buffer.empty()) {
		addTextElement(buffer);
	}
	return root->getChild();
}

PElement Parser::getRoot() const {
	return root->getChild();
}

void Parser::reset() {
	root = current = new Element("doc");
}

void Parser::addElement(string &str) {
	str = trim(str, "<>");
	string::size_type tagSeparator = str.find(" ");
	if (str[0] == '/') {
		//	Closing tag
		//if (current != PElement() && current->getParent() != PElement()) {
		if (current != NULL && recurser.size() > 1) {
		//	cout << "before going to parent" << endl;
			recurser.pop();
			current = recurser.top();
			
		//	cout << "after going to parent" << endl;
		} else {
		//	cout << ":" << current << " : " << current->getParent() << endl;
		}
		return;
	}
	if (tagSeparator == string::npos) {
		//	No attributes for this tag
		if (str[str.length() - 1] == '/') {
			//	Self closing tag
			PElement element(new Element(str.substr(0, str.length() - 1)));
			current->addChild(element);
			return;
		} else {
			//	Opening tag
			PElement element(new Element(str));
			current->addChild(element);
			current = element;
			recurser.push(current);
			return;
		}
	} else {
		//	Attributes for this tag
		string tagName = str.substr(0, tagSeparator);
		PElement element(new Element(tagName));
		string attributes = trim(str.substr(tagSeparator), "</>");
		string name, value = name = "";
		StringTokenizer pairs(attributes, "=");
		while (pairs.hasMoreTokens()) {
			StringTokenizer pair(trim(pairs.nextToken(), "\""), "\"");
			if (name.empty()) {
				name = trim(pair.nextToken());
			} else {
				value = pair.nextToken();
				element->setAttribute(name, value);
				name = trim(pair.nextToken());
			}
		}
		if (str[str.length() - 1] == '/') {
			//	Self closing tag
			current->addChild(element);
			return;
		} else {
			//	Opening tag
			current->addChild(element);
			current = element;
			recurser.push(current);
			return;
		}
	}
}

void Parser::addTextElement(const string &text) {
	PElement element(new Element("#text"));
	element->setText(text);
	current->addChild(element);
}

void Parser::addCDataElement(const string &cdata) {
	PElement element(new Element("#cdata"));
	element->setText(cdata);
	current->addChild(element);
}
