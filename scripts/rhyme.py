#!/usr/bin/python
import shelve
import sys
import string
import os
import os.path

if (os.environ.has_key("RHYMEPATH")):
    RHYMEPATH = os.environ["RHYMEPATH"]
else:
    RHYMEPATH = "scripts"

WordNotFound = "word not found"
VERSION = "0.1"
# Copied from official rhymescripts v0.3
class Rhyme:
    def __init__(self, path = RHYMEPATH):
        wordsPath = "%s/words.db" % path
        rhymesPath = "%s/rhymes.db" % path
        multiplePath = "%s/multiple.db" % path
        if os.path.isfile(wordsPath): self.words = shelve.open(wordsPath)
        else: print "ERROR: could not open words DB at", wordsPath
        if os.path.isfile(rhymesPath): self.rhymes = shelve.open(rhymesPath)
        else: print "ERROR: could not open rhymes DB at", rhymesPath
        if os.path.isfile(multiplePath): self.multiple = shelve.open(multiplePath)
        else: print "ERROR: could not open multiple DB at", multiplePath
        
    def getKey(self, word):
        try:
            return string.split(self.words[string.upper(word)], " ")[0]
        except KeyError:
            raise WordNotFound, string.lower(word)

    def getSyllables(self, word):
        try:
            return int(string.split(self.words[string.upper(word)], " ")[1])
        except KeyError:
            raise WordNotFound, string.lower(word)

    def getPronunciations(self, word):
        try:
            return string.split(self.multiple[string.upper(word)], " ")
        except KeyError:
            return [string.upper(word)]

    #returns a dictionary of lists
    #the keys are an int of the number of syllables
    #and the values are an alphabetized list of rhymes
    def rhyme(self, word):

        #inverts a dictionary, turning key->value into value->[keys]
        def invert(dict):
            inverted = {}

            for key in dict.keys():
                if (inverted.has_key(dict[key])):
                    inverted[dict[key]].append(string.lower(key))
                else:
                    inverted[dict[key]] = [string.lower(key)]

            return inverted

        
        toreturn = {}

        for pronunciation in self.getPronunciations(word):
            for rhyme in string.split(self.rhymes[self.getKey(pronunciation)],
                                      " "):
                if (not toreturn.has_key(rhyme)):
                    toreturn[rhyme] = self.getSyllables(rhyme)

        return invert(toreturn)

    #returns a tuple containing the minimum and maximum syllables
    #for a given word.. ("whitening" returns (2,3), for example)
    def syllables(self, word):
        min = 0
        max = 0
        
        for pronunciation in self.getPronunciations(word):
            syllables = self.getSyllables(pronunciation)
            if (min == 0):
                min = syllables
                max = syllables
            else:
                if (syllables > max):
                    max = syllables
                if (syllables < min):
                    min = syllables

        return (min, max)

if __name__ == "__main__":
    import re
    import sys
    rhymer = Rhyme()
    convertRE = re.compile("[^a-zA-Z0-9-]")
    for word in sys.argv[1:]:
        word = re.sub(convertRE, "", word)
        try:
            results = rhymer.rhyme(word)
        except WordNotFound:
            print "I don't know any rhymes for %s." % word
            continue
        for syl, rhymes in results.items():
            print str(syl), "syllable rhymes for " + word + ": ",
            for rhyme in rhymes: print rhyme + ",",
            print "etc." 