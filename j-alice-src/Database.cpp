/**
 * Database - A cheesy hack!
 *
 * @author	Jonathan Roewen
 */
#include "Database.h"

#include <fstream>
#include <iostream>
#include <cstring>

using namespace std;

const int PROPERTY = 5;
const int VALUE = 6;

string removeNewlines(const string &input) {
	string output = "";
	for (int ix = 0; ix < input.length(); ++ix) {
		//	Just ignore newlines? or replace with a space?
		if (input[ix] != '\n') {
			output += input[ix];
		} else {
			output += " ";
		}
	}
	return output;
}

Database::Database() {
	load();/*
	d["properties"] = table();
	d["input"] = table();*/
}

void Database::load() {
	ifstream fin("db.index");
	if (!fin.is_open()) {
		d["properties"] = table();
		d["input"] = table();
	} else {
		while (fin.good()) {
			string table, line;
			getline(fin, table);
			ifstream i_table((table + ".csv").c_str());
			if (!i_table.is_open()) {
				continue;
			}
			getline(i_table, line);
			while (i_table.good()) {
				string record, id, seven[7];
				getline(i_table, record, ',');
				getline(i_table, id, ',');
				if (!i_table.good()) {
					break;
				}
				for (int ix = 0; ix < 7; ++ix) {
					if (ix != 6) {
						getline(i_table, seven[ix], ',');
					} else {
						getline(i_table, seven[ix]);
					}
				}
				Insert(record, table, new Record(seven[0], seven[1], seven[2], seven[3], seven[4], seven[5], seven[6]));
			}
		}
	}
}

void Database::save() {
	string tables;
	ofstream fout("db.index");
	for (database::iterator it_d = d.begin(); it_d != d.end(); it_d++) {
		table &t = it_d->second;
		string table_name = it_d->first;
		ofstream of((table_name + ".csv").c_str());
		of << "record,id,0,1,2,3,4,5,6" << endl;
		fout << table_name << endl;
		
		for (table::iterator it_t = t.begin(); it_t != t.end(); it_t++) {
			const char *record_name = it_t->first.c_str();
			record &r = it_t->second;
			for (record::iterator it_r = r.begin(); it_r != r.end(); it_r++) {
				int id = it_r->first;
				if (!(it_r->second)) {
					continue;
				}
				Record &r = *(it_r->second);
				of << record_name << "," << id << "," << removeNewlines(r.data[0]) << "," << removeNewlines(r.data[1]) << "," << removeNewlines(r.data[2]) << "," << removeNewlines(r.data[3]) << "," << removeNewlines(r.data[4]) << "," << removeNewlines(r.data[5]) << "," << removeNewlines(r.data[6]) << endl;
			}
		}
	}
}

Record **Database::Select(const string &id, bool *get, const string &from, bool *where, Record *values) {
	if (d.find(from) == d.end()) {
		Record **rset = new Record*[1];
		rset[0] = NULL;
		d[from] = table();
		return rset;
	}
	
	unsigned int ix=0, iy=0, iz=0;
	table &workTable = (*d.find(from)).second;
	
	if (workTable.find(id) == workTable.end()) {
		Record **rs = new Record*[1];
		rs[0] = NULL;
		return rs;
	}
	
	record &workRecord = (*workTable.find(id)).second;
	
	Record **resultSet = new Record*[workRecord.size() + 1];
	int count = -1;
	
	for (iz = 0; iz < workRecord.size(); ++iz) {
		if (workRecord[iz] == NULL) {
			goto outerSelect;
		}
		
		for (ix = 0; ix < 7; ++ix) {
			if (where[ix] && values->data[ix] != (*workRecord.find(iz)).second->data[ix]) {
				goto outerSelect;
			}
		}
		++count;
		resultSet[count] = new Record();
		for (iy = 0; iy < 7; ++iy) {
			if (get[iy]) {
				resultSet[count]->data[iy] = (*workRecord.find(iz)).second->data[iy];
			}
		}
outerSelect:
		;
	}
	unsigned int Count = count + 1;
	for ( ; Count <= workRecord.size(); ++Count) {
		resultSet[Count] = NULL;
	}
	return resultSet;
}

void Database::Insert(const string &id, const string &into, Record *row) {
	if (d.find(into) == d.end()) {
		d[into] = table();
	}
	
	table &workTable = (*d.find(into)).second;
	
	if (workTable.find(id) == workTable.end()) {
		record r; r[0] = row;
		workTable[id] = r;
		d[into] = workTable;
		return;
	}
	record &rec = (*workTable.find(id)).second;
	rec[rec.size()] = row;
	workTable[id] = rec;
	d[into] = workTable;
//	save();
}

void Database::Delete(const string &id, const string &from, bool *where, Record *values) {
	if (d.find(from) == d.end()) {
		throw(DatabaseException("Delete: no table (" + from + ")"));
	}
	
	table &workTable = (*d.find(from)).second;
	if (workTable.find(id) == workTable.end()) {
		return;
	}
	
	record &workRecord = (*workTable.find(id)).second;
	
	for (unsigned int ix = 0; ix < workRecord.size(); ++ix) {
		for (unsigned int iy = 0; iy < 7; ++iy) {
			if (where[iy] && values->data[iy] != (*workRecord.find(ix)).second->data[iy]) {
				goto outerDelete;
			}
		}
		workRecord[ix] = NULL;
outerDelete:
		;
	}
}

void Database::Update(const string &id, const string &name, bool *set, Record *to, bool *where, Record *values) {
	if (d.find(name) == d.end()) {
		throw(DatabaseException("Update: no table (" + name + ")"));
	}
	
	unsigned int iz = 0;
	table &workTable = (*d.find(name)).second;
	if (workTable.find(id) == workTable.end()) {
		throw(DatabaseException("Update: no id (" + id + ")"));
	}
	
	record &workRecord = (*workTable.find(id)).second;
	
	for (unsigned int ix = 0; ix < workRecord.size(); ++ix) {
		for (unsigned int iy = 0; iy < 7; ++iy) {
			if (where[iy] && values->data[iy] != (*workRecord.find(ix)).second->data[iy]) {
				goto outerUpdate;
			}
		}
		for (iz = 0; iz < 7; ++iz) {
			if (set[iz]) {
				workRecord[ix]->data[iz] = to->data[iz];
			}
		}
outerUpdate:
		;
	}
}

Record::Record() {
	data = new string[7];
}

Record::Record(const string &f1, const string &f2, const string &f3, const string &f4, const string &f5, const string &prop, const string &val) {
	data = new string[7];
	data[0] = f1;
	data[1] = f2;
	data[2] = f3;
	data[3] = f4;
	data[4] = f5;
	data[PROPERTY] = prop;
	data[VALUE] = val;
}