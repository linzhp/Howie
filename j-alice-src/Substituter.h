/**
 * Substituter - Performs substitutions :P
 *
 * @author	Jonathan Roewen
 */
#ifndef SUBSTITUTER_H
#define SUBSTITUTER_H

#include <map>
#include <string>

using namespace std;

typedef map<string, string> SubMap;

class Substituter
{
public:
	static map<string, SubMap> maps;
	
	Substituter();
	
	static string replace(SubMap, const string &);
	static string replace(const string &, const string &, const string &);
	static string normalize(const string &);
	static string person(const string &);
	static string substitute(const string &, const string &);
	static string output(const string &);
	static SubMap getSubstituter(const string &);
	static void addSubstitute(const string &, const string &, const string &);
};

#endif 
