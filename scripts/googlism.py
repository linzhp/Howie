#!/usr/bin/python
import mutex
import random
import re
import shelve
import string
import urllib
import sys

class Googlismer:
	def __init__(self):
		self.__cmdRegex = re.compile("(?P<qtype>who|what|where|when) (?P<verb>is|are|was|were) (?P<query>[^?]+)", re.I)
		self.__cacheFile = "googlism_cache"
		try: self.__cache = shelve.open(self.__cacheFile, protocol=2)
		except TypeError:
			# pre-2.3 didn't have a protocol argument.
			self.__cache = shelve.open(self.__cacheFile)
		self.__cacheMutex = mutex.mutex()
		self.__cacheKeys = self.__cache.keys()
	
	def __addToCache(self, (key,value)):
		"""
		Adds the key/value pair to the cache.  If the key already exists, it
		is updated with the new value.  If the cache has grown too large, it
		is pruned.
		"""
		maxCacheSize = 64
		
		# TODO: Keep a priority queue of (timestamp, key) pairs in memory.  Use
		# that pq to intelligently remove the LRU cache entry.  YEAH!
		if len(self.__cacheKeys) > maxCacheSize:
			# Randomly delete half the cache entries
			for deadKey in self.__cache.keys():
				if random.random() < 0.5:
					self.__cache[deadKey] = []
					del self.__cache[deadKey]
			self.__cache.sync()
		self.__cache[key] = value
		self.__cacheKeys.append(key)
		self.__cacheMutex.unlock()
	
	# Legal values for sSearchType: who, what, where, when
	def __googlisms(self, sInput, sSearchType = "what"):
		"Retrieve a list of Googlisms for the string sInput."
		
		# Check for results in cache.
		cacheKey = string.lower("%s,%s" % (sSearchType, sInput))
		if self.__cache.has_key(cacheKey):
			return self.__cache[cacheKey]
		
		nType = 2 # a "what" search
		if string.lower(sSearchType) == "who":
			nType = 1
		elif string.lower(sSearchType) == "what":
			nType = 2
		elif string.lower(sSearchType) == "where":
			nType = 3
		elif string.lower(sSearchType) == "when":
			nType = 4
		else:
			print "ERROR: invalid search type.  must be 'who', 'what', 'where' or 'when'."
			return []
		
		# Fetch the results.
		url = "http://www.googlism.com/index.htm?ism=" + \
			string.replace(urllib.quote(sInput.strip()), " ", "+") + \
			"&type=" + str(nType)
		try: result_page = urllib.urlopen(url)
		except IOError:
			print "ERROR: Could not connect to Googlisms server."
			return {}
		lines = result_page.readlines()
		
		# Extract the useful results
		results = []
		pattern = string.lower(sInput.strip()) + " is (?P<def>.*)\<br\>"
		googlism_regex = re.compile(pattern)
		for line in lines:
			match = googlism_regex.search(line)
			if match is not None:
				results.append(match.group('def'))
		
		# Store results in cache
		self.__cacheMutex.lock(self.__addToCache, (cacheKey,results))
		
		return results
	
	def respondTo(self, input):
		"""
		If input matches the form of a legal Googlism query (e.g. "Who is X?"),
		returns a random answer from the Googlism server.  None is returned
		if an error occurs.
		"""

		# Make sure the input is a Googlism-type question.
		result = self.__cmdRegex.search(input)
		if result is None:
			return None

		# Fetch a list of Googlism answers to the input question, and return
		# a random selection.
		googs = self.__googlisms(result.group('query'), result.group('qtype'))
		if len(googs) != 0:
			return "%s %s %s" % (result.group('query'), string.lower(result.group('verb')),
								 random.choice(googs))
		else:
			return "I don't know %s you're talking about." % string.lower(result.group('qtype'))

def main():
	query = string.join(sys.argv[1:])
	googler = Googlismer()
	response = googler.respondTo(query)
	print response

# Run main.
if __name__ == "__main__":
    main()  		
