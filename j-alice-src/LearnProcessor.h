/**
 * LearnProcessor - Learns AIML files (for now)
 *
 * @author	Jonathan Roewen
 */
#ifndef LEARN_PROCESSOR_H
#define LEARN_PROCESSOR_H

#include "AimlProcessor.h"
#include "Substituter.h"
#include "Parser.h"
#include "Kernel.h"
#include "Nodemaster.h"
#include "Utils.h"

#include <string>
#include <fstream>

using namespace std;

extern int totalCategories;

class LearnProcessor : public AimlProcessor
{
public:
	LearnProcessor() {
		init = false;
	}
	~LearnProcessor() {
	}
	
	string getName() const {
		return "learn";
	}
	string getVersion() const {
		return "1.0";
	}
	string process(Match *m, PElement e, Responder *r, const string &id) {
		PElement pattern = e->getChild("pattern");
		if (pattern == PElement()) {
			//	<learn> a file
			string name = Kernel::process(m, e, r, id);
			Kernel::load(name);
		} else {
			openFile();
			
			if (!init) {
				return "";
			}
			//	<learn> new AIML defined in a <pattern>
			
			//	This needs fixing!
			//	Need to process template, do an output substitution
			//	to change &lt; etc into '<' etc
			//	then parse it...
		//	PElement temp = e->getChild("template");
		/******************************************/
		//	strstream ss;
			string templateText = Kernel::process(m, e->getChild("template"), r, id);
		//	cout << Substituter::output(str) << endl;
		//	ss << Substituter::output(str) << endl;
		//	Parser *p = new Parser();
		//	PElement temp = p->parse(ss, "template");
		/******************************************/
			//	Need to do something similar with <pattern>
		//	strstream s2;
			string patternText = Kernel::process(m, e->getChild("pattern"), r, id);
		//	s2 << str2 << endl;
		//	pattern = p->parse(s2, "template");
		//	PElement that = e->getChild("that");
			//	g-o-d's double learning won't work with this yet
		//	PElement category(new Element("category"));
		//	category->addChild(pattern);
		//	category->addChild(temp);
		//	if (that != PElement()) {
		//		category->addChild(that);
		//	}
		//	PElement root(new Element("aiml"));
		//	root->addChild(category);
		//	Kernel::load10("~temporary memory area~", root);
			//	Now just need to write to a file...
			
			////    NEW CODE HERE    ////
						
			templateText = removeNewlines(templateText);
			
			learnFile << patternText << ",*,*,";
			streamsize startIndex = learnFile.tellg();
			learnFile << templateText;
			streamsize endIndex = learnFile.tellg();
			learnFile << endl;
			cout << "begin = " << startIndex << " end = " << endIndex << endl;
			Nodemaster *node = Kernel::add(patternText, "*", "*");
			if (node != NULL) {
				++totalCategories;
				node->addTemplate(
					new Template("temporary.data", startIndex, endIndex)
				);
			}
			
			closeFile();
		}
		return "";
	}
private:
	string removeNewlines(const string &input) {
		string output = "";
		for (int ix = 0; ix < input.length(); ++ix) {
			if (input[ix] == '\n' || input[ix] == '\r') {
				continue;
			}
			output += input[ix];
		}
		return output;
	}
	
	void openFile() {
		learnFile.open("temporary.data", ios::out | ios::binary | ios::app);
		if (learnFile.is_open()) {
			learnFile.seekp(0, ios::end);
			init = true;
		}
	}
	
	void closeFile() {
		if (init) {
			learnFile.close();
		}
		init = false;
	}
	
	fstream learnFile;
	bool init;
};

#endif
