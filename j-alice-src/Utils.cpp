/**
 * Utils - blah blah blah
 *
 * @author	Jonathan Roewen
 */
#include "Utils.h"
#include <set>

#include <ctype.h>
#include <time.h>	//	Assume works on all platforms .. can fix later

typedef multiset<string> string_set ;
string_set stringset;


char *mystrdup(const char *str){
	if(!str)
		return NULL;
	char *ret = (char*)malloc(strlen(str)+1);
	strcpy(ret,str);
	return ret;
}

char *get_string(const string &str){
	string_set::iterator it;
	if((it = stringset.find(str)) == stringset.end())
		it = stringset.insert(str);
	return  (char *)(*it).c_str();
}

string trim(const string &text) {
	return trim(text, " \t\n\r");
}

string trim(const string &text, const string &remove) {
	string result = text;
	string::size_type index = result.find_first_not_of(remove);
	result = result.erase(0, index);
	index = result.find_last_not_of(remove);
	result = result.erase(index + 1);
	return result;
}

string toLower(const string &text) {
	string result = "";
	for (unsigned int ix = 0; ix < text.length(); ++ix) {
		result += tolower(text[ix]);
	}
	return result;
}

string toUpper(const string &text) {
	string result = "";
	for (unsigned int ix = 0; ix < text.length(); ++ix) {
		result += toupper(text[ix]);
	}
	return result;
}

string replace(const string &text, char a, char b) {
	string result = "";
	for (unsigned int ix = 0; ix < text.length(); ++ix) {
		if (text[ix] == a) {
			result += b;
		} else {
			result += text[ix];
		}
	}
	return result;
}

string remove(const string &text, char r) {
	string result = "";
	for (unsigned int ix = 0; ix < text.length(); ++ix) {
		if (text[ix] != r) {
			result += text[ix];
		}
	}
	return result;
}

long timerMillis() {
	clock_t time = clock();
	return (long)time;
}
