/**
 * Match - The foundation of matches
 *
 * @author	Jonathan Roewen
 */
#include "Match.h"

#include "Nodemaster.h"

void Match::addInputStar(const string &s) {
	inputStar.push_back(s);
}

void Match::addThatStar(const string &s) {
	thatStar.push_back(s);
}

void Match::addTopicStar(const string &s) {
	topicStar.push_back(s);
}

void Match::setInputPattern(const string &s) {
	inputPattern = s;
}

void Match::setThatPattern(const string &s) {
	thatPattern = s;
}

void Match::setTopicPattern(const string &s) {
	topicPattern = s;
}

string Match::getInputPattern() const {
	return inputPattern;
}

string Match::getThatPattern() const {
	return thatPattern;
}

string Match::getTopicPattern() const {
	return topicPattern;
}

void Match::setPath(const string &s) {
	path = s;
}

string Match::getPath() const {
	return path;
}

void Match::setNode(Nodemaster *n) {
	node = n;
}

Nodemaster *Match::getNode() const {
	return node;
}

string Match::getTemplate() const {
	return node->getTemplate();
}

string Match::getInputStar(unsigned int i) const {
	if (i < 1 || i > inputStar.size()) {
		return "";
	}
	return inputStar[inputStar.size() - i];
}

string Match::getThatStar(unsigned int i) const {
	if (i < 1 || i > thatStar.size()) {
		return "";
	}
	return thatStar[thatStar.size() - i];
}

string Match::getTopicStar(unsigned int i) const {
	if (i < 1 || i > topicStar.size()) {
		return "";
	}
	return topicStar[topicStar.size() - i];
}

unsigned int Match::getInputStarCount() const {
	return inputStar.size();
}

unsigned int Match::getThatStarCount() const {
	return thatStar.size();
}

unsigned int Match::getTopicStarCount() const {
	return topicStar.size();
}
