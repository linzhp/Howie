"""
Handles the reading, writing of (and general access to) Howie's
persistent configuration information, stored (by default) in
the howie.ini file.
"""

import os
import string
import sys

import ConfigParser
from howie import frontends
from frontends import *

# Load default values for various config-file options.
# The defaults are scattered throughout the frontend and
# backend modules, each of which may have a dictionary
# attribute called configDefaults similar to the one
# below.  This code merges all the configDefaults into
# one master copy.
configDefaults = {
    "general.botname":		"Howie",
    "general.botmaster":	"Your Name",
    "general.maxdelay":     "0",
    "general.verbose":      "no",
    }
for package in ['frontends']:
    for name in eval("%s.__all__" % package):
        mod = eval("%s.%s" % (package, name))
        try: defaults = mod.configDefaults
        except AttributeError: continue
        for k,v in mod.configDefaults.items():
            if configDefaults.has_key(k):
                continue
            configDefaults[k] = v


def __WriteINI(file, config=configDefaults):
	"""
	Writes the config dictionary to a .INI file.
	"""
	if len(config) == 0:
		return
	iniFile = open(file, "w")
	currentSection = ""
	keys = config.keys()
	keys.sort()
	for key in keys:
		# Don't write entries with no value
		value = config[key]
		if value == "":
			continue

		# Split the key into section and variable.		
		section, var = string.split(key, ".", 1)
		if section != currentSection:
			# start a new section
			if currentSection:
				iniFile.write("\n")
			iniFile.write("[%s]\n" % section)
			currentSection = section

		# write the entry
		iniFile.write("%s = %s\n" % (var, value))
	iniFile.close()

def __ReadINI(file, config={}):
	"""
	Returns a dictionary with keys of the form
	<section>.<option> and the corresponding values.
	"""
	config = config.copy()
	cp = ConfigParser.ConfigParser()
	cp.read(file)
	for sec in cp.sections():
		name = string.lower(sec)
		for opt in cp.options(sec):
			config[name + "." + string.lower(opt)] = string.strip(cp.get(sec, opt))
	return config


__config = {}
def load(file):
    """
    Returns a dictionary containing the current configuration information,
    read from the specified .INI file.
    """
    global __config, configDefaults
    try:
        if not os.path.exists(file):
            __WriteINI(file)
        __config = __ReadINI(file, configDefaults)
    except:
        print "ERROR: could not load config file %s" % file
        sys.exit(-1)
    return __config

def get():
    global __config
    return __config