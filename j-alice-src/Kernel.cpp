/**
 * Kernel - Brains of the operation
 *
 * @author	Jonathan Roewen
 */
#include "Kernel.h"

#include <fstream>
#include <iostream>
//#ifdef __BEOS__
// #include <sstream>
// #define stringstream strstream
//#else
 #include <strstream>
//#endif

#include "Match.h"
#include "Utils.h"
#include "Memory.h"
#include "Parser.h"
#include "Handler.h"
#include "Responder.h"
#include "Nodemaster.h"
#include "Substituter.h"
#include "StringTokenizer.h"
#include "TemplateProcessor.h"
#include "PreParser.h"
#include "AimlWriter.h"

int totalCategories = 0;
long totalSize = 0;
long totalNodes = 0;

const int Kernel::INPUT = 0;
const int Kernel::THAT = 1;
const int Kernel::TOPIC = 2;

const string Kernel::constants = " <that> <topic> ";

static bool trimming = true;

string toString(PElement element) {
	if (element == NULL) {
		return "*";
	}
	string result;
	PElement child = element->getChild();
	while (child != NULL) {
		if (child->getTagname() == "#text") {
			//	Then this child only contains text, no markup
			result += child->getText();
		} else if (child->getTagname() == "#cdata") {
			;
		} else {
			//	Since only using on <that> and <pattern>
			//	The tag can only be <bot_xxx/> or <bot name="xxx"/> or <name/>
			if (child->getTagname() == "name") {
				result += Kernel::respond("BOT NAME", "unknown");
			} else
			if (child->getTagname() == "bot") {
				//	Is a <bot name="xxx"/> predicate
				result += Kernel::respond("BOT " + toUpper(child->getAttribute("name")), "unknown");
			} else {
				//	It is old-skool <bot_xxx/>
				result += Kernel::respond("BOT " + toUpper(child->getTagname().substr(4)), "unknown");
			}
		}
		child = child->getNextSibling();
	}
	return trim(toUpper(result));//Substituter::normalize(result);
}

string templateString(PElement element) {
	if (element == NULL) {
		return "";
	}
	string result;
	PElement child = element->getChild();
	while (child != NULL) {
		if (child->getTagname() == "#text") {
			result += child->getText();
		} else if (child->getTagname() == "#cdata") {
			result += child->getText(false);
		} else {
			string tag = "<";
			if (child->hasNamespace()) {
				tag += child->getNamespace() + ":";
			}
			tag += child->getTagname();
			if (child->hasAttributes()) {
				map<string, string> attr = child->getAttributes();
				map<string, string>::const_iterator itr = attr.begin();
				while (itr != attr.end()) {
					tag += " " + (*itr).first + "=\"" + (*itr).second + "\"";
					itr++;
				}
			}
			if (child->hasChildren()) {
				tag += ">" + templateString(child);
				tag += "</";
				if (child->hasNamespace()) {
					tag += child->getNamespace() + ":";
				}
				tag += child->getTagname() + ">";
			} else {
				tag += "/>";
			}
			result += tag;
		}
		child = child->getNextSibling();
	}
	return result;
}

void prettyPrintHeader(const string &filename, int size, int totalSize, long time) {
	string fn = filename;
	string::size_type f = fn.find_last_of("\\/:");
	if (f != string::npos) {
		fn = fn.substr(f + 1);
	}
	cout << "Learnt: " << fn;
	if (fn.length() >= 20) {
		cout << endl << "                            ";
	} else {
		for (int ix = 0; ix < 20 - fn.length(); ++ix) {
			cout << " ";
		}
	}
	int len = 0;
	for (int iy = size; iy > 0 && len < 6; iy = iy / 10) {
		++len;
	}
	for (int ix = 0; ix < 6 - len; ++ix) {
		cout << " ";
	}
	cout << size << " categories";
	len = 0;
	for (int iy = totalSize; iy > 0 && len < 6; iy = iy / 10) {
		++len;
	}
	for (int ix = 0; ix < 10 - len; ++ix) {
		cout << " ";
	}
	cout << totalSize << " total";
	len = 0;
	for (int iy = (int)time; iy > 0 && len < 6; iy = iy / 10) {
		++len;
	}
	for (int ix = 0; ix < 6 - len; ++ix) {
		cout << " ";
	}
	if (len != 0) {
		cout << " ";
	}
	cout << time << "ms" << endl;
}

Nodemaster *Kernel::root = new Nodemaster();

Kernel::Kernel() {
	new Substituter();
	Handler::init();
	Kernel::loadSubstitutions();
	Kernel::load("std-startup.aiml");
	Kernel::loadTemporaryData();
}

