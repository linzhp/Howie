/**
 * Substituter - This is fucked
 *
 * @author	Jonathan Roewen
 */
#include "Substituter.h"

#include "Utils.h"
#include "StringTokenizer.h"

#include <vector>
#include <iostream>

using namespace std;

map<string, SubMap> Substituter::maps;

vector<string> wordsOfTheSentence, resultingSentence;

Substituter::Substituter() {
	//	Init the output substitutions (internal entitities)
	addSubstitute("output", "&lt;", "<");
	addSubstitute("output", "&gt;", ">");
	addSubstitute("output", "&amp;", "&");
	addSubstitute("output", "&apos;", "'");
	addSubstitute("output", "&quot;", "\"");
}

string Substituter::replace(SubMap m, const string &input) {
	SubMap::const_iterator itr = m.begin();
	string result = input;
	StringTokenizer st(result, " ", true);
	wordsOfTheSentence.clear();
	resultingSentence.clear();
	while (st.hasMoreTokens()) {
		string word = " " + st.nextToken() + " ";	//	?
		wordsOfTheSentence.push_back(word);
		resultingSentence.push_back(word);
	}
	while (itr != m.end()) {
		string key = (*itr).first;
		result = Substituter::replace(key, (*m.find(key)).second, result);
		*itr++;
	}
	string outresult;
	for (unsigned int x = 0; x < resultingSentence.size(); ++x) {
		outresult += " " + trim(resultingSentence[x], " ");
	}
	if (outresult.empty()) {
		return " ";
	}
	return outresult;
}

//	This may screw it up .. want something more intelligent ..
//	right now, it's uppercasing word to find, and lowercasing word replacing it
string Substituter::replace(const string &substx, const string &substy, const string &norm) {
	unsigned int ix = 0, index;
	if (substx.empty()) {
		return norm;
	}
	for ( ; ix < wordsOfTheSentence.size(); ++ix) {
		int len = substx.length();
		if ((index = toUpper(wordsOfTheSentence[ix]).find(toUpper(substx))) != string::npos) {
			string head = wordsOfTheSentence[ix].substr(0, index);
			string tail = wordsOfTheSentence[ix].substr(index + len);
			resultingSentence[ix] = head + toLower(substy) + tail;
			wordsOfTheSentence[ix] = "";
		}
	}
	return norm;
}

//	Breaking normalize! Teehee
string Substituter::normalize(const string &input) {
	string result = input;
	if (!result.empty()) {
	//	result = toUpper(result);
		result = Substituter::replace(getSubstituter("normal"), " " + result).substr(1);
		StringTokenizer st(result, " ");
		result = st.nextToken();
		while (st.hasMoreTokens()) {
			result += " " + trim(st.nextToken(), " ");
		}
		return result;
	} else {
		return "*";
	}
}

string Substituter::person(const string &input) {
	string result = Substituter::replace(getSubstituter("person"), " " + input).substr(1);
	if (!result.empty()) {
		StringTokenizer st(result, " ");
		string str = st.nextToken();
		while (st.hasMoreTokens()) {
			str += " " + trim(st.nextToken(), " ");
		}
		return str;
	} else {
		return "";
	}
}

string Substituter::substitute(const string &input, const string &subs) {
	string result = Substituter::replace(getSubstituter(subs), " " + input).substr(1);
	if (!result.empty()) {
		StringTokenizer st(result, " ");
		string str = st.nextToken();
		while (st.hasMoreTokens()) {
			str += " " + trim(st.nextToken(), " ");
		}
		return str;
	} else {
		return "";
	}
}

string Substituter::output(const string &input) {
//	return Substituter::replace(getSubstituter("output"), " " + input).substr(1);
	string result = Substituter::replace(getSubstituter("output"), " " + input).substr(1);
	if (!result.empty()) {
		StringTokenizer st(result, " ");
		string str = st.nextToken();
		while (st.hasMoreTokens()) {
			str += " " + trim(st.nextToken(), " ");
		}
		return str;
	} else {
		return "";
	}
}

SubMap Substituter::getSubstituter(const string &name) {
	if (maps.find(name) == maps.end()) {
		maps[name] = SubMap();
	}
	return (*maps.find(name)).second;
}

void Substituter::addSubstitute(const string &name, const string &find, const string &replace) {
	SubMap m = Substituter::getSubstituter(name);
	m[find] = replace;
	maps[name] = m;
}
