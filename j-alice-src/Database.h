/**
 * Database - A class to emulate a database we will use later
 *
 * @author	Jonathan Roewen
 */
#ifndef DATABASE_H
#define DATABASE_H

#include <map>
#include <string>

using namespace std;

class Record;

typedef map<int, Record *> record;
typedef map<string, record> table;
typedef map<string, table> database;

class DatabaseException
{
public:
	DatabaseException(const string &s) : exception(s) { }
	
	string getException() const {
		return exception;
	}
	
	string exception;
};

class Database
{
public:
	Database();
	
	database d;
	
	void Insert(const string &, const string &, Record *);
	void Delete(const string &, const string &, bool *, Record *);
	void Update(const string &, const string &, bool *, Record *, bool *, Record *);
	Record **Select(const string &, bool *, const string &, bool *, Record *);
//private:
	void save();
	void load();
};

class Record
{
public:
	string *data;
	
	Record(const string &, const string &, const string &, const string &, const string &, const string &, const string &);
	Record();
};

#endif  
