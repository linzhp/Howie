/**
 * Memory - Stores predicates about the user
 *
 * @author	Jonathan Roewen
 */
#include "Memory.h"

#include "Utils.h"
#include "StringTokenizer.h"

#include <iostream> //?

using namespace std;

Database Memory::db;

void Memory::addValue(const string &property, const string &id, const string &value) {
	try {
		db.Insert(id, "properties", new Record("", "", "", "", "", property, value));
		db.save();
	} catch (DatabaseException &de) {
		cerr << de.getException() << endl;
	}
}

void Memory::removeValue(const string &property, const string &id, const string &value, bool removeAll) {
	try {
		if (removeAll) {
			bool *where = new bool[7];
			where[0] = where[1] = where[2] = where[3] = where[4] = where[6] = false; where[5] = true;
			db.Delete(id, "properties", where, new Record("", "", "", "", "", property, ""));
		} else {
			bool *where = new bool[7];
			where[0] = where[1] = where[2] = where[3] = where[4] = false; where[5] = where[6] = true;
			db.Delete(id, "properties", where, new Record("", "", "", "", "", property, value));
		}
		db.save();
	} catch (DatabaseException &de) {
		cerr << de.getException() << endl;
	}
}

void Memory::setValue(const string &property, const string &id, const string &value) {
	string val = value;
	if (property == "that") {
		val = getThat(val);
	}
	
	try {
		bool *get = new bool[7];
		get[0] = get[1] = get[2] = get[3] = get[4] = get[5] = false; get[6] = true;
		bool *where = new bool[7];
		where[0] = where[1] = where[2] = where[3] = where[4] = where[6] = false; where[5] = true;
		Record **result = db.Select(id, get, "properties", where, new Record("", "", "", "", "", property, ""));
		int count = 0;
		while (result[count] != NULL) {
			count++;
		}
		if (count > 0) {
			db.Update(id, "properties", get, new Record("", "", "", "", "", "", value), where, new Record("", "", "", "", "", property, ""));
		} else {
			db.Insert(id, "properties", new Record("", "", "", "", "", property, value));
		}
		db.save();
	} catch (DatabaseException &de) {
		cerr << de.getException() << endl;
	}
}

string Memory::getValue(const string &property, const string &id) {
	return getValue(property, id, " ");
}

string Memory::getValue(const string &property, const string &id, const string &conjunction) {
	string rs = "", fix = trim(id);
	try {
		bool *get = new bool[7];
		get[0] = get[1] = get[2] = get[3] = get[4] = get[5] = false; get[6] = true;
		bool *where = new bool[7];
		where[0] = where[1] = where[2] = where[3] = where[4] = where[6] = false; where[5] = true;
		Record **result = db.Select(fix, get, "properties", where, new Record("", "", "", "", "", property, ""));
		int count = 0;
		while (result[count] != NULL) {
			rs += " " + conjunction;
			rs += result[count]->data[6];
			rs = trim(rs);
			++count;
		}
	} catch (DatabaseException &de) {
		cerr << de.getException() << endl;
	}
	return rs;
}

void Memory::push(const string &name, const string &value, const string &id) {
	try {
		string v1, v2, v3, v4, v5;
		v1 = v2 = v3 = v4 = v5 = "";
		bool *get = new bool[7];
		get[0] = get[1] = get[2] = get[3] = get[4] = true; get[5] = get[6] = false;
		bool *where = new bool[7];
		where[0] = where[1] = where[2] = where[3] = where[4] = where[5] = where[6] = false;
		Record **result = db.Select(id, get, name, where, new Record("", "", "", "", "", "", ""));
		int count = 0;
		while (result[count] != NULL) {
			v1 = result[count]->data[0];
			v2 = result[count]->data[1];
			v3 = result[count]->data[2];
			v4 = result[count]->data[3];
			++count;
		}
		db.Delete(id, name, where, new Record("", "", "", "", "", "", ""));
		db.Insert(id, name, new Record(value, v1, v2, v3, v4, "" ,""));
		db.save();
	} catch (DatabaseException &de) {
		cerr << de.getException() << endl;
	}
}

string Memory::pop(const string &name, const string &id, int index) {
	string response = "";
	if (index > 5) {
		return response;
	}
	try {
		bool *get = new bool[7];
		get[0] = get[1] = get[2] = get[3] = get[4] = true; get[5] = get[6] = false;
		bool *where = new bool[7];
		where[0] = where[1] = where[2] = where[3] = where[4] = where[5] = where[6] = false;
		Record **result = db.Select(id, get, name, where, new Record("", "", "", "", "", "", ""));
		int count = 0;
		while (result[count] != NULL) {
			response = result[count]->data[index - 1];
			++count;
		}
	} catch (DatabaseException &de) {
		cerr << de.getException() << endl;
	}
	return response;
}

string Memory::getThat(const string &sentence) {
	string that = "";
	string last = sentence;
	StringTokenizer st(sentence, ".?!");
	while (st.hasMoreTokens()) {
		last = st.nextToken();
		if (!last.empty()) {
			that = trim(last);
		}
	}
	return that;
}