string Kernel::bootstrap() {
	trimming = false;
	string result = Kernel::respond("bootstrap", "system");
	trimming = true;
	return result;
}

void Kernel::load(const string &file) {
	PreParser *p = new PreParser();
	bool loadedOk = true;
	int total = totalCategories;
	long time = timerMillis();
	ifstream fin;
	fin.open(file.c_str(), ios::in | ios::binary);
	if (fin.is_open()) {
		p->parse(fin,file);
	} else {
		loadedOk = false;
		cout << "File " << file << " could not be found" << endl;
	}
	if (loadedOk) {
		prettyPrintHeader(file, (totalCategories - total), totalCategories, (timerMillis() - time));
//		cout << "Learnt: " << file << " \t(" << (totalCategories - total) << " categories) \t(" << totalCategories << " total) \t(" << (timerMillis() - time) << "ms)" << endl;
	}
	delete p;
}

void Kernel::convertTempAiml() {
	PElement eAiml(new Element("aiml"));
	PElement eCategory,
		ePattern,
		eThat,
		eTopic,
		eTemplate,
		eText;
	
	ifstream fin("temporary.data", ios::in | ios::binary);
	
	if (!fin.is_open()) {
		cerr << "temporary.data does not exist .. cannot oonvert" << endl;
		return;
	}
	while (fin.good()) {
		string pattern, that, topic, _template;
		getline(fin, pattern, ',');
		if (pattern.empty()) {
			break;
		}
		getline(fin, that, ',');
		getline(fin, topic, ',');
		getline(fin, _template);
		eCategory = new Element("category");
		ePattern = new Element("pattern");
		eText = new Element("#text");
		eText->setText(pattern);
		ePattern->addChild(eText);
		eCategory->addChild(ePattern);
		if (that != "*") {
			eThat = new Element("that");
			eText = new Element("#text");
			eText->setText(that);
			eThat->addChild(eText);
			eCategory->addChild(eThat);
		}
		eTemplate = new Element("template");
		eText = new Element("#text");
		eText->setText(_template);
		eTemplate->addChild(eText);
		eCategory->addChild(eTemplate);
		if (topic != "*") {
			eTopic = new Element("topic");
			eTopic->setAttribute("name", topic);
			eTopic->addChild(eCategory);
			eAiml->addChild(eTopic);
		} else {
			eAiml->addChild(eCategory);
		}
	}
	fin.close();
	////////////
	ofstream fout("temporary.aiml", ios::out | ios::binary | ios::trunc);
	if (!fout.is_open()) {
		cerr << "could not open temporary.aiml for writing" << endl;
		return;
	}
	string buffer = AimlWriter::prettyAiml(eAiml);
	if (buffer.empty()) {
		fout.close();
		return;
	}
	fout << buffer << endl;
	fout.close();
}

void Kernel::loadTemporaryData() {
	//	Format: <pattern>,<that>,<topic>,<template><eol>
	ifstream fin("temporary.data", ios::in | ios::binary);
	if (!fin.is_open()) {
		return;
	}
	while (fin.good()) {
		string pattern, that, topic, _template;
		getline(fin, pattern, ',');
		getline(fin, that, ',');
		getline(fin, topic, ',');
		int startIndex = fin.tellg();
		getline(fin, _template);
		int endIndex = fin.tellg();
		
		Nodemaster *node = add(pattern, that, topic);
		if (node != NULL) {
			node->addTemplate(
				new Template("temporary.data", startIndex, endIndex)
			);
			++totalCategories;
		}
	}
	fin.close();
}

void Kernel::loadSubstitutions() {
	ifstream fin;
	PElement root;
	fin.open("substitutions.xml", ios::in | ios::binary);
	
	if (fin.is_open()) {
		Parser *p = new Parser();
		root = p->parse(fin, "substitutions.xml");
		delete p;
		
		if (root == NULL) {
			return;
		}
	} else {
		return;
	}
	
	velement array;
	velement_it ix;
	root->getChildren("substitute", &array);
	for (ix = array.begin(); ix != array.end(); ++ix) {
		string name = (*ix)->getAttribute("name");
		string find = (*ix)->getAttribute("find");
		string replace = (*ix)->getAttribute("replace");
		Substituter::addSubstitute(name, find, replace);
	}
}

/**
 * At A & B we have a problem. Need to fix Element to do a
 * deep copy, so that we can safely delete the parsed tree.
 * Otherwise will crash and burn, or use far more memory
 * than is necessary.
 */
