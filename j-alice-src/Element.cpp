/**
 * Element - An XML Element
 *
 * @author	Jonathan Roewen
 */
#include "Element.h"
#include "Utils.h"
#include "Match.h"
#include "Memory.h"

#include <iostream>
#include <string.h>

using namespace std;

Element::Element(const string &name = ""):text(""),tname(""),nspace("") {
	if (!name.empty()) {
		setName(name);
	}
	child = next = last = NULL;
}

Element::~Element() {
}

string Element::getTagname() const {
	return tname;
}

string Element::getNamespace() const {
	return nspace;
}

string Element::getText(bool resolve) const {
	if (!resolve) {
		return text;
	} else {
		return resolveEntities(text);
	}
}
//	add bool
string Element::getAttribute(const string &key) const {
	map<string, string>::const_iterator itr = attributes.find(key);
	if (itr == attributes.end()) {
		return "";
	}
	return resolveEntities((*itr).second);
}
//	add bool
string Element::getAttribute(const string &key, Match *m, const string &id) {
	map<string, string>::const_iterator itr = attributes.find(key);
	if (itr == attributes.end()) {
		return "";
	}
	return resolveEntities(parse((*itr).second, m, id));
}

void Element::setName(const string &str) {
	string::size_type ix = str.find(':');
	if (ix != string::npos) {
		setNamespace(str.substr(0, ix));
		setTagname(str.substr(ix + 1));
	} else {
		setTagname(str);
		setNamespace("");
	}
}

void Element::setTagname(const string &str) {
	tname = get_string(str);
}

void Element::setNamespace(const string &str) {
	nspace = get_string(str);
}

void Element::setText(const string &str) {
	text = str;
}

void Element::setAttribute(const string &key, const string &value) {
	attributes[key] = value;
}

bool Element::hasNamespace() const {
	return strlen(nspace)!=0;
}

bool Element::hasAttributes() const {
	return !attributes.empty();
}

bool Element::hasChildren() const {
	return !(child == NULL);
}

PElement Element::getChild() const {
	return child;
}

PElement Element::getChild(const string &tagname) const {
	PElement ch = child;
	while (ch != NULL) {
		if (ch->tname == tagname) {
			return ch;
		}
		ch = ch->next;
	}
	return PElement(NULL);
}

PElement Element::getNextSibling() const {
	return next;
}

void Element::addChild(PElement element) {
	if (child == NULL) {
		child = element;
		last = child;
	} else {
		last->next = element;
		last = last->next;
	}
}

void Element::getChildren(velement *ve) {
	PElement ch = child;
	while (ch != NULL) {
		ve->push_back(ch);
	}
}

void Element::getChildren(const string &tagname, velement *ve) {
	PElement ch = child;
	while (ch != NULL) {
		if (ch->tname == tagname) {
			ve->push_back(ch);
		}
		ch = ch->next;
	}
}

map<string, string> Element::getAttributes() const {
	return attributes;
}

string Element::resolveEntities(const string &input) {
	string output = input;
	int ix = output.find("&");
	int iy = output.find(";", ix);
	
	while (ix != string::npos && iy != string::npos) {
		string head = output.substr(0, ix);
		string tail = output.substr(iy + 1);
		string mid = output.substr(ix, iy + 1 - ix);
		if (mid == "&lt;") {
			output = head + "<" + tail;
			iy -= 3;
		}
		else if (mid == "&gt;") {
			output = head + ">" + tail;
			iy -= 3;
		}
		else if (mid == "&amp;") {
			output = head + "&" + tail;
			iy -= 4;
		}
		else if (mid == "&quot;") {
			output = head + "\"" + tail;
			iy -= 5;
		}
		else if (mid == "&apos;") {
			output = head + "'" + tail;
			iy -= 5;
		}
		else {
			iy = ix + 1;
		}
		ix = output.find("&", iy);
		iy = output.find(";", ix);
	}
	return output;
}

string Element::parse(const string &s, Match *m, const string &id) {
	string result = "";
	int iy = 0;
	for (int ix = 0; ix < s.length(); ++ix) {
		char ch = s[ix];
		if (ch == 's' && ix + 5 < s.length()) {
			if (s.find("star(", ix) == ix) {
				ix += 5;
				string star = s.substr(ix, (iy = s.find(")", ix)) - ix);
				result += toLower(m->getInputStar(atoi(star.c_str())));
				ix = iy;
				continue;
			}
		}
		else if (ch == 't' && ix + 9 < s.length()) {
			if (s.find("thatstar(", ix) == ix) {
				ix += 9;
				string thatstar = s.substr(ix, (iy = s.find(")", ix)) - ix);
				result += toLower(m->getThatStar(atoi(thatstar.c_str())));
				ix = iy;
				continue;
			}
		}
		else if (ch == 't' && ix + 10 < s.length()) {
			if (s.find("topicstar(", ix) == ix) {
				ix += 10;
				string topicstar = s.substr(ix, (iy = s.find(")", ix)) - ix);
				result += toLower(m->getTopicStar(atoi(topicstar.c_str())));
				ix = iy;
				continue;
			}
		}
		else if (ch == 'g' && ix + 4 < s.length()) {
			if (s.find("get(", ix) == ix) {
				ix += 4;
				string get = s.substr(ix, (iy = s.find(")", ix)) - ix);
				result += toLower(Memory::getValue(get, id));
				ix = iy;
				continue;
			}
		}
		result += ch;
	}
	return unslash(result);
}

string Element::unslash(const string &input) {
	string output = "";
	int ix = 0;
	while (ix < input.length()) {
		if (input[ix] == '\\') {
			++ix;
		}
		if (ix >= input.length()) {
			break;
		}
		output += input[ix];
		++ix;
	}
	return output;
}