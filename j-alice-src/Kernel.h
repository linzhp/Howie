/**
 * Kernel - Brains of the operation
 *
 * @author	Jonathan Roewen
 */
#ifndef KERNEL_H
#define KERNEL_H

#include <string>

using namespace std;

#include "Element.h"	//	?

#define	LOOP_EXCEPTION	0001
#define MATCH_EXCEPTION	0002

class Match;
class Responder;
class Nodemaster;
class StringTokenizer;

class Kernel
{
public:
	Kernel();

	static string bootstrap();
	static void load(const string &file);
	static void load10(const string &file, PElement &root);
	static void loadSubstitutions();
	static Nodemaster * add(const string &pattern, const string &that, const string &topic);
	static Nodemaster *add(const string &sentence, Nodemaster *parent);
	static Nodemaster *add(StringTokenizer &st, Nodemaster *parent, int count, const string &sentence);
	static Match *match(const string &input, const string &that, const string &topic);
	static Match *match(Nodemaster *node, Nodemaster *parent, int state, const string &input, const string &star, const string &path);
	static string process(Match *m, PElement e, Responder *, const string &id);
	static string respond(const string &input, const string &id);
	static string respond(const string &input, const string &id, bool srai);
	static string respond(const string &input, const string &id, Responder *);
	static string respond(const string &input, const string &id, int depth, bool srai);
	static string respond(const string &input, const string &id, Responder *, int depth, bool srai);
	
	static void convertTempAiml();
private:
	static void addStar(Match *, const string &, int);
	static void addPath(Match *, const string &, int);
	static bool lookup(const string &, const string &);
	static void loadTemporaryData();
	
	static Nodemaster *root;
	static const int INPUT, THAT, TOPIC;
	static const string constants;
};

#endif 