void Kernel::load10(const string &, PElement &root) {
	velement array;
	velement_it ix;
	root->getChildren("substitute", &array);
	for (ix = array.begin(); ix != array.end(); ++ix) {
		string name = (*ix)->getAttribute("name");
		string find = (*ix)->getAttribute("find");
		string replace = (*ix)->getAttribute("replace");
		Substituter::addSubstitute(name, find, replace);
	}
	array.clear();
	
	root->getChildren("processor", &array);
	for (ix = array.begin(); ix != array.end(); ++ix) {
		string tag = (*ix)->getAttribute("tag");
		string loc = (*ix)->getAttribute("class");
		if (Handler::hasProcessor(tag)) {
//	TEMPORARY WORKAROUND
//	Once have properly handled platform specific dynamic
//	linking, will make processors dynamically linked in
//	at startup, rather than statically provided in Handler
			TemplateProcessor::addProcessor((*Handler::processors.find(tag)).second);
			cout << "Adding Processor: " << loc << endl;
		} else {
			cout << "No " << tag << " processor" << endl;
		}
	}
	array.clear();
	
//	Now to actually load the AIML itself...
	root->getChildren("topic", &array);
	//	All the <topic> nodes
	for (ix = array.begin(); ix != array.end(); ++ix) {
		velement categories;
		(*ix)->getChildren("category", &categories);
		//	All of the <category> nodes per <topic>
		string topic = (*ix)->getAttribute("name");
		for (velement_it iy = categories.begin(); iy != categories.end(); ++iy) {
			//	Load each category
			string that = toString((*iy)->getChild("that"));
			string pattern = toString((*iy)->getChild("pattern"));
//			Nodemaster *node = add(pattern, that, topic);
//			if (node != NULL) {
//				//node->addTemplate(templateString((*iy)->getChild("template")));
//				++totalCategories;
//			}
		}
	}
	array.clear();
	
	root->getChildren("category", &array);
	//	All the <category> nodes without a <topic>
	for (ix = array.begin(); ix != array.end(); ++ix) {
		//	Load each category
		string that = toString((*ix)->getChild("that"));
		string pattern = toString((*ix)->getChild("pattern"));
//		Nodemaster *node = add(pattern, that, "*");
//		if (node != NULL) {
//			node->addTemplate(templateString((*ix)->getChild("template")));
//			++totalCategories;
//		}
	}
//	And I think that concludes loading the AIML :-)
}

Nodemaster *Kernel::add(const string &pattern, const string &that, const string &topic) {
	string path = pattern + " <that> " + that + " <topic> " + topic;
//	cout << "ADD PATH: " << path << endl;
	return add(path, root);
}

Nodemaster *Kernel::add(const string &sentence, Nodemaster *parent) {
	StringTokenizer st(sentence, " ");
	int count = st.countTokens();
	return add(st, parent, count, sentence);
}

Nodemaster * Kernel::add(StringTokenizer &st, Nodemaster *parent, int count, const string &sentence) {
	if (count == 0) {
		return parent;
	} else {
		string word = st.nextToken();
		Nodemaster *n = parent->getChild(word);
		if (n == NULL) {
//	new Nodemapper()? It's an ABC...
			n = new Nodemaster();
			parent->addChild(word, n);
		}
		return add(st, n, count - 1, sentence);
	}
}

Match *Kernel::match(const string &input, const string &that, const string &topic) {
	Match *m = NULL;
	string path = input + " <that> " + that + " <topic> " + topic;
//	cout << "MATCH PATH: " << path << endl;
	m = match(root, root, INPUT, path, "", "");
	if (m != NULL) {
		m->setPath(m->getInputPattern() + " : " + m->getThatPattern() + " : " + m->getTopicPattern());
		return m;
	}
	throw(int(MATCH_EXCEPTION));
}

/**
 * Needs optimizing .. perhaps changes to the Nodemaster/Nodemapper
 * will have to be included.
 * For example: input of say 5 words, and 24 wildcard pattern is
 * extremely slow, and will dramatically increase in time to
 * complete as graphmaster size increases.
 *
 * If (max_tokens(pattern) > max_tokens(remaining input))
 * then stop searching this branch.
 *
 * Still, this is not fail proof, but it will lead to faster
 * running times in the average case :-)
 */
