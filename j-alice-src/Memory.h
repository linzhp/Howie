/**
 * Memory - Facilitates storing predicates and stuff
 *
 * @author	Jonathan Roewen
 */
#ifndef MEMORY_H
#define MEMORY_H

#include <string>

using namespace std;

#include "Database.h"

class Memory
{
public:
	Memory() { }
	
	static Database db;
	
	static void addValue(const string &, const string &, const string &);
	static void removeValue(const string &, const string &, const string &, bool);
	static void setValue(const string &, const string &, const string &);
	static string getValue(const string &, const string &);
	static string getValue(const string &, const string &, const string &);
	static void push(const string &, const string &, const string &);
	static string pop(const string &, const string &, int);
	static string getThat(const string &);
};

#endif  