/*
Match *Kernel::match(Nodemaster *node, Nodemaster *parent, const string &input, const string &star, const string &path) {
	StringTokenizer st(input, " ");
	int count = st.countTokens();
	Match *m = NULL;
	if (count == 0) {
		if (node->hasTemplate()) {
			m = new Match();
			m->setNode(node);
			//	Is this correct?
			m->addTopicStar(trim(star));
			m->setTopicPattern("*");
			return m;
		} else {
			return NULL;
		}
	}
	string word = st.nextToken();
	string tail = "";
	if (st.hasMoreTokens()) {
		tail = input.substr(word.length() + 1, input.length());
	}
	if (node->getChild("_") != NULL) {
		m = match(node->getChild("_"), node, tail, word, path + " _");
		if (m != NULL) {
			if (!trim(star).empty()) {
				m->addInputStar(trim(star));
			}
			return m;
		}
//		cout << "No _ (" << path << ")" << endl;
	}
	if (node->getChild(word) != NULL) {
		if (word[0] == '<') {
			m = match(node->getChild(word), node, tail, star, "");
		} else {
			m = match(node->getChild(word), node, tail, star, path + " " + word);
		}
		if (m != NULL) {
			if (word == "<that>") {
				m->addInputStar(trim(star));
				m->setInputPattern(trim(path));
			} else
			if (word == "<topic>") {
				m->addThatStar(trim(star));
				m->setThatPattern(trim(path));
			}
			return m;
		}
//		cout << "No " << word << " (" << path << ")" << endl;
	}
	if (node->getChild("*") != NULL) {
		m = match(node->getChild("*"), node, tail, word, path + " *");
		if (m != NULL) {
			if (!trim(star).empty()) {
				m->addInputStar(trim(star));
			}
			return m;
		}
//		cout << "No * (" << path << ")" << endl;
	}
	if (node == parent->getChild("*") || node == parent->getChild("_")) {
//		cout << "Adding word to <star/>" << endl;
		return match(node, parent, tail, star + " " + word, path);
	}
	return NULL;
}*/

//	Begin hopefully more efficient match engine, with @ support :)

Match *Kernel::match(Nodemaster *node, Nodemaster *parent, int state, const string &input, const string &star, const string &path) {
	StringTokenizer st(input);
	
	if (st.countTokens() == 0) {
		if (node->hasTemplate()) {
			Match *m = new Match();	//	How come need to wrap it?
			m->setNode(node);
			m->addTopicStar(trim(star));
			m->setTopicPattern(trim(path));
			return m;
		}
		return NULL;
	}
	
	string word = st.nextToken();
	string tail = "";
	Match *m = NULL;
	
	if (st.hasMoreTokens()) {
		tail = trim(input.substr(word.length()));
	}
	
	int index = constants.find(" " + word + " ");
	
	if (index != string::npos) {
		if (node->getChild(word) != NULL) {
			m = match(node->getChild(word), node, state + 1, tail, "", "");
			if (m != NULL) {
				addStar(m, trim(star), state);
				addPath(m, trim(path), state);
			}
		}
		return m;
	}
	if (node->getChild("_") != NULL) {
		m = match(node->getChild("_"), node, state, tail, word, path + " _");
		if (m != NULL) {
			addStar(m, trim(star), state);
			return m;
		}
	}
	if (node->getChild(word) != NULL) {
		m = match(node->getChild(word), node, state, tail, star, path + " " + word);
		if (m != NULL) {
	//		if (!trim(star).empty()) {
	//			addStar(m, trim(star), state);
	//		}
			return m;
		}
	}
	if (node->getChild("@") != NULL) {
	//	This one is a bit of a mystery to get figured out
		//Vector queries = node.getQueries();
		//for (int ix = 0; ix < queries.size(); ++ix) {
			//String type = (String)queries.elementAt(ix);
			//if (lookup(type, star.trim()) == true) {
				//m = match(node.getChild("@" + type), node, state, tail, word, path + " @" + type);
				//if (m != null) {
					//addStar(m, star.trim(), state);
					//return m;
				//}
			//}
		//}
	}
	if (node->getChild("*") != NULL) {
		m = match(node->getChild("*"), node, state, tail, word, path + " *");
		if (m != NULL) {
			addStar(m, trim(star), state);
			return m;
		}
	}
	if (node == parent->getChild("*") ||
			node == parent->getChild("_") ||
			node == parent->getChild("@")) {
		return match(node, parent, state, tail, star + " " + word, path);
	}
	return NULL;
}

void Kernel::addStar(Match *m, const string &value, int state) {
	if (value.empty()) {
		return;
	}
	switch (state) {
		case INPUT:
			m->addInputStar(value);
			return;
		case THAT:
			m->addThatStar(value);
			return;
		case TOPIC:
			m->addTopicStar(value);
			return;
	}
}

void Kernel::addPath(Match *m, const string &value, int state) {
	if (value.empty()) {
		return;
	}
	switch (state) {
		case INPUT:
			m->setInputPattern(value);
			return;
		case THAT:
			m->setThatPattern(value);
			return;
		case TOPIC:
			m->setTopicPattern(value);
			return;
	}
}

bool Kernel::lookup(const string &, const string &) {
	//Object o = querySets.get(type);
	//if (o == null) {
		//return false;
	//}
	
	//Enumeration items = ((Vector)o).elements();
	//while (items.hasMoreElements()) {
		//String item = (String)items.nextElement();
		//if (item.startsWith("@")) {
			//String subset = item.substring(1);
			//if (lookup(subset, input) == true) {
				//return true;
			//} else {
				//item = subset;
			//}
		//}
		//--	REGEX HERE WOULD ROCK
		//bool result = Pattern.matches(item, input.toUpperCase());
		//if (result == true) {
			//return true;
		//}
	//}
	
	return false;
}

string Kernel::process(Match *m, PElement e, Responder *r, const string &id) {
	return TemplateProcessor::processTemplate(m, e, r, id);
}

string Kernel::respond(const string &input, const string &id) {
	return respond(input, id, 1, false);
}

string Kernel::respond(const string &input, const string &id, bool srai) {
	return respond(input, id, 1, srai);
}

string Kernel::respond(const string &input, const string &id, Responder *r) {
	return respond(input, id, r, 1, false);
}

string Kernel::respond(const string &input, const string &id, int depth, bool srai) {
	/**
	 * Instead of NULL, should be: new TextResponder() or something.
	 * NULL isn't going to work later on
	 */
	return respond(input, id, NULL, depth, srai);
}

static int recursionDepth = 0;

string Kernel::respond(const string &input, const string &id, Responder *r, int, bool srai) {
//	cout << "KERNEL: " << input << ":" << id << endl;
	if (!srai) {
		recursionDepth = 0;
	}
	//	I want this to be configurable...
	if (++recursionDepth > 32) {
		Memory::push("that", "", id);
		cerr << "AIML contains an infinite loop" << endl;
		cerr << "Input involved in loop: " << input << endl;
		throw(int(LOOP_EXCEPTION));
	//	Can't seem to forward exceptions proper .. or even catch it
	//	But no reply will do...
	}
	string currentResponse = "", buffer = "";
	Match *m = NULL;
	if (!srai) {
		Memory::push("input", input, id);
	}
	StringTokenizer st(input, ".?!");
	while (st.hasMoreTokens()) {
		string sentence = trim(st.nextToken());
		if (sentence.length() < 1) {
			continue;
		}
		sentence = Substituter::normalize(sentence);
		string that = Memory::getValue("that", id);
		if (!srai) {
			StringTokenizer stThat(that, ".?!");
			while (stThat.hasMoreTokens()) {
				that = stThat.nextToken();
				that = trim(that);
				Memory::push("that", that, id);
			}
		}
		that = Substituter::normalize(that);
		string topic = Memory::getValue("topic", id);
		topic = Substituter::normalize(topic);
		if (that.empty()) {
			that = "*";
		}
		if (topic.empty()) {
			topic = "*";
		}
		try {
			//	Do timing...
			m = match(sentence, that, topic);
			//	For getting the match...
			string tmpl = "<template>" + m->getTemplate() + "</template>";
			Parser *p = new Parser();
			strstream ss;
			ss << tmpl << endl;
			currentResponse = Kernel::process(m, p->parse(ss, "xxx"), r, id);
			Memory::setValue("beforethat", id, that);
			Memory::setValue("that", id, currentResponse);
			delete p;
		} catch (int &ex) {
			//	CodeWarrior turns MATCH_EXCEPTION into 0
			//	I guess because we get here through stack unwinding,
			//	whereas LOOP_EXCEPTION isn't
			//	Oh, and CW using zero overhead exception handling...
			if (ex == LOOP_EXCEPTION) {
			//	cerr << "Loop exception occured" << endl;
				return "I'm lost for words";
			}
			if (ex == MATCH_EXCEPTION) {
				cerr << "There is no match for input: " << sentence << endl;
				//	Was crashing without this in some cases
				m = NULL;
			}
		}
		if (m != NULL) {
		//	cout << "PATH: " << m->getPath() << endl;
			delete m;
			if (srai) {
				--recursionDepth;
				return currentResponse;
			} else {
				buffer += currentResponse + " ";
			}
		}
	}
	string result = buffer;//Substituter::output(buffer);
	//	Aha! Here it is...
	--recursionDepth;
	if (trimming) {
		return trim(result, " \t\r\n");
	}
	return result;
}
